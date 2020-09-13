import gc
import sys
from os import listdir

import esp
import network
import uos
import webrepl
from machine import Pin

################
# Settings #####
################
host_wifi_name = 'GlowingStripes'
host_wifi_password = 'letsglow'
dreamcolor_input_pin_num = 5
led_strip_data_pin_num = 4
#################

# define pins
dreamcolor_input_pin = Pin(dreamcolor_input_pin_num, Pin.IN)
led_strip_data_pin = Pin(led_strip_data_pin_num, Pin.OUT)

esp.osdebug(None)
gc.collect()

# activate wifi
print('Activate WiFi...')
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wifi_networks = wlan.scan()

# search for wifi hotspot
print('Searching for host nearby...')
for network_info in wifi_networks:
    if host_wifi_name in network_info[0]:
        print('Found existing host nearby. Connecting as participant...')
        role = 'participant'
        break
else:
    print('Host not found. Therefore this is now a host...')
    role = 'host'

# if hotspot found, connect ⇒ role: participant
if role == 'participant':
    print('Connecting to host...')
    wlan.connect(host_wifi_name, host_wifi_password)


# else create wifi hotspot ⇒ role: host
elif role == 'host':
    print('Starting host wifi...')
    ap = network.WLAN(network.AP_IF)
    ap.active(True)
    ap.config(essid=host_wifi_name,
              authmode=network.AUTH_WPA_WPA2_PSK,
              password=host_wifi_password)

# if host: get signal from Dreamcolor Microcontroller & share it via wifi


def callback(p):
    led_strip_data_pin.value(p.value())


dreamcolor_input_pin.irq(trigger=Pin.IRQ_RISING |
                         Pin.IRQ_FALLING, handler=callback)

# test with oscilloscope (signal)

# if participant: get signal from host via wifi
