#!/usr/bin/python
from joblib import Parallel, delayed
import multiprocessing

import os
from subprocess import call

inputpath = '/data/amnh/darwin/image_csvs_fft'
segment_exe = '/home/jin.chung17/darwin-notes-image-processing/CurveAnalysis/fft_similarity.py'

def handle_file(filename):
    call([segment_exe, filename])
    
inputs =  os.listdir(inputpath)
num_cores = multiprocessing.cpu_count()
results = Parallel(n_jobs=num_cores)(delayed(handle_file)(i) for i in inputs)

