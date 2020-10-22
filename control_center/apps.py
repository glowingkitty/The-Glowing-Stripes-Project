import time

import requests
from django.apps import AppConfig
from django.db.models.signals import pre_save


class ControlCenterConfig(AppConfig):
    name = 'control_center'
