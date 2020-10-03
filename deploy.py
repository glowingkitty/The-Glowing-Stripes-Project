
import argparse
import os


class Deploy():
    def __init__(self):
        parser = argparse.ArgumentParser()
        parser.add_argument("-t", "--target", required=False)
        parser.add_argument("-f", "--firmware", required=False)
        parser.add_argument("-p", "--port", required=False)

        parser.add_argument("-fsh", "--flashfirmware", required=False)
        parser.add_argument("-us", "--uploadserver", required=False)
        args = parser.parse_args()

        self.target_device = args.target if args.target else 'stripe'
        self.firmware_version = args.target if args.target else '1.13'
        self.dev_usb_port = args.target if args.target else '/dev/tty.usbserial-0001'

        self.flash_firmware = args.target if args.target else 'True'
        self.deploy_server = args.target if args.target else 'True'

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
        if self.flash_firmware == 'True':
            self.install_micropython()

        self.install_rshell()
        if self.deploy_server == 'True':
            print('Collecting and uploading files...')
            self.sync_files()
            print('Synced all files to ESP.')
        if self.deploy_server == 'False':
            self.copy_file('files_stripe/neopixel_plus.py', 'neopixel_plus.py')
            self.copy_file('simple_rainbow.py', 'boot.py')
        os.system('screen {} 115200'.format(self.dev_usb_port))


if __name__ == '__main__':
    Deploy().start()
