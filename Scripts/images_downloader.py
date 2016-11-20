#!/usr/bin/python
__author__ = 'mtrebi'

import urllib
import csv
import sys

# images_downloader input.csv output_folder start

links_file  = open(str(sys.argv[1]), "rb")
log_file  = open('log.txt', 'w')
csvreader = csv.reader(links_file, delimiter='\t')

next(csvreader)
error_count = 0
total_count = 0;
base_url = "http://darwin.amnh.org//files/images/large/"
for row in csvreader:
	if (total_count >= int(sys.argv[3])):	# start at start position
		eid = row[0]
		image_name = row[1]

		space_pos = image_name.find(' ')
		if space_pos != -1: # Fix spaces
			image_name = image_name[:space_pos] + image_name[space_pos + 2:]
		url = base_url + eid + "_" + image_name 
		if urllib.urlopen(url).getcode() == 404: # Store errors
			log_file.write(row[0] + " " + row[1])
			error_count +=1
		else:
			urllib.urlretrieve(url, str(sys.argv[2]) + image_name)
	total_count += 1
links_file.close()

log_file.write("ERRORS: ", error_count)
log_file.write("TOTAL: ", total_count)

log_file.close()