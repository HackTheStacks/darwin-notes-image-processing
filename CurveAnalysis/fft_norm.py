#!/usr/bin/env python
import numpy as np
import sys
import scipy.io as sio
import os
import operator

base_dir = '/data/amnh/darwin/'
curves_fft_dir = base_dir + 'image_csvs_fft/'

fft_norm_map = {}

def compute_fft_norm(curve_filename):
    curve_name = curves_fft_dir + curve_filename 
    fft_array = sio.loadmat(curve_name)['fft']
    # zero out scaling and rotation components
    fft_array[0][0] = 0
    fft_norm = np.linalg.norm(fft_array)
    return fft_norm

def stringify(x):
    float_score = '%.20f' % x[1]
    return x[0] + '\t' + float_score
    
input_files = os.listdir(curves_fft_dir)
fft_norm_filename = base_dir + 'fft_norms.csv'

for curve_filename in input_files:
    norm_value = compute_fft_norm(curve_filename)
    fft_norm_map[curve_filename] = norm_value 

# sort by magnitude of the fft norm
final_sorted_results = sorted(fft_norm_map.items(), key=operator.itemgetter(1), reverse=True)[:100]
fft_sim_file = open(fft_norm_filename, 'w')
stringified_results = [stringify(x) for x in final_sorted_results]
fft_sim_file.write('\n'.join(stringified_results))
fft_sim_file.close()

