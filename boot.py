# This file is executed on every boot (including wake-boot from deepsleep)
import gc

import esp
from stripes import Stripes

esp.osdebug(None)

gc.collect()

Stripes().on()
