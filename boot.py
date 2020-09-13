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
dreamcolor_input_pin = 5
led_strip_data_pin = 4
#################

esp.osdebug(None)
# uos.dupterm(None, 1) # disable REPL on UART(0)

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

# start webrepl to interact with ESP, update code and test
webrepl.start()
gc.collect()

# if host: get signal from Dreamcolor Microcontroller & share it via wifi
test_counter = 0
while test_counter < 100:
    dreamcolor_input = Pin(dreamcolor_input_pin, Pin.IN).value()
    print('dreamcolor_input: '+str(dreamcolor_input))
    Pin(led_strip_data_pin, Pin.OUT).value(dreamcolor_input)
    test_counter += 1


# if participant: get signal from host via wifi
