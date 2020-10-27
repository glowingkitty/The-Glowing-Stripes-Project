import json
import os
import random
import time

import requests
from neopixel_plus import NeoPixel

from led_animations import LEDanimations
from pi_hardware import PiZeroWH

dirname = os.path.dirname(__file__)


class StripeConfig:
    def config():
        with open(os.path.join(dirname, 'stripe_config.json')) as json_file:
            json_data = json.load(json_file)
            return json_data


class Stripe():
    def __init__(self,
                 led_strip_data_pin_num=StripeConfig.config()[
                     'led_strip_data_pin_num'],
                 num_of_leds=StripeConfig.config()['num_of_leds'],
                 host_address=StripeConfig.config()['host_address']
                 ):
        self.host_address = host_address

        # setting up the LEDs
        self.led_strip_data_pin_num = led_strip_data_pin_num
        self.num_of_leds = num_of_leds
        self.leds = NeoPixel(n=self.num_of_leds,
                             target='adafruit' if self.host_address == 'raspberrypi.local' else 'micropython',
                             test=False if self.host_address == 'raspberrypi.local' else True)

        self.machine = PiZeroWH()

    @property
    def ip_address(self):
        from get_ip import get_ip
        return get_ip(self.host_address)

    @property
    def name(self):
        # read name from config.json
        return StripeConfig.config()['name']

    @property
    def last_animation(self):
        last_animation = LEDanimations().last_used
        if not last_animation:
            last_animation = {
                "id": "b943uee3y7",
                "name": "Rainbow"
            }
        return last_animation

    @property
    def id(self):
        # read id from config.json, if it doesn't exist yet, generate one and write it to config
        json_data = StripeConfig.config()
        if not json_data['id']:
            json_data['id'] = self.get_random_id()
            self.update_id(json_data['id'])
        return json_data['id']

    def get_random_id(self):
        return ''.join([random.choice(['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']) for n in range(10)])

    def update_last_animation(self, animation):
        if animation['id'] == '0' or animation['id'] == '0000000000':
            print('Skipped saving "Setup mode"')
        else:
            # save name to config.json
            json_data = StripeConfig.config()
            json_data['last_animation'] = animation

            with open(os.path.join(dirname, 'stripe_config.json'), 'w') as outfile:
                json.dump(json_data, outfile)

                print('New last_animation: {}'.format(
                    json_data['last_animation']))

    def update_name(self, new_name):
        # save name to config.json
        json_data = StripeConfig.config()
        json_data['name'] = new_name

        with open(os.path.join(dirname, 'stripe_config.json'), 'w') as outfile:
            json.dump(json_data, outfile)

            print('New name: ', json_data['name'])

    def update_id(self, new_id):
        # save id to config.json
        json_data = StripeConfig.config()
        json_data['id'] = new_id

        with open(os.path.join(dirname, 'stripe_config.json'), 'w') as outfile:
            json.dump(json_data, outfile)

            print('New id: ', json_data['id'])

    def signup(self):
        print('Sign up to host...')
        # try to sign up and only stop when successfull
        success = False
        while not success:
            # try:
            led_strip_data = {
                "id": self.id,
                "name": self.name,
                "ip_address": self.ip_address,
                "last_animation": self.last_animation,
                "num_of_leds": self.num_of_leds
            }
            request = requests.post(
                'http://'+self.host_address+'/signup', json=led_strip_data)
            print(request.status_code)
            if request.status_code == 200:
                success = True
            else:
                time.sleep(2)
            # except:
            #     print('signup failed...')
            #     time.sleep(2)
            #     pass

    def glow(self,
             id=None,
             based_on=None,
             customization=None):
        # take current mode from json and glow in infinite loop
        if not id:
            last_animation = self.last_animation
            id = last_animation['id']
            if 'based_on' in last_animation and last_animation['based_on']:
                based_on = last_animation['based_on']
            if 'customization' in last_animation and last_animation['customization']:
                customization = last_animation['customization']

        default_animations = {
            '0000000000': self.leds.color,
            'b943uee3y7': self.leds.rainbow_animation,
            '8hsylal9v7': self.leds.beats,
            'leta9ed5fc': self.leds.moving_dot,
            'kack2555kd': self.leds.light_up,
            '7u9tjpd0gi': self.leds.transition,
        }

        if id in default_animations:
            # play default animation
            print('Play default animation...')
            default_animations[id](
                stop_ongoing_animation=True
            )
        else:
            # get custom animation from led_animations
            print('Play custom animation...')
            default_animations[based_on['id']](
                stop_ongoing_animation=True,
                customization_json=customization)

    def on(self):
        self.signup()
        self.glow()

    def off(self):
        # shutdown via linux command
        self.machine.off()
