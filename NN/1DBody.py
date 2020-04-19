import tensorflow as tf
#tf.executing_eagerly()
import pickle
import sys
import time
import datetime
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
    self.d0 = tf.keras.layers.Dense(100,input_shape=(15,), name="Den0") #, activation = 'relu')
    #self.d1 = tf.keras.layers.Dense(100, name="Den1")
    #self.d2 = tf.keras.layers.Dense(100, name="Den2")
    #self.d3 = tf.keras.layers.Dense(100, name="Den3")
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
   # x = self.d1(x)
   # x = self.norm2(x)
   # x = self.d2(x)
   # x = self.norm2(x)
   # x = self.d3(x)
  #  x = self.norm2(x)
    x = self.d4(x)
    x = self.d5(x)
    x = self.d6(x)



    betas = self.betas(x)
    # add betas loss. PCA should not be too big or small
    betaLoss = tf.reduce_mean(tf.math.pow(betas, 2)) * 3e-5
    self.add_loss(betaLoss)
    #tf.print('\n',betas[0],summarize=-1)
    tf.print('\n[',betas[0][0],',',betas[0][1],',',betas[0][2],',',betas[0][3],',',betas[0][4],',',
              betas[0][5],',',betas[0][6],',',betas[0][7],',',betas[0][8],',',betas[0][9],']')
    #tf.print('\nB  = ',betas[0], summarize=-1)
    #batchSize = tf.shape(input=betas)[0]
    #theta = tf.zeros((batchSize,1,72),dtype='float32')
    verts, _, _ = self.SMPL(betas, self.theta, get_skin=True)
    #y_pred = CollectPred(verts,self.index)
    #return y_pred
    return verts

def MSE( y_true, y_pred):
  y_pred = CollectPred(y_pred)
  #tf.print(y_pred,'\n\n\n\n')
  tf.print('True  = ',y_true[0],summarize=-1)
  tf.print('Pred  = ',y_pred[0],summarize=-1)
  diff = tf.math.abs( tf.math.subtract(y_true, y_pred) , name='ABS')
  accuracy = tf.math.divide (diff,y_true, name='Divide')
  acc = tf.reduce_mean(accuracy,0, name='Reduce_Mean') * 100
  tf.print("Err   = ",acc,summarize=-1)
  #tf.print("         Height UpChest LowChest Waist Hip Thigh Calf ")
  #return  tf.keras.losses.MSE(y_true,y_pred)
  loss = tf.math.square( tf.math.subtract(y_pred, y_true) , name='Square' )
  betaWeight = tf.constant([1,1,1,1,1, 1,1,1,1,1, 1,1,1,1,1],dtype=np.float32) # chestDepth is 3
  loss = loss * betaWeight
  #tf.print('Loss = ',loss[0],summarize=-1)
  return loss



# height, chest, waist, hip, thigh, calf, upperArm, arm, leg, back, upperBody
data = np.loadtxt("Mix.txt").astype(np.float32) 
data = np.delete(data, 12, 1) # delete chest depth for the convenience of user testing

np.random.shuffle(data)


batchSize = 32# let the size of dataset match the batch size 32
n = data.shape[0] 
nBatch = int(n/batchSize) # Step
data = data[0:batchSize*nBatch]

y_train = data[0:batchSize*(nBatch-10)]
y_test = data[batchSize*(nBatch-10):batchSize*nBatch]

mean = np.mean(data, axis = 0)#Standardize data
std = np.std(data, axis=0)
data = (data - mean) / std

x_train = data[0:batchSize*(nBatch-10)]
x_test = data[batchSize*(nBatch-10):batchSize*nBatch]

print(mean, std)
print(x_train[0], x_test[0], y_train[0], y_test[0])

'''
model = MyModel(batchSize=batchSize)
# Include the epoch in the file name (uses `str.format`)
checkpoint_path = "model/mix/cp.ckpt"
checkpoint_dir = os.path.dirname(checkpoint_path)
# Create a callback that saves the model's weights every 5 epochs
cp_callback = tf.keras.callbacks.ModelCheckpoint(
    filepath=checkpoint_path, 
    verbose=1, 
    save_weights_only=True,
    save_freq=y_train.shape[0]*10) # save weights every 10 epochs

epoch = 30
RMSprop = tf.keras.optimizers.RMSprop(learning_rate=0.0001)
model.compile(optimizer=RMSprop,
              loss=MSE,#'mean_squared_error',
                           )


tf.keras.utils.plot_model(model, 'a.png', show_shapes=True)

#print(result,result.shape,type(result))
#vert = np.zeros((1472,6890,3),dtype='float32') # It is used to make the model ouput vertex info when use function 'call'
#y_train = [y_train, vert]
log_dir = "logs/fit/" + datetime.datetime.now().strftime("%Y%m%d-%H%M%S") + "_Mix"
tensorboard_callback = tf.keras.callbacks.TensorBoard(log_dir=log_dir, histogram_freq=1)

model.fit(x_train, y_train, epochs=epoch,batch_size=batchSize ,verbose=1,
  callbacks=[cp_callback,tensorboard_callback], validation_data=(x_test, y_test) )
model.summary()
model.evaluate(x_test, y_test)
model.save('MixModel') 
'''