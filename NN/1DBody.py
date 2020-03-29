import tensorflow as tf
#tf.executing_eagerly()
import pickle
import sys
import time
import os
os.environ['TF_CPP_MIN_LOG_LEVEL']='3'
import numpy as np
from SMPL.test_runner import GetSmplModel
from SMPL.batch_smpl import SMPL
from CustomLoss import *
np.set_printoptions(threshold=sys.maxsize)
physical_devices = tf.config.list_physical_devices('GPU') 
print("Num GPUs:", len(physical_devices),'\n',physical_devices,'\n\n') 


class MyModel(tf.keras.Model):
  def __init__(self, batchSize = 32):
    super(MyModel, self).__init__()
    self.d0 = tf.keras.layers.Dense(100,input_shape=(14,), name="Den0") #, activation = 'relu')
    self.d1 = tf.keras.layers.Dense(100, name="Den1")
    self.d2 = tf.keras.layers.Dense(100, name="Den2")
    self.d3 = tf.keras.layers.Dense(100, name="Den3")
    self.d4 = tf.keras.layers.Dense(100, name="Den4")
    self.d5 = tf.keras.layers.Dense(60, name="Den5")
    self.d6 = tf.keras.layers.Dense(20, name="Den6")

    self.betas = tf.keras.layers.Dense(10,name="betas")
    self.theta = tf.Variable(tf.zeros((batchSize,1,72)),trainable=False, name='theta')
    for i in range(batchSize):
      self.theta[i,0,41].assign(-0.2) # Left shoulder z
      self.theta[i,0,44].assign(0.2) # right shoulder z
      self.theta[i,0,50].assign(-0.8) # Left shoulder z
      self.theta[i,0,53].assign(0.8) # right shoulder z
    self.SMPL = SMPL("SMPL/models/neutral_smpl_with_cocoplus_reg.pkl")

    with open('IndexForBody.pkl', 'rb') as f:
      self.index = pickle.load(f)
    
    #print(self.chest.shape,type(self.chest))

  def call(self, x):
    x = self.d0(x)
   # x = self.norm1(x)
    x = self.d1(x)
   # x = self.norm2(x)
    x = self.d2(x)
   # x = self.norm2(x)
    x = self.d3(x)
  #  x = self.norm2(x)
    x = self.d4(x)
    x = self.d5(x)
    x = self.d6(x)



    betas = self.betas(x)
    # add betas loss. PCA should not be too big or small
    betaLoss = tf.reduce_mean(tf.math.pow(betas, 2)) * 3e-5
    self.add_loss(betaLoss)
    tf.print('\n',betas[0],summarize=-1)

    #tf.print('\nB  = ',betas[0], summarize=-1)
    batchSize = tf.shape(input=betas)[0]
    #theta = tf.zeros((batchSize,1,72),dtype='float32')
    verts, _, _ = self.SMPL(betas, self.theta, get_skin=True)
    y_pred = CollectPred(verts,self.index)
    return y_pred

def MSE(y_true, y_pred):
  #tf.print(y_pred,'\n\n\n\n')
  tf.print('True  = ',y_true[0],summarize=-1)
  tf.print('Pred  = ',y_pred[0],summarize=-1)
  diff = tf.math.abs( tf.math.subtract(y_true, y_pred) )
  accuracy = tf.math.divide (diff,y_true)
  acc = tf.reduce_mean(accuracy,0) * 100
  tf.print("Err   = ",acc,summarize=-1)
  #tf.print("         Height UpChest LowChest Waist Hip Thigh Calf ")
  #return  tf.keras.losses.MSE(y_true,y_pred)
  loss = tf.math.square( tf.math.subtract(y_pred, y_true) )
  #tf.print('Loss = ',loss[0],summarize=-1)
  return loss



# height, chest, waist, hip, thigh, calf, upperArm, arm, leg, back, upperBody
data = np.loadtxt("Female.txt").astype(np.float32) 
data = np.delete(data, 2, 1) # delete lower chest

batchSize = 32# let the size of dataset match the batch size 32
n = data.shape[0] 
nBatch = int(n/batchSize) # Step
data = data[0:batchSize*nBatch]

y_train = data[0:batchSize*(nBatch-3)]
y_test = data[batchSize*(nBatch-3):batchSize*nBatch]

mean = np.mean(data, axis = 0)#Standardize data
std = np.std(data, axis=0)
data = (data - mean) / std
x_train = data[0:batchSize*(nBatch-3)]
x_test = data[batchSize*(nBatch-3):batchSize*nBatch]


model = MyModel(batchSize=batchSize)
epoch = 30
#initial_learning_rate = 0.05
#lr_schedule = tf.compat.v1.train.exponential_decay(initial_learning_rate,
#  200, nBatch , 0.96, staircase=True)

adam = tf.keras.optimizers.Adam(learning_rate = 0.001)
RMSprop = tf.keras.optimizers.RMSprop(learning_rate=0.0001)
model.compile(optimizer=RMSprop,
              loss=MSE,#'mean_squared_error',
              metrics=['mean_squared_error'])

tf.keras.utils.plot_model(model, 'a.png', show_shapes=True)

#print(result,result.shape,type(result))
#vert = np.zeros((1472,6890,3),dtype='float32') # It is used to make the model ouput vertex info when use function 'call'
#y_train = [y_train, vert]
model.fit(x_train, y_train, epochs=epoch,batch_size=batchSize ,verbose=1)
model.summary()
model.evaluate(x_test, y_test)






'''
train_dataset = tf.data.Dataset.from_tensor_slices((x_train,y_train))
train_dataset = train_dataset.shuffle(buffer_size=2000).batch(batchSize)

epochs = 15

# Iterate over epochs.
for epoch in range(epochs):
  verts = None
  # Iterate over the batches of the dataset.
  for step, x in enumerate(train_dataset):
    with tf.GradientTape() as tape:
      #print(x.shape)
      pred, verts = model(x[0])
      # Compute reconstruction loss
      loss = heightLoss(x[1], pred)

    grads = tape.gradient(loss, model.trainable_weights)
    RMSprop.apply_gradients(zip(grads, model.trainable_weights))

    print('Epoch = ',epoch,', Step = ',step)
    print('Loss = ', tf.math.reduce_mean(loss))
    print('pred = ', pred[0])
  
  result = verts[0]
  if epoch == epochs-1:
    faces = np.load(os.path.join('SMPL/', 'smpl_faces.npy'))
    outmesh_path = os.path.join( 'ResultCustomLoop.obj')
    with open(outmesh_path, 'w') as fp:
      for v in result:
        fp.write('v %f %f %f\n' % (v[0], v[1], v[2]))

      for f in faces + 1:
        fp.write('f %d %d %d\n' % (f[0], f[1], f[2]))

@tf.function
def train_step(model, data, labels):
    with tf.GradientTape() as tape:
      pred, verts = model(data)
      loss = heightLoss(labels, pred)
    gradients = tape.gradient(loss, model.trainable_variables)
    RMSprop.apply_gradients(zip(gradients, model.trainable_variables))

for epoch in range(epochs):
  print('Epoch = ',epoch)
  n = 0
  for data, labels in train_dataset:
    print(n)
    n+=1
    train_step(model, data, labels)'''