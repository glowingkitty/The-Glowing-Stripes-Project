
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

    def start(self):
        if self.flash_firmware == 'True':
            self.install_micropython()

        self.install_rshell()
        self.sync_folder('neopixel_plus')
        self.copy_file('led_test.py', 'boot.py')
        os.system('screen {} 115200'.format(self.dev_usb_port))


if __name__ == '__main__':
    Deploy().start()
