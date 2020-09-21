# This file is executed on every boot (including wake-boot from deepsleep)
import gc

import esp
from host import Host

# from stripe import Stripe

esp.osdebug(None)

gc.collect()

# Stripe().on()
Host().on()
