import json
import os
import socket
import time
from shutil import copyfile


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

        os.system("cd esp32_webserver && pio run -t uploadfs --upload-port led_strip__"+led_strip_id+"__server.local")
        time.sleep(10)
        os.system("cd esp32_webserver && pio run -t upload --upload-port led_strip__"+led_strip_id+"__server.local")
        success.append(led_strip_name+"' (webserver, "+led_strip_id+")")
        print("LED strip (webserver) '"+led_strip_name+"' ("+led_strip_id+") updated!")
    else:
        offline.append(led_strip_name+"' (webserver, "+led_strip_id+")")
        print("LED strip (webserver) '"+led_strip_name+"' ("+led_strip_id+") offline. Skip.")

    
    # check if domain is online (led_strip__<id>__leds.local)
    if hostname_resolves("led_strip__"+led_strip_id+"__leds.local"):
        print("LED strip (LEDs) '"+led_strip_name+"' ("+led_strip_id+") online. Start OTA update...")
        # if yes:
        # copy json file to /data/stripe_config.json
        copyfile("./stripe_configs/"+filename, "./esp32_leds/data/stripe_config.json")

        os.system("cd esp32_leds && pio run -t uploadfs --upload-port led_strip__"+led_strip_id+"__server.local")
        time.sleep(10)
        os.system("cd esp32_leds && pio run -t upload --upload-port led_strip__"+led_strip_id+"__server.local")
        success.append(led_strip_name+"' (LEDs, "+led_strip_id+")")
        print("LED strip (LEDs) '"+led_strip_name+"' ("+led_strip_id+") updated!")
    else:
        offline.append(led_strip_name+"' (LEDs, "+led_strip_id+")")
        print("LED strip (LEDs) '"+led_strip_name+"' ("+led_strip_id+") offline. Skip.")
        

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

