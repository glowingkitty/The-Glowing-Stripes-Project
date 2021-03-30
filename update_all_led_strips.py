import json
import os
import socket
import time
from shutil import copyfile

import requests


def hostname_resolves(hostname):
    try:
        socket.gethostbyname(hostname)
        return 1
    except socket.error:
        return 0

print("Start updating all LED strips...")

# process LED strip chips first 

# backup /data/stripe_config.json to /data/stripe_config.json
copyfile("./esp32_webserver/data/stripe_config.json", "./esp32_webserver/data/stripe_config_backup.json")
copyfile("./esp32_leds/data/stripe_config.json", "./esp32_leds/data/stripe_config_backup.json")

# for every json file in /stripe_configs:
success = []
offline = []
for filename in [x for x in os.listdir("./stripe_configs") if x.endswith(".json")]:

    # load json file and id
    with open("./stripe_configs/"+filename) as json_file:
        data = json.load(json_file)
        led_strip_id = data["0"]
        led_strip_name = data["1"]

    # check if domain is online (led_strip__<id>__server.local)
    if hostname_resolves("led_strip__"+led_strip_id+"__server.local"):
        print("LED strip (webserver) '"+led_strip_name+"' ("+led_strip_id+") online. Start OTA update...")
        # if yes:
        # copy json file to /data/stripe_config.json
        copyfile("./stripe_configs/"+filename, "./esp32_webserver/data/stripe_config.json")

        print("Uploading code to led_strip__"+led_strip_id+"__server.local...")
        time.sleep(2)
        os.system("cd esp32_webserver && pio run -t upload --upload-port led_strip__"+led_strip_id+"__server.local")

        print("Waiting for LED strip (webserver) to be online again...")
        while hostname_resolves("led_strip__"+led_strip_id+"__server.local")==0:
            print("...")
            time.sleep(1)

        print("Uploading firmware to led_strip__"+led_strip_id+"__server.local...")
        time.sleep(2)
        os.system("cd esp32_webserver && pio run -t uploadfs --upload-port led_strip__"+led_strip_id+"__server.local")

        success.append(led_strip_name+"' (webserver, "+led_strip_id+")")
        print("LED strip (webserver) '"+led_strip_name+"' ("+led_strip_id+") updated!")

        print("Waiting for LED strip (webserver) to be online again...")
        while hostname_resolves("led_strip__"+led_strip_id+"__server.local")==0:
            print("...")
            time.sleep(1)

        #TODO update config from all devices (new customizations is now "7", not "5"!)

        print("Activating OTA for led_strip__"+led_strip_id+"__leds.local...")
        os.environ['NO_PROXY'] = "led_strip__"+led_strip_id+"__server.local"
        response = requests.get("http://led_strip__"+led_strip_id+"__server.local/activate_ota")
        if response==200:
            
            print("LED strip (LEDs) '"+led_strip_name+"' ("+led_strip_id+") online. Start OTA update...")
            # if yes:
            # copy json file to /data/stripe_config.json
            copyfile("./stripe_configs/"+filename, "./esp32_leds/data/stripe_config.json")

            print("Uploading code to led_strip__"+led_strip_id+"__server.local...")
            time.sleep(2)
            os.system("cd esp32_leds && pio run -t upload --upload-port led_strip__"+led_strip_id+"__leds.local")

            print("Waiting for LED strip (webserver) to be online again...")
            while hostname_resolves("led_strip__"+led_strip_id+"__server.local")==0:
                print("...")
                time.sleep(1)

            print("Uploading firmware to led_strip__"+led_strip_id+"__leds.local...")
            time.sleep(2)
            os.system("cd esp32_leds && pio run -t uploadfs --upload-port led_strip__"+led_strip_id+"__leds.local")

            success.append(led_strip_name+"' (LEDs, "+led_strip_id+")")
            print("LED strip (LEDs) '"+led_strip_name+"' ("+led_strip_id+") updated!")
        
        else:
            print(response)


    else:
        offline.append(led_strip_name+"' ("+led_strip_id+")")
        print("LED strip '"+led_strip_name+"' ("+led_strip_id+") offline. Skip.")

    
    
        

# copy backup file back to /data/stripe_config.json
copyfile("./esp32_webserver/data/stripe_config_backup.json", "./esp32_webserver/data/stripe_config.json")
copyfile("./esp32_leds/data/stripe_config_backup.json", "./esp32_leds/data/stripe_config.json")

# remove backup file
os.remove("./esp32_webserver/data/stripe_config_backup.json")
os.remove("./esp32_leds/data/stripe_config_backup.json")

# Summarize how many LED strips have been updated and how many have been offline
print("Updates complete!")
print("=================")
print("")
print("Updated:")
for name in success:
    print(name)
print("")
print("Offline (unable to update):")
for name in offline:
    print(name)
print("")

