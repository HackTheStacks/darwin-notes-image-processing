#!/usr/bin/env python
import csv
from scipy import signal
from scipy.fftpack import fft
import scipy.io as io
import sys

base_dir = '/data/amnh/darwin/'
#base_dir = '/home/queenjin/sandbox/amnh/matchural/'
input_curves_dir = base_dir + 'image_csvs/'
curves_fft_dir = base_dir + 'image_csvs_fft/'
curve_filename = sys.argv[1]
curve_name = input_curves_dir + curve_filename 
input_curve = list(csv.reader(open(curve_name)))

N = 4096
resample_curve = signal.resample(input_curve, N)

y_signal = []
for x, y in resample_curve:
  y_signal.append(y)

yfft = fft(y_signal)
fft_filename = curves_fft_dir + curve_filename[:-4] + '_fft.mat'

# Write FFT to file
io.savemat(fft_filename, {"fft": yfft})

