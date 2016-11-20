#!/usr/bin/python
import os
from subprocess import call

inputpath = '/data/amnh/darwin/images'
segment_exe = '/home/luis_ibanez/bin/darwin-notes-image-processing/Release/Segmentation/ImageToEdges'

for filename in os.listdir(inputpath):
    print filename
    call([segment_exe, filename])

