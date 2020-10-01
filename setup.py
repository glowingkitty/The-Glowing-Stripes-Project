
# Create all folders for stripe device

import json
# move all files to the correct folders
import os


class Setup():
    def __init__(self):
        self.target_device = 'stripe'
        self.firmware_version = '1.13'
        self.dev_usb_port = '/dev/tty.usbserial-0001'

        self.folders = [
            'files_all_devices',
            'files_host',
            'files_stripe',
            'MicroWebSrv2',
            'www'
        ]

    def install_micropython(self):
        # search for firmware
        firmware = [x for x in os.listdir() if x.startswith(
            'esp32-') and x.endswith(self.firmware_version+'.bin')][0]
        # install esptool
        print('Install esptool...')
        os.system('pip install esptool')
        # format esp
        print('Format ESP...')
        os.system('esptool.py --port {} erase_flash'.format(self.dev_usb_port))
        # copy firmware
        print('Copy firmware...')
        os.system(
            'esptool.py --chip esp32 --port {} --baud 460800 write_flash -z 0x1000 {}'.format(self.dev_usb_port, firmware))

    def install_rshell(self):
        print('Install rshell for copying files to ESP...')
        os.system('pip install rshell')

    def sync_folder(self, folder):
        os.system(
            'rshell --port {} rsync -m {} /pyboard/{}'.format(
                self.dev_usb_port,
                folder,
                folder
            ))

    def copy_file(self, from_path, to_path):
        os.system('rshell --port {} cp {} /pyboard/{}'.format(
            self.dev_usb_port,
            from_path,
            to_path
        ))

    def sync_files(self):
        for folder in self.folders:
            ignore_folder = {"stripe": "host", "host": "stripe"}
            if folder != 'files_'+ignore_folder[self.target_device]:
                print('Sync folder ... ', folder)
                self.sync_folder(folder)

        self.copy_file(
            'files_{}/boot.py'.format(self.target_device), 'boot.py')

    def start(self):
        started_correct = input(
            'Did you start this script from an Python3 virtual environment?')
        if started_correct.lower() == 'y':
            target = input('Enter target device. Default "stripe"')
            if target != '':
                self.target_device = target
            print('Want to reinstall MicroPython on your ESP32?')
            install_micropython = input('Enter "y" if yes, else "n".')
            if install_micropython.lower() == 'y':
                firmware_version = input(
                    'What firmware version do you want to install? Press Enter for 1.10')
                if firmware_version != '':
                    self.firmware_version = firmware_version
                dev_usb_port = input(
                    'Connect your ESP to your computer. Once done: What is the /dev port? Press enter for /dev/tty.usbserial-0001')
                if dev_usb_port != '':
                    self.dev_usb_port = dev_usb_port
                self.install_micropython()

            self.install_rshell()
            print('Collecting and uploading files...')
            self.sync_files()
            print('Synced all files to ESP.')
            os.system('screen {} 115200'.format(self.dev_usb_port))
        else:
            print(
                'Please create a virtual python environment based on Python3 first and start the script from there.')


if __name__ == '__main__':
    Setup().start()
