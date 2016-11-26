img = None
base = '/home/ibanez/data/amnh/darwin_notes/'
inputpath = base + 'segmentations/'
filenames = os.listdir(inputpath)
filename = iter(filenames)

def setup():
    global img
    size(1000, 1000)
    frame.setResizable(True)
    
def loadNextImageAndResize():
    global inputpath
    global filename
    global img
    name = inputpath + filename.next()
    img = loadImage(name)
    print name, img.width, img.height
    frame.setSize(1000 * img.width/img.height, 1000)
            
def mousePressed():
    loadNextImageAndResize()

def draw():
    global img
    loadNextImageAndResize()
    image(img, 0, 0, 1000 * img.width/img.height, 1000)