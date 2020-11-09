import os
from shutil import copyfile


class Services():
    def __init__(self,
                 systemd_folder='/etc/systemd/system/',
                 glowingstripes_systemd_folder='/home/host/the-glowing-stripes-project/services/'
                 ):
        self.systemd_folder = systemd_folder
        self.glowingstripes_systemd_folder = glowingstripes_systemd_folder

    @property
    def all(self):
        return [x for x in os.listdir(self.glowingstripes_systemd_folder) if x.endswith('.service')]

    @property
    def systemd_files(self):
        return [x for x in os.listdir(self.systemd_folder) if x.endswith('.service')]

    def add(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        # copy over in systemd folder, if already exists, overwrite it
        if os.path.isfile(self.glowingstripes_systemd_folder + name):
            copyfile(self.glowingstripes_systemd_folder + name,
                     self.systemd_folder + name)

            # reload the service files to include the new service
            os.system('sudo systemctl daemon-reload')
            print('Added {}'.format(name))
        else:
            print('{} doesnt exist in {}'.format(
                name, self.glowingstripes_systemd_folder))

    def start(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        # check if service already copied over, else, copy over first
        if not os.path.isfile(self.systemd_folder + name):
            self.add(name)

        # start the new service
        os.system('sudo systemctl start {}'.format(name))
        print('Started {}'.format(name))

    def enable(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        # check if service already copied over, else, copy over first
        if not os.path.isfile(self.systemd_folder + name):
            self.add(name)

        # start the new service
        os.system('sudo systemctl enable {}'.format(name))
        print('Enabled {}'.format(name))

    def disable(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        if not os.path.isfile(self.systemd_folder + name):
            print('service {} doesnt exist'.format(name))
        else:
            os.system('sudo systemctl disable {}'.format(name))
            print('Disabled {}'.format(name))

    def status(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        if not os.path.isfile(self.systemd_folder + name):
            print('service {} doesnt exist'.format(name))
        else:
            os.system('sudo systemctl status {}'.format(name))

    def stop(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        if not os.path.isfile(self.systemd_folder + name):
            print('service {} doesnt exist'.format(name))
        else:
            os.system('sudo systemctl stop {}'.format(name))
            print('Stopped {}'.format(name))

    def restart(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        if not os.path.isfile(self.systemd_folder + name):
            print('service {} doesnt exist'.format(name))
        else:
            os.system('sudo systemctl restart {}'.format(name))
            print('Restarted {}'.format(name))

    def remove(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        if not os.path.isfile(self.systemd_folder + name):
            print('service {} doesnt exist'.format(name))
        else:
            # disable the new service
            self.stop(name)
            self.disable(name)
            os.remove(self.systemd_folder + name)
            os.system('sudo systemctl daemon-reload')
            os.system('sudo systemctl reset-failed')
            print('Removed {}'.format(name))

    def add_all(self):
        for service_file in self.all:
            self.add(service_file)

    def start_all(self):
        for service_file in self.all:
            self.start(service_file)

    def enable_all(self):
        for service_file in self.all:
            self.enable(service_file)

    def stop_all(self):
        for service_file in self.all:
            self.stop(service_file)

    def restart_all(self):
        for service_file in self.all:
            self.restart(service_file)

    def remove_all(self):
        for service_file in self.all:
            self.remove(service_file)
