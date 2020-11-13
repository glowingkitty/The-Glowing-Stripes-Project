import os
import subprocess

import requests

import iwlist


class PiZeroWH:
    def default_wifi(with_password=True):
        details = {
            'essid': 'TheGlowingStripes',
            'psk': 'letsglow',
            'key_mgmt': 'WPA-PSK'
        } if with_password == True else {
            'essid': 'TheGlowingStripes',
            'current_wifi': False,
            'encryption': 'WPA-PSK',
            'signal_strength': 100
        }
        return details

    def connected_to_internet():
        try:
            if requests.get('https://github.com').status_code == 200:
                return True
            else:
                return False
        except:
            return False

    def current_wifi_network():
        try:
            essid = str(subprocess.run(["iwgetid"], capture_output=True).stdout).split(
                'ESSID:"')[1].split('"')[0]
        except:
            essid = None

        return essid

    def wifi_networks():
        # get wifi networks
        content = iwlist.scan(interface='wlan0')
        networks = iwlist.parse(content)

        # return basics or all details
        current_wifi = PiZeroWH.current_wifi_network()
        networks = [{
            'essid': x['essid'],
            'current_wifi':True if current_wifi == x['essid'] else False,
            'is_default_wifi':True if PiZeroWH.default_wifi()['essid'] == x['essid'] else False,
            'encryption':'WPA-PSK' if x['encryption'] == 'wpa2' else x['encryption'],
            'signal_strength':int(x['signal_quality'])
        } for x in networks]

        # sort them by strength
        networks = sorted(
            networks, key=lambda wifi: wifi['signal_strength'], reverse=True)
        networks = sorted(
            networks, key=lambda wifi: wifi['current_wifi'], reverse=True)

        return networks

    def saved_wifis():
        # open wpa_supplicant.conf
        wpa_supplicant = open(
            "/etc/wpa_supplicant/wpa_supplicant.conf", "r").read()

        # parse networks
        networks_string = 'network='+wpa_supplicant.split('network=', 1)[1]
        networks = [{
            'essid': x.split('ssid="')[1].split('"')[0],
            'psk':x.split('psk="')[1].split('"')[0] if 'psk' in x else None,
            'key_mgmt':x.split('key_mgmt=')[1].split('\n')[0] if 'key_mgmt' in x else None,
        } for x in networks_string.split('network=') if x != '']

        return networks

    def wpa_supplicant_header():
        wpa_supplicant = open(
            "/etc/wpa_supplicant/wpa_supplicant.conf", "r").read()
        header = wpa_supplicant.split('network=', 1)[0]
        return header

    def get_networks_text(networks):
        # make sure to always have PiZeroWH.default_wifi() added as well
        default_wifi = PiZeroWH.default_wifi()
        if default_wifi not in networks:
            networks += [default_wifi]

        networks_text = ''
        for network in networks:
            networks_text += '\n'
            networks_text += 'network={\n'
            networks_text += '        ssid="{}"\n'.format(
                network['essid'])
            networks_text += '        psk="{}"\n'.format(network['psk'])
            networks_text += '        key_mgmt={}\n'.format(
                network['key_mgmt'])
            networks_text += '}\n'
        return networks_text

    def add_wifi(essid, password, encryption='WPA-PSK', position='start'):
        # add wifi to wpa_supplicant.conf, if it doesn't exist yet
        networks = PiZeroWH.saved_wifis()

        # check if wifi already in list
        new_wifi = {'essid': essid, 'psk': password, 'key_mgmt': encryption}
        if new_wifi in networks:
            if position == 'start' and new_wifi == networks[0]:
                print(
                    '"{}" already saved as No. 1 wifi in wpa_supplicant.conf'.format(essid))
            elif position != 'start' and new_wifi != networks[0]:
                print('"{}" already saved in wpa_supplicant.conf'.format(essid))
            else:
                print('Update order of wifi networks...')
                if position == 'start':
                    networks = [new_wifi] + \
                        [x for x in networks if x != new_wifi]
                else:
                    networks = [x for x in networks if x !=
                                new_wifi]+[new_wifi]

                wpa_supplicant = PiZeroWH.wpa_supplicant_header()+PiZeroWH.get_networks_text(networks)
                f = open("/etc/wpa_supplicant/wpa_supplicant.conf", "w")
                f.write(wpa_supplicant)
                f.close()
                print('Updated order of wifi networks')

        else:
            print('Adding {} to wpa_supplicant.conf...'.format(essid))
            if position == 'start':
                networks.insert(0, new_wifi)
            else:
                networks += [new_wifi]

            # build new wpa_supplicant.conf
            wpa_supplicant = PiZeroWH.wpa_supplicant_header()+PiZeroWH.get_networks_text(networks)
            f = open("/etc/wpa_supplicant/wpa_supplicant.conf", "w")
            f.write(wpa_supplicant)
            f.close()
            print('Added {} to wpa_supplicant.conf'.format(essid))

    def remove_wifi(essid):
        default_wifi = PiZeroWH.default_wifi()
        if essid == default_wifi['essid']:
            print('Cannot remove {}, its the default wifi network TheGlowingStripesProject will create in case there is no other wifi accessible'.format(
                default_wifi['essid']))
        else:
            print('Remove {} from wpa_supplicant.conf...'.format(essid))
            networks = [x for x in PiZeroWH.saved_wifis()
                        if x['essid'] != essid]
            wpa_supplicant = PiZeroWH.wpa_supplicant_header()+PiZeroWH.get_networks_text(networks)
            f = open("/etc/wpa_supplicant/wpa_supplicant.conf", "w")
            f.write(wpa_supplicant)
            f.close()
            print('Updated wpa_supplicant.conf')

    def restart_wifi():
        # restart wifi related services
        print('Restarting wifi (autohotspot.service)...')
        os.system('sudo systemctl restart autohotspot')
        print('Wifi restarted')

    def connect_to_wifi(essid, password, encryption):
        # add wifi if it doesn't exist yet
        PiZeroWH.add_wifi(essid, password, encryption)
        PiZeroWH.restart_wifi()

    def disconnect_from_wifi(essid):
        # disconnect from wifi by moving "TheGlowingStripes" default network to top and restart wifi (authotspot.service)
        default_wifi = PiZeroWH.default_wifi()
        PiZeroWH.remove_wifi(essid)
        PiZeroWH.add_wifi(
            default_wifi['essid'], default_wifi['psk'], default_wifi['key_mgmt'])
        PiZeroWH.restart_wifi()

    def off():
        # shutdown raspberry pi
        os.system("shutdown now -h")
