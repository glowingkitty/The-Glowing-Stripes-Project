

import requests
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setup(3, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.wait_for_edge(3, GPIO.FALLING)

# shutdown all led strips via POST request to server
try:
    print('Got shut down request! Shutting down via http://theglowingstripes.local/shutdown_all_led_strips ...')
    requests.post("http://theglowingstripes.local/shutdown_all_led_strips")
except:
    print('Shutdown failed. Server offline? Doing it the other way - using "shutdown now -h"')
    import os

    # and in case server is offline, only shutdown host
    os.system("shutdown now -h")
