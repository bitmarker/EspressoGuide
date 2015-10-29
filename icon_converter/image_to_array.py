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

def convert_icon(image_name, no_meta=False):
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

    output += ", ".join(hex_data) + "};\n"

    if not no_meta:
        output += "const int icon_count_{0} = {1};\n".format(var_name, len(hex_data))
        output += "const int icon_width_{0} = {1};".format(var_name, im.size[0])

    return output

print convert_icon("../icons/welcome.jpg")
print convert_icon("../icons/trend_up_big.jpg")
print convert_icon("../icons/trend_down_big.jpg")
print convert_icon("../icons/trend_up_small.jpg")
print convert_icon("../icons/trend_down_small.jpg")

font_icons = [
    'zero', 'one', 'two', 'three', 'four', 'five', 'six', 'seven', 'eight', 'nine'
]

for icon in font_icons:
    image_name = "../icons/fonts/{0}.jpg".format(icon)
    print convert_icon(image_name)


# print("\n")
# i = 0
# for icon in font_icons:
#     print("case {1}: DRAW_DIGIT({0}, x, y); break;".format(icon, i))
#     i += 1
#
# i = 0
# for icon in font_icons:
#     print("case {1}: return DIGIT_WIDTH({0});".format(icon, i))
#     i += 1