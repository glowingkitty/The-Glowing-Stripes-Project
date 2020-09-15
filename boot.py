# This file is executed on every boot (including wake-boot from deepsleep)
import gc
import sys
from os import listdir

import esp
import machine
import network
import uos
import webrepl
from stripes import Stripes

esp.osdebug(None)

gc.collect()

Stripes().on()
