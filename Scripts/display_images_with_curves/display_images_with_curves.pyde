#
# Visualize the images from the Darwin Notes project
#
import csv

note_name = '77726_MS-DAR-00205-00001-000-00096'
base_dir = '/home/ibanez/data/amnh/darwin_notes/'
images_dir = base_dir + 'images/'
curves_dir = base_dir + 'image_csvs/'
image_name = images_dir + note_name + '.jpg'
north_curve_name = curves_dir + note_name + '_north.csv'
south_curve_name = curves_dir + note_name + '_south.csv'

img = None
north_curve = None
south_curve = None

rescaledWidth = 100
rescaledHeight = 100
newCenterX = 0

def setup():
    global img
    global north_curve
    global south_curve
    global rescaledWidth
    global rescaledHeight
    global newCenterX
    size(1000, 1000)
    background(128)
    img = loadImage(image_name)
    print image_name, img.width, img.height
    north_curve = csv.reader(open(north_curve_name))
    south_curve = csv.reader(open(south_curve_name))
    rescaledWidth = 1000 * img.width / img.height
    rescaledHeight = 1000
    newCenterX = (1000 - rescaledWidth) / 2

def map_coordinates(ox, oy):
    ix = int(float(ox) * rescaledWidth / img.width + newCenterX)
    iy = int(float(oy) * rescaledHeight / img.height)
    return ix, iy
    
def draw():
    image(img, newCenterX, 0, rescaledWidth, rescaledHeight)
    stroke(255, 0, 0)
    strokeWeight(5)
    for sx, sy in north_curve:
        x, y = map_coordinates(sx, sy) 
        point(x,y)
        stroke(255, 0, 0)
    stroke(0, 255, 0)
    strokeWeight(5)
    for sx, sy in south_curve:
        x, y = map_coordinates(sx, sy) 
        point(x,y)