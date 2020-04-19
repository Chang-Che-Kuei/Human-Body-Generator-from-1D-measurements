#!/usr/bin/python3
# -*- encoding: utf-8 -*-

import sys
import os
import tensorflow as tf
import numpy as np
os.environ['CUDA_VISIBLE_DEVICES'] = '-1'
from batch_smpl import SMPL


if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Usage: python3 test_runner.py <path-to-SMPL-model>")
        sys.exit()
    
    dirpath = os.path.dirname(os.path.realpath(__file__))
    smpl_path = os.path.realpath(sys.argv[1])
    smpl_model = SMPL(smpl_path)
  
    betas = np.random.randn(1, 10).astype(np.float32) * 0
    thetas = np.random.randn(1, 72).astype(np.float32) * 0
    #a = tf.compat.v1.placeholder(tf.float32, shape=betas.shape)
    #b = tf.compat.v1.placeholder(tf.float32, shape=thetas.shape)
    
    betas[0] = [ 0.512395859 , -0.547654688 , -0.418111682 , 3.35437632 , 3.27270198 , -3.25339627 , 2.39611864 , 1.31069338 , -0.190984294 , 1.39778149 ]

    thetas[0][41]  = -0.2 # Left shoulder 39
    thetas[0][44]  = 0.2 # Left shoulder 39
    thetas[0][50]  = -0.8 # Left shoulder 39
    thetas[0][53]  = 0.8 # right shoulder 42

    thetas[0][5] = 0.1 # Left leg
    thetas[0][8] = -0.1# Right leg

    thetas[0][55] = -0.1 # left elbow
    thetas[0][58] = 0.1# right elbow

    # import scipy.io
    # info_dict = scipy.io.loadmat(os.path.join(dirpath, '32_07_c0001_info.mat'))
    # thetas = np.transpose(info_dict['pose'], (1, 0))[0, np.newaxis]
    # betas = np.transpose(info_dict['shape'], (1, 0))[0, np.newaxis]

    verts, _, _ = smpl_model(betas, thetas, get_skin=True)
    #print(type(verts),verts.shape,'\n\n\n')
    verts = verts[0]

    #config = tf.compat.v1.ConfigProto()
    #config.gpu_options.allow_growth = True  # pylint: disable=no-member
    #sess = tf.compat.v1.Session(config=config)
    #result = sess.run(verts, feed_dict={a:betas, b:thetas})


    faces = np.load(os.path.join(dirpath, 'smpl_faces.npy'))
    #print(dirpath)
    
    outmesh_path = os.path.join(dirpath, '2.obj')
    with open(outmesh_path, 'w') as fp:
        for v in verts:
            fp.write('v %f %f %f\n' % (v[0], v[1], v[2]))

        for f in faces + 1:
            fp.write('f %d %d %d\n' % (f[0], f[1], f[2]))

