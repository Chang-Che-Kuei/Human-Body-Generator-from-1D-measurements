import tensorflow as tf
import numpy as np
from tensorflow import math# pow, square
# index of body
chestI = np.array([4160,4087,4090,4175,4173,4180,6332,3078,2872,692,685,687,600,599,670,669,1422,1425,
614,1198,1764,1423,645,644,738,764,2907,1254,1237,751,752,3015,4240,4241,4720,4737,6367,4253,4252,4225,
4134,4133,4897,5231,4683,4100,4899,4895,4157,4160])
lowerChestI = np.array([1330,1201,928,929,659,660,1758,1759,942,620,621,1270,795,796,2845,2844,1213,768,
	767,3018,4257,4256,4696,6306,6305,6304,4284,4753,4109,4110,4428,5226,5225,4150,4147,4416,4414,4689])
#lowerChestI = np.array([3512,2849,927,926,2846,617,618,3481,2848,2840,2841,2852,1268,2826,
	#1496,2828,2827,2839,3174,6299,6288,6289,4968,6287,4749,6313,6302,6301,6309,6879,4104,
	#4105,6307,4413,4412,6310,3512])
waistI =  np.array([3501,1337,918,917,920,680,679,706,939,847,831,832,845,846,887,2929,2930,1780,
1783,3022,5246,5245,6389,6390,4374,4331,4332,4317,4318,4333,4194,4167,4168,4407,4406,4404,4403,4813,3501])
hipI =  np.array([3120,6542,6540,6541,6560,6559,6558,6510,4920,4921,4928,6550,4986,4401,4399,4692,4349,4350,3511,866,
863,1207,913,915,1513,3129,1455,1447,1448,3085,3137,3138,3139,3117,3118,3119,3120])
thighI =  np.array([4397,4437,4436,4929,4455,4454,4934,4933,4936,4459,4458,4391,4392,4444,4393,4394,4839,4397])
calfI =  np.array([4590,4588,4587,4582,4583,4942,4940,4938,4580,4579,5000,4575,4576,4642,4845,4590])
upperArmI =  np.array([790,628,629,1397,1398,1383,1382,1386,1233,1234,1389,1390,1394,1395,1379,1380,1316,1312,790])
chestI -= 1
lowerChestI -= 1
waistI -= 1
hipI -= 1
thighI -= 1
calfI -= 1
upperArmI -= 1
X, Y, Z = 0, 2, 1

def CollectPred(verts):
	batchSize = tf.shape(verts)[0]

	height    = Height(verts, batchSize)
	# Circumference
	chest     = XYplane(verts, chestI, batchSize)
	lowerChest= XYplane(verts, lowerChestI, batchSize)
	waist     = XYplane(verts, waistI, batchSize)
	hip       = XYplane(verts, hipI, batchSize)
	thigh     = XYplane(verts, thighI, batchSize)
	calf      = XYplane(verts, calfI, batchSize)
	#upperArm  = YZplane(verts, upperArmI, batchSize)
	# Length
	arm       = Arm(verts, batchSize)
	leg       = Leg(verts, batchSize)
	back      = Back(verts, batchSize)
	shouderH  = ShoulderHeight(verts, batchSize);
	chestH    = ChestHeight(verts, batchSize);
	chestDepth= ChestDepth(verts, batchSize);
	hipH      = HipHeight(verts, batchSize);
	kneeH     = KneeHeight(verts, batchSize);
	hipDepth  = HipDepth(verts, batchSize);
	#upperBody = UpperBody(verts, batchSize)
	
	result = tf.concat([height, chest, lowerChest, waist, hip, thigh, calf, arm, leg, back,
		shouderH, chestH, hipH, kneeH, hipDepth], -1, name="IsThis") 
	#tf.print('shape = ',tf.shape(result))
	#tf.print('height = ',tf.shape(height))
	return result


