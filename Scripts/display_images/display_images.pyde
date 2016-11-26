img = None
base = '/home/ibanez/data/amnh/darwin_notes/'
inputpath = base + 'segmentations/'
filenames = os.listdir(inputpath)
filename = iter(filenames)

def setup():
    global img
    size(1000, 1000)
    # surface.setResizable(True)
    
def loadNextImageAndResize():
    global inputpath
    global filename
    global img
    name = inputpath + filename.next()
    img = loadImage(name)
    print name, img.width, img.height
    # surface.setSize(1000 * img.width/img.height, 1000)
            
def mousePressed():
    loadNextImageAndResize()

def draw():
    background(128)
    name = inputpath + filename.next()
    img = loadImage(name)
    print name, img.width, img.height
    rescaledWidth = 1000 * img.width/img.height
    rescaledHeight = 1000
    newCenterX = (1000 - rescaledWidth) / 2
    image(img, newCenterX, 0, rescaledWidth, rescaledHeight)