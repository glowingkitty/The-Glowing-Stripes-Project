import os
import socket

import network
import urequests
import webrepl
from machine import Pin, Timer
from neopixel_plus import NeoPixel


class Stripe():
    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 host_server_ip='192.168.4.1',
                 webrepl_password='letsglow',
                 led_strip_data_pin_num=4,
                 num_of_leds=60,
                 test=False
                 ):
        # Wlan
        self.host_wifi_name = host_wifi_name
        self.host_wifi_password = host_wifi_password
        self.host_server_ip = host_server_ip
        self.webrepl_password = webrepl_password
        self.current_mode = None
        self.wlan = None

        # setting up the LEDs
        self.test = test
        self.led_strip_data_pin_num = led_strip_data_pin_num
        self.led_strip_data_pin = Pin(self.led_strip_data_pin_num, Pin.OUT)
        self.num_of_leds = num_of_leds
        self.leds = NeoPixel(pin_num=self.led_strip_data_pin_num,
                             n=self.num_of_leds, start_point=0, brightness=1, bpp=3, test=self.test)

    def activate_wifi(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Activate WiFi...')
            self.wlan = network.WLAN(network.STA_IF)
            self.wlan.active(True)

    def activate_webrepl(self):
        print('Starting LEDstrip wifi...')
        self.ap = network.WLAN(network.AP_IF)
        self.ap.active(True)
        self.ap.config(essid='LEDstrip',
                       authmode=network.AUTH_WPA_WPA2_PSK,
                       password=self.host_wifi_password)
        print('LEDstrip wifi accessible now')
        webrepl.start(password=self.webrepl_password)

    def scan_wifis(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Scan for nearby WiFi networks...')
            if not self.wlan:
                self.activate_wifi()

            return self.wlan.scan()

    def connect_to_host(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            wifi_networks = self.scan_wifis()

            print('Searching for host nearby...')
            for network_info in wifi_networks:
                if self.host_wifi_name in network_info[0]:
                    print('Found existing host nearby. Connecting to host...')

                    if not self.wlan.isconnected():
                        print('connecting to network...')
                        self.wlan.connect(self.host_wifi_name,
                                          self.host_wifi_password)
                        while not self.wlan.isconnected():
                            pass
                    print('Connected to host:', self.wlan.ifconfig())

                    break
            else:
                print('Host not found. Please turn on the host.')

    def glow_boot(self, num_of_leds):
        self.leds.count_glowing(limit=num_of_leds)

    def glow_test(self):
        self.leds.testing()

    def glow_up_and_down(self, limit=1):
        self.leds.glowup_and_down(limit=limit)

    def glow_rainbow(self, limit=1):
        self.leds.rainbow_animation(limit=limit)

    def get_current_mode(self, other_arguments=None):
        print('Request mode ...')
        response = urequests.get('http://192.168.4.1/current_mode.json')
        parsed = response.json()
        response.close()
        self.current_mode = parsed['current_mode']
        print('mode: ', self.current_mode)

    def install_updates(self):
        print('Search for updates...')
        self.glow_boot(3)
        response = urequests.get('http://192.168.4.1/files')
        parsed = response.json()
        response.close()

        self.glow_boot(2)
        print('Download files...', str(parsed['files']))
        for file_name in parsed['files']:
            print('Download "{}"...'.format(file_name))
            response = urequests.get('http://192.168.4.1/'+file_name)
            content = response.content
            response.close()

            f = open(file_name, "w")
            f.write(content)
            f.close()
        print('Updated files...')

        self.glow_boot(1)
        print('Restarting LED strip...')
        self.restart()

    def get_signal(self):
        tim1 = Timer(1)
        tim1.init(callback=self.get_current_mode, period=10000)

        while True:

            if self.current_mode == 'glow_rainbow':
                self.glow_rainbow()
            elif self.current_mode == 'glow_up_and_down':
                self.glow_up_and_down()
            elif self.current_mode == 'update':
                self.install_updates()

    def on(self):
        self.glow_boot(1)
        self.device_info()
        self.connect_to_host()

        self.glow_boot(2)
        self.get_signal()

    def restart(self):
        if self.test:
            print('Restart can only be performed on real hardware, not in test mode.')
        else:
            import machine
            machine.reset()

    def get_storage(self):
        result = os.statvfs('/')
        print('Total storage: {}kb'.format(result[0]))
        print('Free storage: {}kb'.format(result[3]))

    def files(self):
        folders = []
        for filename in os.listdir():
            if '.'not in filename:
                folders.append(filename)
        print('/ -> {}'.format([x for x in os.listdir() if '.' in x]))
        for folder in folders:
            try:
                print('/{}/ -> {}'.format(folder,
                                          [x for x in os.listdir(folder) if '.' in x]))
            except:
                pass

    def device_info(self):
        print('///////////////////////////////////////////////')
        print('/// The Glowing Stripes Project | LED strip ///')
        print('//////////////////////////////////////////////')
        print()
        print('//////////////')
        print('/// Files: ///')
        print('//////////////')
        self.files()
        print()
        self.get_storage()
        print()
        print()
