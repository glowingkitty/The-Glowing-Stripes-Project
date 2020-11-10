import os
import socket
import time
from socket import timeout

import requests

from stripe import Stripe

host_ip_address = None
while True:
    # every few seconds, check ip address of "theglowingstripes.local" has changed (warning: might be outdated because socket /DNS is using a cache)
    try:
        new_host_ip_address = socket.gethostbyname('theglowingstripes.local')

        if new_host_ip_address != host_ip_address:
            print('IP address of host has changed. Signing up to new host')
            # if ip address has changed, make /signup request
            Stripe().signup()
            host_ip_address = new_host_ip_address

        else:
            print('IP address of host seems to be the same. Test if host is accessible')
            # else check if host is still accessible
            try:
                response = requests.get(
                    'http://theglowingstripes.local/', timeout=2)
                if response.status_code == 200:
                    print('Yes, still online. Test again in 5 seconds')
                    print('')
            except:
                # if host not accessible, restart network manager to make this led strip the new host
                print(
                    'Host not accessible. Probably turned off. Restart avahi-daemon to make this LED strip the new host...')
                os.system('sudo systemctl restart avahi-daemon')
                print('avahi-daemon restarted')
    except socket.gaierror:
        print('theglowingstripes.local currently unavailable. Trying again in 5 seconds...')

    time.sleep(5)
