from stripe import Stripe
import logging
import os
import socket
import time
from socket import timeout

import requests

# Get an instance of a logger
logger = logging.getLogger(__name__)


host_ip_address = None
wait_seconds = 10
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
                    logger.info('Yes, still online. Test again in {} seconds\n'.format(
                        wait_seconds))
            except:
                # if host not accessible, restart network manager to make this led strip the new host
                logger.warning(
                    'Host not accessible. Probably turned off. Restart avahi-daemon to make this LED strip the new host...')
                os.system('sudo systemctl restart avahi-daemon')
                logger.warning('avahi-daemon restarted')
    except socket.gaierror:
        logger.error('theglowingstripes.local currently unavailable. Trying again in {} seconds...'.format(
            wait_seconds))

    time.sleep(wait_seconds)
