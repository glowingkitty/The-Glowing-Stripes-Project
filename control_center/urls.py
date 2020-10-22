"""control_center URL Configuration

The `urlpatterns` list routes URLs to views. For more information please see:
    https://docs.djangoproject.com/en/3.1/topics/http/urls/
Examples:
Function views
    1. Add an import:  from my_app import views
    2. Add a URL to urlpatterns:  path('', views.home, name='home')
Class-based views
    1. Add an import:  from other_app.views import Home
    2. Add a URL to urlpatterns:  path('', Home.as_view(), name='home')
Including another URLconf
    1. Import the include() function: from django.urls import include, path
    2. Add a URL to urlpatterns:  path('blog/', include('blog.urls'))
"""
from django.urls import path

from . import views

urlpatterns = [
    path('', views.Host.index, name='index'),
    path('signup', views.Host.signup_led_strip, name='signup_led_strip'),
    path('mode', views.Host.mode, name='mode'),

    # access json files
    path('current_mix', views.Settings.current_mix, name='current_mix'),
    path('led_animations', views.Settings.led_animations, name='led_animations'),
    path('saved_mixes', views.Settings.saved_mixes, name='saved_mixes'),
]
