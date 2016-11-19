import os
from subprocess import call

inputpath = 'originals'
outputpath = 'segmentations'

for filename in os.listdir(inputpath):
    current = os.path.join(inputpath, filename)
    if os.path.isfile(current):
        call([segment_exe, current, result])

