import os
import socket
import sys
import time

import network
import webrepl
from machine import Pin
from update import Update


class Host():
    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 webrepl_password='letsglow',
                 dreamcolor_input_pin_num=5,
                 test=False
                 ):
        # Wlan
        self.host_wifi_name = host_wifi_name
        self.host_wifi_password = host_wifi_password
        self.webrepl_password = webrepl_password
        self.wlan = None
        self.ap = None

        # setting up the LEDs
        self.test = test
        self.dreamcolor_input_pin_num = dreamcolor_input_pin_num
        self.dreamcolor_input_pin = Pin(self.dreamcolor_input_pin_num, Pin.IN)

        # setting up server
        self.default_mode = 'glow_rainbow'

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

    def forward_dreamcolor_signal(self):
        def callback(p):
            self.led_strip_data_pin.value(p.value())
            print(p.value())

        self.dreamcolor_input_pin.irq(handler=callback)

    def activate_webrepl(self):
        if self.test:
            print('webrepl can only be used on real hardware, not in test mode.')
        else:
            print('Activate webrepl...')
            if not self.ap:
                print('Host wifi not active yet. Start host wifi...')
                self.start_host_wifi()
            webrepl.start(password=self.webrepl_password)

    def start_server(self):
        print('Starting server...')
        self.server_addr = socket.getaddrinfo('192.168.4.1', 80)[0][-1]

        self.server_socket = socket.socket()
        self.server_socket.bind(self.server_addr)
        self.server_socket.listen(10)

        print('Started istening on', self.server_addr)

        # while True:
        #     self.server_cl, self.server_addr = self.server_socket.accept()
        #     print('client connected from', self.server_addr)

        #     f = open('stripe.py', 'wb')  # open in binary
        #     while (True):
        #         # receive data and write it to file
        #         l = self.server_cl.recv(1024)
        #         while (l):
        #             f.write(l)
        #             l = self.server_cl.recv(1024)
        #     f.close()

        #     self.server_cl.close()

        # self.server_socket.close()

        while True:
            self.server_cl, self.server_addr = self.server_socket.accept()
            print('client connected from', self.server_addr)
            self.server_cl.send(
                'HTTP/1.0 200 OK\r\nContent-type: application/json\r\n\r\n')
            self.server_cl.send(
                """{"current_mode": """+('"'+self.default_mode+'"')+"""}""")
            self.server_cl.close()

    def on(self):
        self.start_host_wifi()

        # TODO: forwarding signal doesn't work
        # self.forward_dreamcolor_signal()

        self.start_server()

    def restart(self):
        if self.test:
            print('Restart can only be performed on real hardware, not in test mode.')
        else:
            import machine
            machine.reset()
