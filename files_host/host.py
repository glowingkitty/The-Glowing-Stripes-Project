import json
import os
import time
from time import sleep

from MicroWebSrv2 import *

from hardware import ESP
from www.error import save_error

connected_led_strips = []


def add_update_led_strip(self, json_data):
    print('add_update_led_strip...')
    for entry in connected_led_strips:
        if entry['id'] == json_data['id']:
            entry = json_data
    else:
        connected_led_strips.append(json_data)

    print('Connected LED strips: '+str(connected_led_strips))


class Host():
    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 webrepl_password='letsglow',
                 test=False
                 ):
        self.esp = ESP(host_wifi_name=host_wifi_name,
                       host_wifi_password=host_wifi_password,
                       webrepl_password=webrepl_password)

        # setting up the LEDs
        self.test = test

    def check_for_updates(self):
        # Check if new files are in main folder
        root_folder = os.listdir()
        start_update = False
        for file in root_folder:
            if file not in self.esp.root_files:
                if file.endswith('.py'):
                    print(
                        'New .py file detected: "{}" - moving it to www/ folder'.format(file))
                    os.rename(file, 'www/{}'.format(file))
                    start_update = True
                    self.esp.files()
                    self.esp.get_storage()

                elif file.endswith('.html') or file.endswith('.css') or file.endswith('.js'):
                    print(
                        'New front-end file detected: "{}" - moving it to www/ folder...'.format(file))
                    os.rename(file, 'www/{}'.format(file))
                    self.esp.files()
                    self.esp.get_storage()

        if start_update:
            print('Change mode to "update" to distribute updates...')
            self.share_changes(
                {"changes": ["install_updates"]}
            )

    def share_changes(self, changes):
        # TODO
        print()

    def start_server(self):
        print('Starting server...')

        # Define web routes
        @WebRoute(GET, '/', name='Control interface')
        def RequestLandingpage(microWebSrv2, request):
            request.Response.ReturnRedirect('/index.html')

        @WebRoute(POST, '/signup', name='Signup new LED strip')
        def SignupLEDstrip(microWebSrv2, request):
            print('A new LED strip connected...')
            data = request.GetPostedJSONObject()

            # see if LED strip is already saved in database, if yes, update IP, else: create new entry
            add_update_led_strip(data)

            request.Response.ReturnOk()
            print(data['name']+' connected (id: '+data['id']+')')

        @WebRoute(POST, '/change', name='Change LED strips')
        def PostNewLEDmode(microWebSrv2, request):
            print('Updating LED strips...')
            data = request.GetPostedJSONObject()

            # TODO replace IDs in request with current IPs & send requests

            # TODO send new status to LED strips via /change POST request

            request.Response.ReturnOk()

        @WebRoute(GET, '/files', name='Overview of files which can be updated')
        def RequestFiles(microWebSrv2, request):
            request.Response.ReturnOkJSON({
                'files': [x for x in os.listdir('www') if x != '__init__.py' and x.endswith('.py')]
            })

        mws2 = MicroWebSrv2()
        mws2.SetEmbeddedConfig()

        mws2._slotsCount = 4
        mws2._bindAddr = ('192.168.4.1', '80')
        mws2._allowAllOrigins = True
        mws2.StartManaged()

        # Main program loop until keyboard interrupt,
        try:
            while True:
                # check if new files available, if true, move them and activate update mode
                self.check_for_updates()
                sleep(1)
        except KeyboardInterrupt:
            mws2.Stop()

    def on(self):
        self.esp.device_info()
        self.esp.start_host_wifi()
        self.esp.activate_webrepl()
        self.start_server()
