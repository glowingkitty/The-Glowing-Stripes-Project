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

# backup /data/stripe_config.json to /data/stripe_config.json
copyfile("./data/stripe_config.json", "./data/stripe_config_backup.json")

# for every json file in /stripe_configs:
success = []
offline = []
for filename in [x for x in os.listdir("./stripe_configs") if x.endswith(".json")]:

    # load json file and id
    with open("./stripe_configs/"+filename) as json_file:
        data = json.load(json_file)
        led_strip_id = data["0"]
        led_strip_name = data["1"]

    # check if domain is online (led_strip__<id>.local)
    if hostname_resolves("led_strip__"+led_strip_id+".local"):
        print("LED strip '"+led_strip_name+"' ("+led_strip_id+") online. Start OTA update...")
        # if yes:
        # copy json file to /data/stripe_config.json
        copyfile("./stripe_configs/"+filename, "./data/stripe_config.json")

        os.system("pio run -t uploadfs --upload-port led_strip__"+led_strip_id+".local")
        time.sleep(10)
        os.system("pio run -t upload --upload-port led_strip__"+led_strip_id+".local")
        success.append(led_strip_name+"' ("+led_strip_id+")")
        print("LED strip '"+led_strip_name+"' ("+led_strip_id+") updated!")
    else:
        offline.append(led_strip_name+"' ("+led_strip_id+")")
        print("LED strip '"+led_strip_name+"' ("+led_strip_id+") offline. Skip.")
        

# copy backup file back to /data/stripe_config.json
copyfile("./data/stripe_config_backup.json", "./data/stripe_config.json")

# remove backup file
os.remove("./data/stripe_config_backup.json")

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

