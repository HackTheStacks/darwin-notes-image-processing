#!/usr/bin/env python
import numpy as np
import sys
import scipy.io as sio

base_dir = '/home/queenjin/sandbox/amnh/matchural/'
#base_dir = '/data/amnh/darwin/'
curves_fft_dir = base_dir + 'image_csvs_fft/'

def similarity_score(curve1_filename, curve2_filename):
    curve1_name = curves_fft_dir + curve1_filename 
    curve2_name = curves_fft_dir + curve2_filename 

    fft1 = sio.loadmat(curve1_name)['fft']
    fft2 = sio.loadmat(curve2_name)['fft']

    norm1 = np.linalg.norm(fft1)
    norm2 = np.linalg.norm(fft2)
    print(norm1)
    print(norm2)
    print('\ndot product')
    dot_product = np.absolute(np.vdot(fft1, fft2))
    print(dot_product)
    mul_norm = norm1 * norm2
    if mul_norm != 0: 
        return dot_product / mul_norm 
    return 0

curve1_filename = sys.argv[1]
curve2_filename = sys.argv[2]

sim_score = similarity_score(curve1_filename, curve2_filename)
print('\nsim score:')
print(sim_score)
