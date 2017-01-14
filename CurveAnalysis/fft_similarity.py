#!/usr/bin/env python
import numpy as np
import sys
import scipy.io as sio
import os
import operator

#base_dir = '/home/queenjin/sandbox/amnh/matchural/'
base_dir = '/data/amnh/darwin/'
curves_fft_dir = base_dir + 'image_csvs_fft/'
fft_similarity_dir = base_dir + 'fft_similarity/'

sim_score_map = {}

def similarity_score(curve1_filename, curve2_filename):
    curve1_name = curves_fft_dir + curve1_filename 
    curve2_name = curves_fft_dir + curve2_filename 

    fft1 = sio.loadmat(curve1_name)['fft']
    fft2 = sio.loadmat(curve2_name)['fft']

    # zero out scaling and rotation components
    fft1[0][0] = 0
    fft2[0][0] = 0
    norm1 = np.linalg.norm(fft1)
    norm2 = np.linalg.norm(fft2)
    dot_product = np.absolute(np.vdot(fft1, fft2))
    mul_norm = norm1 * norm2
    if mul_norm != 0: 
        return dot_product / mul_norm 
    return 0

def stringify(x):
    float_score = '%.20f' % x[1]
    return x[0] + '\t' + float_score
    
curve1_filename = sys.argv[1]
input_files = os.listdir(curves_fft_dir)
fft_similarity_filename = fft_similarity_dir + curve1_filename[:-4] + '_similarity.txt'

for curve2_filename in input_files:
    sim_score = similarity_score(curve1_filename, curve2_filename)
    sim_score_map[curve2_filename] = sim_score 

# sort
final_sorted_results = sorted(sim_score_map.items(), key=operator.itemgetter(1), reverse=True)[:100]
fft_sim_file = open(fft_similarity_filename, 'w')
stringified_results = [stringify(x) for x in final_sorted_results]
fft_sim_file.write('\n'.join(stringified_results))
fft_sim_file.close()

