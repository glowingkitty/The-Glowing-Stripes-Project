import os

import requests


class PiZeroWH:
    @property
    def connected_to_internet():
        try:
            if requests.get('https://github.com').status_code == 200:
                return True
            else:
                return False
        except:
            return False

    def off():
        # shutdown raspberry pi
        os.system("shutdown now -h")
