# This file is executed on every boot (including wake-boot from deepsleep)
import gc

import esp

from host import Host

esp.osdebug(None)

gc.collect()


Host().on()
