import tensorflow as tf
#tf.executing_eagerly()
import pickle
import sys
import time
import time
import os
os.environ['TF_CPP_MIN_LOG_LEVEL']='3'
import numpy as np
from SMPL.test_runner import GetSmplModel
from SMPL.batch_smpl import SMPL
from CustomLoss import *
np.set_printoptions(threshold=sys.maxsize)


'''
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
    #tf.print('\n',betas[0],summarize=-1)
    tf.print('\n[',betas[0][0],',',betas[0][1],',',betas[0][2],',',betas[0][3],',',betas[0][4],',',
              betas[0][5],',',betas[0][6],',',betas[0][7],',',betas[0][8],',',betas[0][9],']')
    #tf.print('\nB  = ',betas[0], summarize=-1)
    batchSize = tf.shape(input=betas)[0]
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
  diff = tf.math.abs( tf.math.subtract(y_true, y_pred) )
  accuracy = tf.math.divide (diff,y_true)
  acc = tf.reduce_mean(accuracy,0) * 100
  tf.print("Err   = ",acc,summarize=-1)
  #tf.print("         Height UpChest LowChest Waist Hip Thigh Calf ")
  #return  tf.keras.losses.MSE(y_true,y_pred)
  loss = tf.math.square( tf.math.subtract(y_pred, y_true) )
  betaWeight = tf.constant([1,1,1,1,1, 1,1,1,1,1, 1,1,3,1,1,1],dtype=np.float32) # chestDepth is 3
  loss = loss * betaWeight
  #tf.print('Loss = ',loss[0],summarize=-1)
  return loss
'''

data = np.ones((32,15))
mean = [1.70343, 1.0322145, 0.88027215, 0.895398, 1.0549142, 0.5642951, 0.37465364,
        0.54686505, 0.7503665,  0.39009097, 1.3823942, 1.222587, 0.85598695, 0.43910486, 0.2697188 ]
std =  [0.09938579, 0.12538183, 0.14000037, 0.15226765, 0.11731844, 0.05897906, 0.0364428,
        0.03658748, 0.0499182,  0.03276951, 0.08317201, 0.08408944, 0.05824336, 0.03547727, 0.03816335] 

'''
model =  MyModel()
RMSprop = tf.keras.optimizers.RMSprop(learning_rate=0.0001)
model.compile(optimizer=RMSprop,
              loss=MSE,#'mean_squared_error',
                           )


model.load_weights('model/cp.ckpt')
start = time.perf_counter()
result = model.predict(x_test[0:32])
elapsed = time.perf_counter() - start
print('Elapsed %.3f seconds.' % elapsed)


start = time.perf_counter()
result = model.predict(x_test[0:32])
elapsed = time.perf_counter() - start
print('Elapsed %.3f seconds.' % elapsed)
'''
#-------------------------------------------------------

faces = np.load('SMPL/smpl_faces.npy')

#         [height, chest, lowerChest, waist, hip, thigh, calf, arm, leg, back, shouderH, chestH, hipH, kneeH, hipDepth]
data[0] = [1.70,   1.10,  1.00,        0.95, 0.96, 0.65,0.42, 0.5, 0.787,0.47, 1.47,     1.25,  0.864,0.39,  0.259 ]
data[0:32] = data[0]
data = (data-mean) / std

wholeModel = tf.keras.models.load_model('MixModel')
start = time.perf_counter()
result = wholeModel.predict(data[0:32])
elapsed = time.perf_counter() - start
#print('Elapsed %.3f seconds.' % elapsed)

'''
start = time.perf_counter()
result = wholeModel.predict(data[0:32])
elapsed = time.perf_counter() - start
print('Model %.3f seconds.' % elapsed)
'''

phase2 = time.perf_counter()
result = result[0]
outmesh_path = '0417.obj'
with open(outmesh_path, 'w') as fp:
  for v in result:
    fp.write('v %f %f %f\n' % (v[0], v[1], v[2]))

  for f in faces + 1:
    fp.write('f %d %d %d\n' % (f[0], f[1], f[2]))
end = time.perf_counter() - phase2
print('Output %.3f seconds.' % end)


'''
from SMPL.test_runner import GetSmplModel
from SMPL.batch_smpl import SMPL
smpl = SMPL("SMPL/models/neutral_smpl_with_cocoplus_reg.pkl")
betas = np.zeros((32,10),dtype=np.float32)
theta = np.zeros((32,1,72),dtype=np.float32)
smplStart = time.perf_counter()
verts, _, _ = smpl(betas, theta, get_skin=True)
smplTime = time.perf_counter() - smplStart
print('SMPL %.3f seconds.' % smplTime)

smplStart = time.perf_counter()
verts, _, _ = smpl(betas, theta, get_skin=True)
smplTime = time.perf_counter() - smplStart
print('SMPL %.3f seconds.' % smplTime)
'''