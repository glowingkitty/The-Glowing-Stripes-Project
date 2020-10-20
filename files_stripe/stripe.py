import json
import random

import urequests
from hardware import ESP
from machine import Pin

from neopixel_plus import NeoPixel


class Stripe():
    def __init__(self,
                 led_strip_data_pin_num=4,
                 num_of_leds=60,
                 ):

        # setting up the LEDs
        self.led_strip_data_pin_num = led_strip_data_pin_num
        self.led_strip_data_pin = Pin(self.led_strip_data_pin_num, Pin.OUT)
        self.num_of_leds = num_of_leds
        self.leds = NeoPixel(pin_num=self.led_strip_data_pin_num,
                             n=self.num_of_leds, start_point=0, brightness=1, bpp=3)

        self.esp = ESP()

    @property
    def name(self):
        # read name from config.json
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            return json_data['name']

    def update_name(self, new_name):
        # save name to config.json
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            json_data['name'] = new_name

        with open('config.json', 'w') as outfile:
            json.dump(json_data, outfile)

            print('New name: ', json_data['name'])

    @property
    def id(self):
        # read id from config.json, if it doesn't exist yet, generate one and write it to config
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            if not json_data['id']:
                json_data['id'] = self.get_random_id()
                self.update_id(json_data['id'])
            return json_data['id']

    def get_random_id(self):
        return ''.join([random.choice(['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z', '0', '1', '2', '3', '4', '5', '6', '7', '8', '9']) for n in range(10)])

    def update_id(self, new_id):
        # save id to config.json
        with open('config.json') as json_file:
            json_data = json.load(json_file)
            json_data['id'] = new_id

        with open('config.json', 'w') as outfile:
            json.dump(json_data, outfile)

            print('New id: ', json_data['id'])

    def start_server(self):
        print('Starting strip server...')
        from MicroWebSrv2 import MicroWebSrv2

        # Define web routes
        @WebRoute(POST, '/change', name='Change LED strip mode')
        def PostNewLEDmode(microWebSrv2, request):
            print('Updating LED strip mode...')
            data = request.GetPostedJSONObject()

            if data['new_mode']['name'] == 'install_updates':
                request.Response.ReturnOk()
                # TODO

            else:
                # TODO
                print('Process LED mode...')

            request.Response.ReturnOk()

        mws2 = MicroWebSrv2()
        mws2.SetEmbeddedConfig()

        mws2._slotsCount = 4
        mws2._allowAllOrigins = True
        mws2.StartManaged()

        # Tell host that LED strip is online and ready for requests via POST request to /signup
        print('Sign up to host...')
        led_strip_data = {
            "id": self.id,
            "name": self.name,
            "ip_address": self.esp.ip
        }
        print(led_strip_data)
        response = urequests.post(
            'http://192.168.4.1/signup', headers={'content-type': 'application/json'}, data=led_strip_data)
        response.close()

        # Main program loop until keyboard interrupt,
        try:
            while True:
                # TODO start glowing
                sleep(1)
        except KeyboardInterrupt:
            mws2.Stop()

    def on(self):
        self.esp.device_info()
        self.esp.connect_to_host()

        self.start_server()
