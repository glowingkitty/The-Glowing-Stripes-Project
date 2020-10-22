import os


class PiZeroWH():
    def off():
        # shutdown raspberry pi
        os.system("shutdown now -h")
