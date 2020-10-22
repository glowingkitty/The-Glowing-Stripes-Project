import socket
import subprocess


def get_ip(host_address='raspberrypi.local'):
    return subprocess.getstatusoutput('hostname -I')[1].replace(' ', '') if host_address == 'raspberrypi.local' else socket.gethostbyname_ex(socket.gethostname())[-1][-1]
