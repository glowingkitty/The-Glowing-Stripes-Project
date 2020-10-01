import os

from machine import Pin

from hardware_networking import Networking


class ESP():
    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 webrepl_password='letsglow'
                 ):

        self.networking = Networking(
            host_wifi_name=host_wifi_name,
            host_wifi_password=host_wifi_password,
            webrepl_password=webrepl_password)

    def restart(self):
        print('Restarting host...')
        machine.reset()

    def get_storage(self):
        result = os.statvfs('/')
        print()
        print('Total storage: {}kb'.format(result[0]))
        print('Free storage: {}kb'.format(result[3]))
        print()

    def files(self):
        print()
        print('//////////////')
        print('/// Files: ///')
        print('//////////////')
        folders = []
        for filename in os.listdir():
            if '.'not in filename:
                folders.append(filename)
        print('/ -> {}'.format([x for x in os.listdir() if '.' in x]))
        for folder in folders:
            try:
                print('/{}/ -> {}'.format(folder,
                                          [x for x in os.listdir(folder) if '.' in x]))
            except:
                pass
        print()

    def device_info(self):
        print('//////////////////////////////////////////')
        print('/// The Glowing Stripes Project /////////')
        print('/////////////////////////////////////////')
        self.files()
        self.get_storage()
