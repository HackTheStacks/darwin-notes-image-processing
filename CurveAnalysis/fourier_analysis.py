#!/usr/bin/env python
import csv
import numpy as np
from scipy import signal
from scipy.fftpack import fft, ifft
from scipy import fftpack
import matplotlib.pyplot as plt

note_name = '77726_MS-DAR-00205-00001-000-00096'
base_dir = '/home/ibanez/data/amnh/darwin_notes/'
images_dir = base_dir + 'images/'
curves_dir = base_dir + 'image_csvs/'
image_name = images_dir + note_name + '.jpg'
north_curve_name = curves_dir + note_name + '_north.csv'
south_curve_name = curves_dir + note_name + '_south.csv'

input_north_curve = list(csv.reader(open(north_curve_name)))
input_south_curve = list(csv.reader(open(south_curve_name)))

N = 4096
north_curve = signal.resample(input_north_curve, N)
south_curve = signal.resample(input_south_curve, N)

x_signal = []
y_signal = []
for x, y in north_curve:
  y_signal.append(y)
  x_signal.append(x)

y_mean = sum(y_signal)/len(y_signal)
y_normal_signal = [(y - y_mean) for y in y_signal]

plt.plot(y_signal)
plt.plot(y_normal_signal)
plt.grid()
plt.show()

yfft = fft(y_normal_signal)

plt.semilogy(2.0/N * np.abs(fftpack.fftshift(yfft)))
plt.grid()
plt.show()

