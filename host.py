import json
import os
import socket
import sys
import time
from time import sleep

import network
import webrepl
from machine import Pin
from MicroWebSrv2 import *


def change_mode(mode):
    with open('www/current_mode.json') as json_file:
        json_data = json.load(json_file)
        json_data['current_mode'] = mode

    # write updated mode
    with open('www/current_mode.json', 'w') as outfile:
        json.dump(json_data, outfile)

        print('New mode: ', json_data['current_mode'])


class Host():
    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 webrepl_password='letsglow',
                 dreamcolor_input_pin_num=5,
                 test=False
                 ):
        self.root_files = os.listdir()

        # Wlan
        self.host_wifi_name = host_wifi_name
        self.host_wifi_password = host_wifi_password
        self.webrepl_password = webrepl_password
        self.wlan = None
        self.ap = None

        # setting up the LEDs
        self.test = test

    @property
    def mode(self):
        with open('www/current_mode.json') as json_file:
            json_data = json.load(json_file)
            return json_data['current_mode']

    def activate_wifi(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Activate WiFi...')
            self.wlan = network.WLAN(network.STA_IF)
            self.wlan.active(True)

    def scan_wifis(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Scan for nearby WiFi networks...')
            if not self.wlan:
                self.activate_wifi()

            return self.wlan.scan()

    def start_host_wifi(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Starting host wifi...')
            self.ap = network.WLAN(network.AP_IF)
            self.ap.active(True)
            self.ap.config(essid=self.host_wifi_name,
                           authmode=network.AUTH_WPA_WPA2_PSK,
                           password=self.host_wifi_password)
            print('Host wifi accessible now as "{}"'.format(self.host_wifi_name))

    def activate_webrepl(self):
        if self.test:
            print('webrepl can only be used on real hardware, not in test mode.')
        else:
            print('Activate webrepl...')
            if not self.ap:
                print('Host wifi not active yet. Start host wifi...')
                self.start_host_wifi()
            webrepl.start(password=self.webrepl_password)

    def check_for_updates(self):
        # Check if new files are in main folder
        root_folder = os.listdir()
        start_update = False
        for file in root_folder:
            if file not in self.root_files:
                if file.endswith('.py'):
                    print(
                        'New .py file detected: "{}" - moving it to www/ folder'.format(file))
                    os.rename(file, 'www/{}'.format(file))
                    start_update = True

                elif file.endswith('.html') or file.endswith('.css') or file.endswith('.js'):
                    print(
                        'New front-end file detected: "{}" - moving it to www/ folder...'.format(file))
                    os.rename(file, 'www/{}'.format(file))

        if start_update:
            print('Change mode to "update" to distribute updates...')
            # save current mode
            previous_mode = self.mode
            change_mode('update')
            # wait for 10 seconds, so every LED strip can get update mode, before switching back to previous mode
            time.sleep(10)
            change_mode(previous_mode)

    def start_server(self):
        print('Starting server...')

        # Define web routes
        @WebRoute(GET, '/', name='Control interface')
        def RequestLandingpage(microWebSrv2, request):
            request.Response.ReturnRedirect('/index.html')

        @WebRoute(POST, '/', name='Change LED strip mode')
        def PostNewLEDmode(microWebSrv2, request):
            data = request.GetPostedURLEncodedForm()
            print('Updating LED strip mode...')
            # TODO send new status to LED strips and make them listen via socket?

            for key in data.keys():
                change_mode(key)
                break

            request.Response.ReturnOk()

        @WebRoute(GET, '/files', name='Overview of files which can be updated')
        def RequestFiles(microWebSrv2, request):
            request.Response.ReturnOkJSON({
                'files': [x for x in os.listdir('www') if x != '__init__.py' and x.endswith('.py')]
            })

        mws2 = MicroWebSrv2()
        mws2.SetEmbeddedConfig()

        mws2._slotsCount = 4
        mws2._bindAddr = ('192.168.4.1', '80')
        mws2.StartManaged()

        # Main program loop until keyboard interrupt,
        try:
            while True:
                # check if new files available, if true, move them and activate update mode
                self.check_for_updates()
                sleep(1)
        except KeyboardInterrupt:
            mws2.Stop()

    def on(self):
        self.start_host_wifi()
        self.activate_webrepl()
        self.start_server()

    def restart(self):
        if self.test:
            print('Restart can only be performed on real hardware, not in test mode.')
        else:
            import machine
            machine.reset()
