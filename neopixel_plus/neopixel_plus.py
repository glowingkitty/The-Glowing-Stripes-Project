import math
import random
import time
from random import randint

from neopixel_plus.animations import *


class NeoPixel:
    def __init__(self,
                 pin_num=None,
                 n=30,
                 start_led=0,
                 test=False,
                 overwrite_line=True,
                 debug=False,
                 target='micropython'  # or 'adafruit'
                 ):
        self.debug = debug
        self.target = target
        self.strip_length = n
        self.addressable_strip_length = n
        self.start_led = start_led
        self.test = test
        self.pin_num = pin_num if pin_num else 10 if self.target == 'micropython' else 18 if self.target == 'adafruit' else None
        self.overwrite_line = overwrite_line
        self.sections = self.get_sections()

        if self.test:
            self.leds = [[0, 0, 0] for x in range(self.strip_length)]
        else:
            from neopixel import NeoPixel as NeoPixelOriginal
            if self.target == 'micropython':
                self.leds = NeoPixelOriginal(
                    self.get_pin(),
                    self.strip_length)
            elif self.target == 'adafruit':
                self.leds = NeoPixelOriginal(
                    self.get_pin(),
                    self.strip_length,
                    auto_write=False)

    def get_pin(self):
        if self.target == 'micropython':
            from machine import Pin
            return Pin(self.pin_num, Pin.OUT)

        elif self.target == 'adafruit':
            import board
            if self.pin_num == 18:
                return board.D18
            elif self.pin_num == 23:
                return board.D23
            elif self.pin_num == 24:
                return board.D24
            elif self.pin_num == 24:
                return board.D24
            elif self.pin_num == 25:
                return board.D25
            elif self.pin_num == 12:
                return board.D12
            elif self.pin_num == 16:
                return board.D16
            elif self.pin_num == 4:
                return board.D4
            elif self.pin_num == 17:
                return board.D17
            elif self.pin_num == 27:
                return board.D27
            elif self.pin_num == 22:
                return board.D22
            elif self.pin_num == 5:
                return board.D5
            elif self.pin_num == 6:
                return board.D6
            elif self.pin_num == 13:
                return board.D13
            elif self.pin_num == 26:
                return board.D26

    def get_sections(self):
        if self.debug:
            print('NeoPixel().get_sections()')

        sections_length = 15
        sections = []
        counter = 0
        while counter < self.addressable_strip_length:
            sections.append(
                [x for x in range(counter, counter+sections_length)])
            counter += sections_length
        return sections

    def get_led_selectors(self, sections='all'):
        if self.debug:
            print('NeoPixel().get_led_selectors(sections={})'.format(sections))

        if type(sections) == str:
            if sections == 'all':
                return range(self.addressable_strip_length)
            elif sections == 'random':
                return self.sections[randint(0, len(self.sections)-1)]
        else:
            selected_leds = []
            for entry in sections:
                selected_leds += self.sections[entry]

            return selected_leds

    def write(self, s_after_wait=1.0/36.0):
        if self.debug:
            print('NeoPixel().write(s_after_wait={})'.format(s_after_wait))

        if self.test:
            from colr import color
            print(
                ''.join(color('  ', back=(x[0], x[1], x[2])) for x in self.leds), end='\r' if self.overwrite_line and not self.debug else '\n')
        else:
            if self.target == 'micropython':
                self.leds.write()
            elif self.target == 'adafruit':
                self.leds.show()
        time.sleep(s_after_wait)

    def insert_led(self, position=0, rgb=[0, 0, 0]):
        if self.debug:
            print('NeoPixel().insert_led(position={},rgb={})'.format(position, rgb))
        # save state of all leds as list, insert LED at position, then write LEDs
        leds = [[x[0], x[1], x[2]] for x in self.leds]
        leds.insert(position, rgb)
        leds = leds[:-1]
        for i in range(len(leds)):
            self.leds[i] = leds[i]

    def append_led(self, rgb=[0, 0, 0]):
        if self.debug:
            print('NeoPixel().append_led(rgb={})'.format(rgb))
        # save state of all leds as list, append LED at the end, then write LEDs
        leds = [[x[0], x[1], x[2]] for x in self.leds]
        leds.append(rgb)
        leds = leds[1:]
        for i in range(len(leds)):
            self.leds[i] = leds[i]

    def get_led(self, i, start=None):
        if self.debug:
            print('NeoPixel().get_led(i={},start={}'.format(i, start))
        i = i+self.start_led
        if i < 0:
            i += self.addressable_strip_length
        if start and start == 'end':
            i += (self.strip_length-self.addressable_strip_length)

        return i

    def off(self):
        if self.debug:
            print('NeoPixel().off()')
        for i in range(self.strip_length):
            self.leds[i] = (0, 0, 0)
        self.write()

    def on(self, num=None):
        if self.debug:
            print('NeoPixel().on(num={})'.format(num))
        if type(num) == int:
            num = self.get_led(num)
            self.leds[num] = (255, 255, 255)
        else:
            for i in range(self.strip_length):
                self.leds[i] = (255, 255, 255)
        self.write()

    def color(self, r, g, b):
        if self.debug:
            print('NeoPixel().color(r={},g={},b={})'.format(r, g, b))
        for i in range(self.strip_length):
            i = self.get_led(i)
            self.leds[i] = (r, g, b)
        self.write()

    def test_animations(self):
        # run all the animations for testing
        print('Start testing animations...')
        while True:
            self.rainbow_animation(loop_limit=2)

            self.beats(loop_limit=3)
            self.beats(loop_limit=3, start='end')
            self.beats(loop_limit=3, start='start + end')
            self.beats(loop_limit=3, start='center', brightness=0.5)

            self.moving_dot(loop_limit=3)
            self.moving_dot(loop_limit=3, start='end', brightness=0.5)

            self.light_up(loop_limit=3)
            self.light_up(loop_limit=3, sections='random')
            self.light_up(loop_limit=3, sections=[0])

            self.transition(loop_limit=3)
            self.transition(loop_limit=3, sections=[0])

    def rainbow_animation(self,
                          loop_limit=None,
                          brightness=1,
                          duration_ms=1000,
                          pause_ms=None):
        RainbowAnimation(
            led_strip=self,
            loop_limit=loop_limit,
            brightness=brightness,
            duration_ms=duration_ms,
            pause_ms=pause_ms
        ).glow()

    def beats(self,
              rgb_colors=None,
              brightness=1,
              brightness_fixed=False,
              max_height=1,
              loop_limit=None,
              duration_ms=200,
              pause_ms=300,
              start='start',
              num_random_colors=5):
        BeatsUpAndDown(
            led_strip=self,
            rgb_colors=rgb_colors,
            brightness=brightness,
            brightness_fixed=brightness_fixed,
            max_height=max_height,
            loop_limit=loop_limit,
            duration_ms=duration_ms,
            pause_ms=pause_ms,
            start=start,
            num_random_colors=num_random_colors
        ).glow()

    def moving_dot(self,
                   rgb_colors=None,
                   brightness=1,
                   loop_limit=None,
                   duration_ms=200,
                   pause_a_ms=0,
                   pause_b_ms=300,
                   start='start',
                   num_random_colors=5):
        MovingDot(
            led_strip=self,
            rgb_colors=rgb_colors,
            brightness=brightness,
            loop_limit=loop_limit,
            duration_ms=duration_ms,
            pause_a_ms=pause_a_ms,
            pause_b_ms=pause_b_ms,
            start=start,
            num_random_colors=num_random_colors
        ).glow()

    def light_up(self,
                 rgb_colors=None,
                 brightness=1,
                 loop_limit=None,
                 duration_ms=200,
                 pause_ms=200,
                 num_random_colors=5,
                 sections='all'):
        LightUp(
            led_strip=self,
            rgb_colors=rgb_colors,
            brightness=brightness,
            loop_limit=loop_limit,
            duration_ms=duration_ms,
            pause_ms=pause_ms,
            num_random_colors=num_random_colors,
            sections=sections
        ).glow()

    def transition(self,
                   rgb_colors=None,
                   brightness=1,
                   loop_limit=None,
                   duration_ms=200,
                   pause_ms=200,
                   num_random_colors=5,
                   sections='all'):
        Transition(
            led_strip=self,
            rgb_colors=rgb_colors,
            brightness=brightness,
            loop_limit=loop_limit,
            duration_ms=duration_ms,
            pause_ms=pause_ms,
            num_random_colors=num_random_colors,
            sections=sections
        ).glow()
