import json
import os
import random
import time

import requests
from neopixel_plus import NeoPixel

from pi_hardware import PiZeroWH

dirname = os.path.dirname(__file__)


class Stripe():
    def __init__(self,
                 led_strip_data_pin_num=4,
                 num_of_leds=60,
                 host_address='raspberrypi.local'
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
        with open(os.path.join(dirname, 'config.json')) as json_file:
            json_data = json.load(json_file)
            return json_data['name']

    def update_name(self, new_name):
        # save name to config.json
        with open(os.path.join(dirname, 'config.json')) as json_file:
            json_data = json.load(json_file)
            json_data['name'] = new_name

        with open(os.path.join(dirname, 'config.json'), 'w') as outfile:
            json.dump(json_data, outfile)

            print('New name: ', json_data['name'])

    @property
    def id(self):
        # read id from config.json, if it doesn't exist yet, generate one and write it to config
        with open(os.path.join(dirname, 'config.json')) as json_file:
            json_data = json.load(json_file)
            if not json_data['id']:
                json_data['id'] = self.get_random_id()
                self.update_id(json_data['id'])
            return json_data['id']

    def get_random_id(self):
        return ''.join([random.choice(['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']) for n in range(10)])

    def update_id(self, new_id):
        # save id to config.json
        with open(os.path.join(dirname, 'config.json')) as json_file:
            json_data = json.load(json_file)
            json_data['id'] = new_id

        with open(os.path.join(dirname, 'config.json'), 'w') as outfile:
            json.dump(json_data, outfile)

            print('New id: ', json_data['id'])

    def signup(self):
        print('Sign up to host...')
        # try to sign up and only stop when successfull
        success = False
        while not success:
            try:
                led_strip_data = {
                    "id": self.id,
                    "name": self.name,
                    "ip_address": self.ip_address}
                request = requests.post(
                    'http://'+self.host_address+':8000/signup', json=led_strip_data)
                print(request.status_code)
                if request.status_code == 200:
                    success = True
                else:
                    time.sleep(1)
            except:
                print('signup failed...')
                time.sleep(1)
                pass

    def glow(self):
        # TODO take current mode from json and glow in infinite loop
        self.leds.rainbow_animation()

    def on(self):
        self.signup()
        self.glow()

    def off(self):
        # shutdown via linux command
        self.machine.off()
