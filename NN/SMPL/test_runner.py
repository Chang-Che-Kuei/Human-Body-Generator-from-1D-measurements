#!/usr/bin/python3
# -*- encoding: utf-8 -*-

import sys
import os
import tensorflow as tf
import numpy as np

from SMPL.batch_smpl import SMPL


def GetSmplModel(modelPath, betas, thetas):
    
    dirpath = os.path.dirname(os.path.realpath(__file__))
    smpl_path = os.path.realpath(modelPath)
    smpl_model = SMPL(smpl_path)
  
    #betas = np.random.randn(1, 10).astype(np.float32) * 0
    #thetas = np.random.randn(1, 72).astype(np.float32) * 0
    a = tf.compat.v1.placeholder(tf.float32, shape=betas.shape)
    b = tf.compat.v1.placeholder(tf.float32, shape=thetas.shape)
    
    # import scipy.io
    # info_dict = scipy.io.loadmat(os.path.join(dirpath, '32_07_c0001_info.mat'))
    # thetas = np.transpose(info_dict['pose'], (1, 0))[0, np.newaxis]
    # betas = np.transpose(info_dict['shape'], (1, 0))[0, np.newaxis]

    verts, _, _ = smpl_model(betas, thetas, get_skin=True)
    verts = verts[0] # There are batches of model. It takes the first model of vertices

    config = tf.compat.v1.ConfigProto()
    config.gpu_options.allow_growth = True  # pylint: disable=no-member
    sess = tf.compat.v1.Session(config=config)
    result = sess.run(verts, feed_dict={a:betas, b:thetas})

    faces = np.load(os.path.join(dirpath, 'smpl_faces.npy'))
    
    outmesh_path = os.path.join(dirpath, '2.obj')
    with open(outmesh_path, 'w') as fp:
        for v in result:
            fp.write('v %f %f %f\n' % (v[0], v[1], v[2]))

        for f in faces + 1:
            fp.write('f %d %d %d\n' % (f[0], f[1], f[2]))

    return result

