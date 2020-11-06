import socket
import subprocess


def get_ip(host_address='theglowingstripes.local'):
    return subprocess.getstatusoutput('hostname -I')[1].replace(' ', '') if host_address == 'theglowingstripes.local' else socket.gethostbyname_ex(socket.gethostname())[-1][-1]
