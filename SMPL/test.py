import numpy as np
# index of body
chestI = np.array([4160,4087,4090,4175,4173,4180,6332,3078,2872,692,685,687,600,599,670,669,1422,1425,
614,1198,1764,1423,645,644,738,764,2907,1254,1237,751,752,3015,4240,4241,4720,4737,6367,4253,4252,4225,
4134,4133,4897,5231,4683,4100,4899,4895,4157,4160],dtype=np.int32)
lowerChestI = np.array([1330,1201,928,929,659,660,1758,1759,942,620,621,1270,795,796,2845,2844,1213,768,
	767,3018,4257,4256,4696,6306,6305,6304,4284,4753,4109,4110,4428,5226,5225,4150,4147,4416,4414,4689],dtype=np.int32)
waistI =  np.array([3501,1337,918,917,920,680,679,706,939,847,831,832,845,846,887,2929,2930,1780,
1783,3022,5246,5245,6389,6390,4374,4331,4332,4317,4318,4333,4194,4167,4168,4407,4406,4404,4403,4813.3501],dtype=np.int32)
hipI =  np.array([3120,6542,6540,6541,6560,6559,6558,6510,4920,4921,4928,6550,4986,4401,4399,4692,4349,4350,3511,866,
863,1207,913,915,1513,3129,1455,1447,1448,3085,3137,3138,3139,3117,3118,3119,3120],dtype=np.int32)
thighI =  np.array([4397,4437,4436,4929,4455,4454,4934,4933,4936,4459,4458,4391,4392,4444,4393,4394,4839,4397],dtype=np.int32)
calfI =  np.array([4590,4588,4587,4582,4583,4942,4940,4938,4580,4579,5000,4575,4576,4642,4845,4590],dtype=np.int32)
upperArmI =  np.array([790,628,629,1397,1398,1383,1382,1386,1233,1234,1389,1390,1394,1395,1379,1380,1316,1312,790],dtype=np.int32)
chestI -= 1
lowerChestI -= 1
waistI -= 1
hipI -= 1
thighI -= 1
calfI -= 1
upperArmI -= 1
N_VERTEX = 6890

f = open("2.obj","r")
vert = np.zeros((N_VERTEX,3))

for i in range(N_VERTEX):
	line = f.readline()
	line = line.split(" ")
	vert[i] = [float(line[1]), float(line[2]), float(line[3])]
f.close()


f = open("checkPart.obj","w")
All = [chestI, waistI, lowerChestI, hipI, thighI, calfI]
n = 1
for part in All:
	for i in part:
		f.write("v %f %f %f\n" % (vert[i][0], vert[i][1], vert[i][2]))
	for i in range(part.shape[0]-1):
		f.write("l %d %d\n" % (n,n+1))
		n = n + 1
	n+=1


f.write("v %f %f %f\n" % (vert[1821][0], vert[1821][1], vert[1821][2]))
f.write("v %f %f %f\n" % (vert[1642][0], vert[1642][1], vert[1642][2]))
f.write("v %f %f %f\n" % (vert[2208][0], vert[2208][1], vert[2208][2]))
f.write("l %d %d %d\n" % (n,n+1,n+2))
n = n +3


f.write("v %f %f %f\n" % (vert[4837][0], vert[4837][1], vert[4837][2]))
f.write("v %f %f %f\n" % (vert[4837][0], vert[3438][1], vert[4837][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2

f.write("v %f %f %f\n" % (vert[1821][0], vert[1821][1], vert[1821][2]))
f.write("v %f %f %f\n" % (vert[5325][0], vert[5325][1], vert[5325][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2


f.write("v %f %f %f\n" % (vert[1821][0], vert[1821][1], vert[1821][2]))
f.write("v %f %f %f\n" % (vert[1821][0], vert[3438][1], vert[1821][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2

f.write("v %f %f %f\n" % (vert[4429][0], vert[4429][1], vert[4429][2]))
f.write("v %f %f %f\n" % (vert[4429][0], vert[3438][1], vert[4429][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2

f.write("v %f %f %f\n" % (vert[4429][0], vert[4429][1], vert[4429][2]))
f.write("v %f %f %f\n" % (vert[4429][0], vert[4429][1], vert[4735][2]))#chest depth
f.write("l %d %d\n" % (n,n+1))
n = n +2


f.write("v %f %f %f\n" % (vert[3119][0], vert[3119][1], vert[3119][2]))
f.write("v %f %f %f\n" % (vert[3119][0], vert[3438][1], vert[3119][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2


f.write("v %f %f %f\n" % (vert[4533][0], vert[4533][1], vert[4533][2]))
f.write("v %f %f %f\n" % (vert[4533][0], vert[3438][1], vert[4533][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2


f.write("v %f %f %f\n" % (vert[3119][0], vert[3119][1], vert[3119][2]))
f.write("v %f %f %f\n" % (vert[3510][0], vert[3510][1], vert[3510][2]))
f.write("l %d %d\n" % (n,n+1))
n = n +2


f.close()