#
# Visualize the images from the Darwin Notes project
#
base_dir = '/home/ibanez/data/amnh/darwin_notes/'
input_dir = base_dir + 'segmentations/'
filenames = os.listdir(input_dir)
filename = iter(filenames)

def setup():
    size(1000, 1000)

def draw():
    background(128)
    name = input_dir + filename.next()
    img = loadImage(name)
    print name, img.width, img.height
    rescaledWidth = 1000 * img.width/img.height
    rescaledHeight = 1000
    newCenterX = (1000 - rescaledWidth) / 2
    image(img, newCenterX, 0, rescaledWidth, rescaledHeight)