def Height(verts, batchSize):
	#height = tf.math.subtract(tf.math.reduce_max(input_tensor=verts[:,:,1],axis=1), tf.math.reduce_min(input_tensor=verts[:,:,1],axis=1))
	#height = tf.reshape(height,(-1,1))
	height = verts[:,411,1] - verts[:,3438,1] # head.y - Soleplate.y
	#tf.print('Argmax = ',tf.math.argmax(verts, axis=1))
	#tf.print('Argmin = ',tf.math.argmin(verts, axis=1))
	return tf.reshape(height,(batchSize,-1))

def XYplane(verts, index, batchSize):
	length = 0
	for i in range(1, index.shape[0]):
		xDiff = tf.math.subtract(verts[:,index[i-1],X],  verts[:,index[i],X])
		yDiff = tf.math.subtract(verts[:,index[i-1],Y],  verts[:,index[i],Y])
		length += tf.math.sqrt( tf.math.pow(xDiff,2.0) + tf.math.pow(yDiff,2.0) )
	length = tf.reshape(length,(batchSize,-1))
	return length

def YZplane(verts, index, batchSize):
	length = 0
	for i in range(1, index.shape[0]):
		xDiff = tf.math.subtract(verts[:,index[i-1],Y],  verts[:,index[i],Y])
		yDiff = tf.math.subtract(verts[:,index[i-1],Z],  verts[:,index[i],Z])
		length += tf.math.sqrt( tf.math.pow(xDiff,2.0) + tf.math.pow(yDiff,2.0) )
	length = tf.reshape(length,(batchSize,-1))
	return length


def Arm(verts, batchSize):
	shoulder = verts[:,1881] #leftShoulder
	elbow = verts[:,1642]
	wrist = verts[:,2208] # shoulder.x - wrist.x
	s2e = tf.math.sqrt( tf.math.pow(shoulder[:,X]-elbow[:,X],2) + tf.math.pow(shoulder[:,Y]-elbow[:,Y],2) +
	 					tf.math.pow(shoulder[:,Z]-elbow[:,Z],2) )
	e2w = tf.math.sqrt( tf.math.pow(elbow[:,X]-wrist[:,X],2) + tf.math.pow(elbow[:,Y]-wrist[:,Y],2) +
	 					tf.math.pow(elbow[:,Z]-wrist[:,Z],2) )
	result = s2e + e2w
	return tf.reshape(result, (batchSize,-1))

def Leg(verts, batchSize):
	result = verts[:,4837,1] - verts[:,3438,1] # crotch.y - Soleplate.y
	return tf.reshape(result, (batchSize,-1))

def Back(verts, batchSize):
	result = verts[:,1881,0] - verts[:,6469,0] # leftShoulder.x - rightShoulder.x
	return tf.reshape(result, (batchSize,-1))

def ShoulderHeight(verts, batchSize):
	result = verts[:,1881,1] - verts[:,3438,1] # shoulder.y - Soleplate.y
	return tf.reshape(result, (batchSize,-1))

def ChestHeight(verts, batchSize):
	result = verts[:,4429,1] - verts[:,3438,1] # nipple.y - Soleplate.y
	return tf.reshape(result, (batchSize,-1))

def ChestDepth(verts, batchSize):
	result = verts[:,4429,2] - verts[:,4735,2] # nipple.y - back.y
	return tf.reshape(result, (batchSize,-1))

def HipHeight(verts, batchSize):
	result = verts[:,3119,1] - verts[:,3438,1] # hip.y - Soleplate.y
	return tf.reshape(result, (batchSize,-1))

def KneeHeight(verts, batchSize):
	result = verts[:,4533,1] - verts[:,3438,1] # hip.y - Soleplate.y
	return tf.reshape(result, (batchSize,-1))

def HipDepth(verts, batchSize):
	result = verts[:,3510,2] - verts[:,3119,2] # hip.y - belly.y
	return tf.reshape(result, (batchSize,-1))




''' Too unprecise in dataset.
def UpperBody(verts, batchSize):
	result = verts[:,3470,1] - verts[:,1783,1] # Neckbottom.y - pantUpperEdge.y
	return tf.reshape(result, (batchSize,-1))
'''