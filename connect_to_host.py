import getopt
import os
import socket
import time
from socket import timeout

import requests

from stripe import Stripe

current_animation_rgb_colors = None
if __name__ == "__main__":
    try:
        opts, args = getopt.getopt(sys.argv[1:], "hc:", ["colors="])
    except getopt.GetoptError:
        print('connect_to_host.py -c <colors>')
        sys.exit(2)
    for opt, arg in opts:
        if opt in ("-c", "--colors"):
            current_animation_rgb_colors = eval(arg)
            if current_animation_rgb_colors == '':
                current_animation_rgb_colors = None
            elif '[' in current_animation_rgb_colors:
                current_animation_rgb_colors = current_animation_rgb_colors.replace(
                    '[', '').replace(']', '').split(',')


host_ip_address = None
wait_seconds = 10
while True:
    # every few seconds, check ip address of "theglowingstripes.local" has changed (warning: might be outdated because socket /DNS is using a cache)
    try:
        new_host_ip_address = socket.gethostbyname('theglowingstripes.local')

        if new_host_ip_address != host_ip_address:
            print('IP address of host has changed. Signing up to new host')
            # if ip address has changed, make /signup request
            Stripe(current_animation_rgb_colors=current_animation_rgb_colors).signup()
            host_ip_address = new_host_ip_address

        else:
            print('IP address of host seems to be the same. Test if host is accessible')
            # else check if host is still accessible
            try:
                response = requests.get(
                    'http://theglowingstripes.local/', timeout=2)
                if response.status_code == 200:
                    print('Yes, still online. Test again in {} seconds'.format(
                        wait_seconds))
                    print('')
            except:
                # if host not accessible, restart network manager to make this led strip the new host
                print(
                    'Host not accessible. Probably turned off. Restart avahi-daemon to make this LED strip the new host...')
                os.system('sudo systemctl restart avahi-daemon')
                print('avahi-daemon restarted')
    except socket.gaierror:
        print('theglowingstripes.local currently unavailable. Trying again in {} seconds...'.format(
            wait_seconds))

    time.sleep(wait_seconds)
