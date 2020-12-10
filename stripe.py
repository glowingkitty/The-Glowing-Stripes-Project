import json
import os
import random
import signal
import subprocess
import time
from random import randint

import requests

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
                 python_location='/home/host/the-glowing-stripes-project/pyvenv/bin/python',
                 project_path='/home/host/the-glowing-stripes-project',
                 neopixel_plus_package_path='/home/host/the-glowing-stripes-project/pyvenv/lib/python3.7/site-packages/neopixel_plus',
                 led_strip_data_pin_num=StripeConfig.config()[
                     'led_strip_data_pin_num'],
                 num_of_leds=StripeConfig.config()['num_of_leds'],
                 host_address=StripeConfig.config()['host_address'],
                 current_animation_rgb_colors=None
                 ):
        self.host_address = host_address

        # setting up the LEDs
        self.led_strip_data_pin_num = led_strip_data_pin_num
        self.num_of_leds = num_of_leds
        self.current_animation = None
        self.current_animation_rgb_colors = current_animation_rgb_colors
        self.python_location = python_location
        self.project_path = project_path
        self.neopixel_plus_package_path = neopixel_plus_package_path

        self.machine = PiZeroWH

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
        # if self.current_animation_rgb_colors: overwrite rgb colors with current_animation_rgb_colors
        animation = LEDanimations().last_used
        if self.current_animation_rgb_colors and 'rgb_colors' in animation['customization']:
            animation['customization']['rgb_colors'] = self.current_animation_rgb_colors
        return animation

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
        if animation['id'] == '0000000000':
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
            try:
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
            except requests.exceptions.ConnectionError:
                print('Server not online yet. Try again in 2 seconds...')
                time.sleep(2)
                pass

    def glow(self,
             id=None,
             based_on=None,
             customization={}):
        # take current mode from json and glow in infinite loop
        if not id:
            last_animation = self.last_animation
            id = last_animation['id']
            if 'based_on' in last_animation and last_animation['based_on']:
                based_on = last_animation['based_on']
            if 'customization' in last_animation and last_animation['customization']:
                customization = last_animation['customization']

        # get animation with all the details
        selected_animation = LEDanimations().get_animation(id)

        # replace "random" string with random generated colors
        if 'colors_selected' in customization and customization['colors_selected'] == 'random':
            customization['rgb_colors'] = [[randint(0, 255), randint(0, 255), randint(
                0, 255)] for x in range(0, customization['num_random_colors'])]
            self.current_animation_rgb_colors = customization['rgb_colors']

        # play animation
        if self.current_animation:
            print('Stopping previous animation...')
            self.current_animation.send_signal(signal.SIGINT)
        if id == '1111111111':
            print('/off requested. LEDs are turned off (Pi stays awake)')
        else:
            print('Play animation...')
            command = [
                self.python_location,
                self.neopixel_plus_package_path+'/neopixel_plus.py',
                '-a',
                selected_animation['neopixel_plus_function'],
                '-d',
                'adafruit',
                '-n',
                str(self.num_of_leds),
                '-c',
                str(customization)
            ]
            self.current_animation = subprocess.Popen(command)

    def off(self):
        # shutdown via linux command
        self.current_animation.send_signal(signal.SIGINT)
        self.machine.off()
