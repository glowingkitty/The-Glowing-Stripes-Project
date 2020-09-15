import random
import time

import network
import webrepl
from machine import Pin
from neopixel_plus import NeoPixel


class Stripes():
    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 webrepl_password='letsglow',
                 dreamcolor_input_pin_num=5,
                 led_strip_data_pin_num=4,
                 num_of_leds=60,
                 activate_webrepl_on_boot=True,
                 test=False
                 ):
        # Wlan
        self.host_wifi_name = host_wifi_name
        self.host_wifi_password = host_wifi_password
        self.webrepl_password = webrepl_password
        self.wlan = None
        self.ap = None
        self.role = None
        self.activate_webrepl_on_boot = activate_webrepl_on_boot

        # setting up the LEDs
        self.test = test
        self.dreamcolor_input_pin_num = dreamcolor_input_pin_num
        self.led_strip_data_pin_num = led_strip_data_pin_num
        self.dreamcolor_input_pin = Pin(self.dreamcolor_input_pin_num, Pin.IN)
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
        if self.test:
            print('webrepl can only be used on real hardware, not in test mode.')
        else:
            print('Activate webrepl...')
            if not self.ap:
                print('Host wifi not active yet. Start host wifi...')
                self.start_host_wifi()
            webrepl.start(password=self.webrepl_password)

    def scan_wifis(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Scan for nearby WiFi networks...')
            if not self.wlan:
                self.activate_wifi()

            return self.wlan.scan()

    def search_for_host(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            wifi_networks = self.scan_wifis()

            print('Searching for host nearby...')
            for network_info in wifi_networks:
                if self.host_wifi_name in network_info[0]:
                    print('Found existing host nearby. Connecting as participant...')
                    self.role = 'participant'
                    break
            else:
                print('Host not found. Therefore this is now a host...')
                self.role = 'host'

    def connect_to_host(self):
        if self.test:
            print('Wifi can only be used on real hardware, not in test mode.')
        else:
            print('Connecting to host...')
            self.wlan.connect(self.host_wifi_name, self.host_wifi_password)

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

    def forward_dreamcolor_signal(self):
        def callback(p):
            self.led_strip_data_pin.value(p.value())
            print(p.value())

        self.dreamcolor_input_pin.irq(handler=callback)

    def glow_test(self):
        self.leds.testing()

    def glow_random(self):
        self.leds.random_flashing()

    def glow_up_and_down(self):
        self.leds.glowup_and_down()

    def glow_rainbow(self):
        self.leds.rainbow_animation()

    def on(self):
        self.search_for_host()

        if self.role == 'participant':
            self.connect_to_host()
        else:
            self.start_host_wifi()

        if self.activate_webrepl_on_boot:
            self.activate_webrepl()

        # TODO: forwarding signal doesn't work
        # self.forward_dreamcolor_signal()

        # show turn-on animation, glow all leds
        # self.glow_test()
        self.leds.rainbow_animation()

    def restart(self):
        if self.test:
            print('Restart can only be performed on real hardware, not in test mode.')
        else:
            import machine
            machine.reset()
