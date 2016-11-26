#!/usr/bin/env python
import csv
import math
import numpy as np
from scipy import signal
from scipy.fftpack import fft, ifft
from scipy import fftpack
import matplotlib.pyplot as plt
import datetime

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

t1 = datetime.datetime.now();

x_signal = []
y_signal = []
for x, y in north_curve:
  y_signal.append(y)
  x_signal.append(x)

t2 = datetime.datetime.now();

y_mean = sum(y_signal)/len(y_signal)
y_centered_signal = [(y - y_mean) for y in y_signal]

y_2_sum = 0.0
for yn in y_centered_signal:
  y_2_sum += yn * yn

y_norm = math.sqrt(y_2_sum)
y_normal_signal = [(y / y_norm) for y in y_centered_signal]

t3 = datetime.datetime.now();
yfft = fft(y_normal_signal)

t4 = datetime.datetime.now();

print 'read:   ', t2-t1
print 'normal: ', t3-t2
print 'fft:    ', t4-t3
print 'total:  ', t4-t1

plt.plot(y_signal)
plt.grid()
plt.show()

plt.plot(y_normal_signal)
plt.grid()
plt.show()

plt.semilogy(2.0/N * np.abs(fftpack.fftshift(yfft)))
plt.grid()
plt.show()

