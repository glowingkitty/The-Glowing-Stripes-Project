

import requests
import RPi.GPIO as GPIO

GPIO.setmode(GPIO.BCM)
GPIO.setup(3, GPIO.IN, pull_up_down=GPIO.PUD_UP)
GPIO.wait_for_edge(3, GPIO.FALLING)

# shutdown all led strips via POST request to server
try:
    requests.post("http://raspberrypi.local/shutdown_all_led_strips")
except:
    import os

    from stripe import Stripe
    Stripe().off()
    # and in case server is offline, only shutdown host
    os.system("shutdown now -h")
