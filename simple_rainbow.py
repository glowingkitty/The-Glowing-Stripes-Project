# upload as boot.py

import gc

import esp
from neopixel_plus import NeoPixel

esp.osdebug(None)

gc.collect()

leds = NeoPixel(pin_num=4, n=150, start_point=0, brightness=0.5,
                bpp=3, animation_up_and_down=False)
leds.rainbow_animation()
