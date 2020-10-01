
# Create all folders for stripe device

import json
# move all files to the correct folders
import os


class Setup():
    def __init__(self,
                 target_device='stripe',
                 target_ip='192.168.4.1',
                 webrepl_password='letsglow',
                 firmware_version='1.13'
                 ):
        # check if host or strip
        self.firmware_version = firmware_version
        self.device = 'computer' if 'files_stripe' in os.listdir() else 'esp'
        self.target_device = target_device
        self.target_ip = target_ip
        self.webrepl_password = webrepl_password

        self.folders = [
            'files_all_devices',
            'files_host',
            'files_stripe',
            'MicroWebSrv2',
            'MicroWebSrv2/libs',
            'MicroWebSrv2/mods',
            'www',
            'www/fonts',
            'www/icons',
            'www/images',
        ]

    @property
    def already_executed(self):
        # check if already executed based on created .txt file
        return 'setup_completed.txt' in os.listdir()

    def mark_as_executed(self):
        # create text file to mark script as executed
        with open('setup_completed.txt', 'w') as fp:
            pass

    def check_folders(self):
        # TODO create folder
        # TODO create __init__.py file inside folder
        pass

    def check_and_move_files(self):
        # TODO
        pass

    def install_micropython(self):
        # TODO
        pass

    def save_file_list(self):
        files = [('files.json', 'files.json')]
        for folder in self.folders:
            files += [(x, folder+'/'+x) for x in os.listdir(folder)
                      if '.' in x and x != '__init__.py']

        with open('files.json', 'w') as outfile:
            json.dump({
                "files": files
            }, outfile)

            print('Saved filelist')

    def upload_files(self):
        with open('config.json') as json_file:
            files = json.load(json_file)['files']

            for file_entry in files:
                ignore_folder = {"stripe": "host", "host": "stripe"}
                if not 'files_'+ignore_folder[self.target_device] in file_entry[1]:
                    print('Upload... ', file_entry[1])
                    os.system(
                        'python webrepl_cli.py -p {} {} {}:/{}'.format(
                            self.webrepl_password,
                            file_entry[1],
                            self.target_ip,
                            file_entry[0]
                        ))

    def start(self):
        # check if setup.py is executed on ESP or computer
        if self.device == 'esp':
            # check if setup was already executed
            if self.already_executed == False:
                self.check_folders()
                self.check_and_move_files()
                self.mark_as_executed()
        else:
            self.install_micropython()
            # self.setup_webrepl()
            # self.save_file_list()
            # self.upload_files()
            # os.remove('files.json')
            # print('Copied all files to ESP. Please restart ESP to finish the setup.')


Setup().start()
