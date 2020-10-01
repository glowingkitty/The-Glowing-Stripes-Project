# This file is executed on every boot (including wake-boot from deepsleep)
import os

import machine

if 'files_stripe' in os.listdir():
    for file_name in os.listdir('files_stripe'):
        print('Move '+file_name+' to root')
        os.rename('files_stripe/'+file_name, file_name)
    print('Delete files_stripe/')
    os.rmdir('files_stripe')

elif 'files_host' in os.listdir():
    for file_name in os.listdir('files_host'):
        print('Move '+file_name+' to root')
        os.rename('files_host/'+file_name, file_name)
    print('Delete files_host/')
    os.rmdir('files_host')

if 'files_all_devices' in os.listdir():
    for file_name in os.listdir('files_all_devices'):
        print('Move '+file_name+' to root')
        os.rename('files_all_devices/'+file_name, file_name)

    print('Delete files_all_devices/')
    os.rmdir('files_all_devices')

    print('Root: '+str(os.listdir()))

    print('Reset esp')
    machine.reset()

import gc

import esp

from host import Host

esp.osdebug(None)

gc.collect()


Host().on()
