import random
import time

from neopixel_plus import NeoPixel


class Stripes():
    def __init__(self,
                 host_wifi_name='GlowingStripes',
                 host_wifi_password='letsglow',
                 dreamcolor_input_pin_num=5,
                 led_strip_data_pin_num=4,
                 test=False
                 ):
        # setting up the LEDs
        self.leds = NeoPixel(pin_num=led_strip_data_pin_num,
                             n=60, start_point=0, brightness=1, bpp=3, test=test)

        self.test = test

    def on(self):
        # TODO: show turn-on animation, glow all leds
        self.leds.rainbow_animation()

    def restart(self):
        if self.test:
            print('Restart can only be performed on real hardware, not in test mode.')
        else:
            import machine
            machine.reset()
