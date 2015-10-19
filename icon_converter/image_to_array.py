from PIL import Image
import os
from os.path import basename

#image_name = "../icons/espresso_mug.jpg"
#image_name = "../icons/warmup.jpg"
#image_name = "../icons/heatup.jpg"
#image_name = "../icons/pump.jpg"
#image_name = "../icons/welcome.jpg"

image_name = "../icons/steam.jpg"

#image_name = "../icons/toocold.jpg"
#image_name = "../icons/toohot.jpg"

var_name = os.path.splitext(basename(image_name))[0]
im = Image.open(image_name)

pix = im.load()


def is_white(pixel, threshold=200):
    if pixel[0] > threshold and pixel[1] > threshold and pixel[2] > threshold:
        return True
    else:
        return False

output = "const unsigned int icon_data_{0}[] PROGMEM = {{".format(var_name)

hex_data = []

for x in range(0, im.size[0]):
    for y in range(0, im.size[1]):
        if not is_white(pix[x, y]):
            coordinate = ((x & 0xff) << 8) | (y & 0xff)
            hex_data.append(format(coordinate, '#06x'))

output += ", ".join(hex_data) + "};"

print output