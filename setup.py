
# Create all folders for stripe device

# move all files to the correct folders
import os


class Setup():
    def __init__(self):

        # TODO check if host or strip
        self.device = 'computer' if 'files_stripe' in os.listdir() else 'esp'

        self.folders = [
            'MicroWebSrv2',
            'MicroWebSrv2/libs',
            'MicroWebSrv2/mods',
            'www',
            'www/fonts',
            'www/icons',
            'www/images',
        ]

        self.files = [
            ('XAsyncSockets.py', 'MicroWebSrv2/libs/XAsyncSockets.py'),
            ('urlUtils.py', 'MicroWebSrv2/libs/urlUtils.py'),
            ('PyhtmlTemplate.py', 'MicroWebSrv2/mods/PyhtmlTemplate.py'),
            ('WebSockets.py', 'MicroWebSrv2/mods/WebSockets.py'),
            ('httpRequest.py', 'MicroWebSrv2/httpRequest.py'),
            ('httpResponse.py', 'MicroWebSrv2/httpResponse.py'),
            ('microWebSrv2.py', 'MicroWebSrv2/microWebSrv2.py'),
            ('webRoute.py', 'MicroWebSrv2/webRoute.py'),
        ]

    @property
    def already_executed(self):
        # TODO check if already executed based on created .txt file
        pass

    def mark_as_executed(self):
        # TODO create text file to mark script as executed
        pass

    def check_folders(self):
        # TODO create folder
        # TODO create __init__.py file inside folder
        pass

    def check_and_move_files(self):

        os.system(
            'python webrepl_cli.py -p letsglow 192.168.4.1:/start_webrepl.py test.py')
        pass

    def start(self):
        # check if setup.py is executed on ESP or computer

        if self.device == 'esp':
            # TODO check if setup was already executed
            if self.already_executed == False:
                self.check_folders()
                self.check_and_move_files()
                self.mark_as_executed()
        else:
            # TODO format ESP and upload files
            pass


Setup().start()
