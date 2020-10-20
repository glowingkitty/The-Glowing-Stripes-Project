# upload as boot.py

import gc

import esp
from neopixel_plus import NeoPixel

esp.osdebug(None)

gc.collect()

NeoPixel(pin_num=4, n=80).rainbow_animation()
