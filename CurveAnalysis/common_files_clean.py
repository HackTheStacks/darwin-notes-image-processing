import os

cleaned_dir = '/data/amnh/darwin/image_csvs_fft_clean/'
fft_sim_dir = '/data/amnh/darwin/fft_similarity/'

cleaned_dir_files = os.listdir(cleaned_dir)
fft_sim_dir_files = os.listdir(fft_sim_dir)

cleaned_dir_files_concat = {x[:-8] for x in cleaned_dir_files}
fft_sim_dir_files_concat = {x[:-19] for x in fft_sim_dir_files}

file_intersection = cleaned_dir_files_concat.intersection(fft_sim_dir_files_concat)

top_match = [(x, open(fft_sim_dir + x + '_fft_similarity.txt').readlines()[1].split('\t')) for x in file_intersection]

output = open('top_items.txt', 'w')
for x in top_match:
  output.write("%s %s %s" % (x[0], x[1][0], x[1][1]))

output.close()
 
#top_20_sorted = sorted(top_match, reverse=True, key=lambda x: float(x[1][1].strip()))
#print(top_20_sorted)
