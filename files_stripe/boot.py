# This file is executed on every boot (including wake-boot from deepsleep)
import gc

import esp
from stripe import Stripe

esp.osdebug(None)

gc.collect()

Stripe().on()
