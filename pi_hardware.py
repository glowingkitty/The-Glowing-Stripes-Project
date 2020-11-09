import os

import requests

import iwlist


class PiZeroWH:
    def connected_to_internet():
        try:
            if requests.get('https://github.com').status_code == 200:
                return True
            else:
                return False
        except:
            return False

    def wifi_networks(details='basic'):
        # get wifi networks
        content = iwlist.scan(interface='wlan0')
        cells = iwlist.parse(content)

        # sort them by strength
        cells = sorted(
            cells, key=lambda wifi: wifi['signal_quality'], reverse=True)

        # return basics or all details
        if details == 'basic':
            return [{
                'essid': x['essid'],
                'encryption':x['encryption'],
                'signal_strength':x['signal_quality']
            } for x in cells]
        else:
            return cells

    def off():
        # shutdown raspberry pi
        os.system("shutdown now -h")
