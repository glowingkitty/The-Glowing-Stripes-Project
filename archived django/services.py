import os
from shutil import copyfile


class Services():
    def __init__(self,
                 glowingstripes_systemd_folder='/home/host/the-glowing-stripes-project/services/',
                 exclude_from_restart=['autohotspot.service'],
                 systemd_system_services=['autohotspot.service'],
                 ):
        self.systemd_folder = '/etc/systemd/system/'
        self.glowingstripes_systemd_folder = glowingstripes_systemd_folder
        self.exclude_from_restart = exclude_from_restart
        self.systemd_system_services = systemd_system_services

    @property
    def all(self):
        services = [x for x in os.listdir(self.glowingstripes_systemd_folder) if x.endswith(
            '.service') and x != 'server.service']
        services += ['server.service']
        return services

    @property
    def systemd_files(self):
        return [x for x in os.listdir(self.systemd_folder) if x.endswith('.service')]

    def add(self, name):
        if not name.endswith('.service'):
            name = name+'.service'

        # remove link first,then add it
        if os.path.isfile(self.systemd_folder+name):
            os.unlink(self.systemd_folder+name)

        # link file
        if os.path.isfile(self.glowingstripes_systemd_folder + name):

            os.symlink(self.glowingstripes_systemd_folder +
                       name, self.systemd_folder+name)

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

        if name in self.systemd_system_services:
            print(
                'Cannot stop {}, its an essential service. If you are 100 percent sure, stop it manually.')
        else:
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

        if name in self.systemd_system_services:
            print(
                'Cannot remove {}, its an essential service. If you are 100 percent sure, remove it manually.')
        else:
            if not os.path.isfile(self.systemd_folder + name):
                print('service {} doesnt exist'.format(name))
            else:
                # disable the new service
                self.stop(name)
                self.disable(name)
                os.unlink(self.systemd_folder+name)
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
            if service_file not in self.exclude_from_restart:
                self.restart(service_file)

    def remove_all(self):
        for service_file in self.all:
            self.remove(service_file)
