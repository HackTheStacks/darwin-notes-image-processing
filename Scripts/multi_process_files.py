#!/usr/bin/python
from joblib import Parallel, delayed
import multiprocessing

import os
from subprocess import call

inputpath = '/data/amnh/darwin/images'
segment_exe = '/home/luis_ibanez/bin/darwin-notes-image-processing/Release/Segmentation/ImageAnalyzer'

def handle_file(filename):
    call([segment_exe, filename])
    
inputs =  os.listdir(inputpath)
num_cores = multiprocessing.cpu_count()
results = Parallel(n_jobs=num_cores)(delayed(handle_file)(i) for i in inputs)

