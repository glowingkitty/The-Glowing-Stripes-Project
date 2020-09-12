import gc
import sys
from os import listdir

import esp
import machine
import network
import uos
import webrepl

esp.osdebug(None)
# uos.dupterm(None, 1) # disable REPL on UART(0)

# activate wifi
print('Activate WiFi...')
wlan = network.WLAN(network.STA_IF)
wlan.active(True)
wifi_networks = wlan.scan()

# search for wifi hotspot
print('Searching for host nearby...')
host_wifi_name = 'GlowingStripes'
host_wifi_password = 'letsglow'

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
# if participant: get signal from host via wifi

webrepl.start()
gc.collect()
