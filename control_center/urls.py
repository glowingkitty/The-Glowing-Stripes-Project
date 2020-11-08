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
     path('restore_all_led_strips', views.Host.restore_all_led_strips,
          name='restore_all_led_strips'),
     path('shutdown_all_led_strips', views.Host.shutdown_all_led_strips,
          name='shutdown_all_led_strips'),
     path('restart_all_led_strips', views.Host.restart_all_led_strips,
          name='restart_all_led_strips'),
     path('update_all_led_strips', views.Host.update_all_led_strips,
         name='update_all_led_strips'),


     # stripe related urls
     path('get_mode', views.Stripe.get_mode, name='get_mode'),
     path('restore_previous_mode', views.Stripe.restore_previous_mode,
          name='restore_previous_mode'),
     path('shutdown', views.Stripe.shutdown,
          name='shutdown'),
     path('update', views.Stripe.update,
          name='update'),
     path('restart', views.Stripe.restart,
          name='restart'),

     # access json files
     path('web_control_config', views.Settings.web_control_config,
          name='web_control_config'),
     path('connected_led_strips', views.Settings.connected_led_strips,
          name='connected_led_strips'),
     path('led_animations', views.Settings.led_animations, name='led_animations'),
     path('saved_mixes', views.Settings.saved_mixes, name='saved_mixes'),
]
