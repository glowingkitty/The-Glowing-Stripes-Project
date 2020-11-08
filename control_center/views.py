import json
import os

import git
import requests
from django.http import HttpResponse, JsonResponse, request
from django.shortcuts import render
from django.template import loader
from django.views.decorators.csrf import csrf_exempt
from stripe import Stripe as LEDstripe

connected_led_strips = []
dirname = os.path.dirname(__file__)
led_strip = LEDstripe()


class Helper:
    def add_update_led_strip(json_data):
        print('add_update_led_strip...')
        if json_data in connected_led_strips:
            print('LED strip already connected')

        else:
            for entry in connected_led_strips:
                if entry['id'] == json_data['id']:
                    entry = json_data

            else:
                connected_led_strips.append(json_data)

        print('Connected LED strips: '+str(connected_led_strips))

    def get_strip_ip_address(id):
        for entry in connected_led_strips:
            if entry['id'] == id:
                return entry['ip_address']
        else:
            print('id not found')
            return None


class Settings:
    def web_control_config(request):
        with open(os.path.join(os.path.dirname(dirname), 'web_control_config.json')) as json_file:
            return JsonResponse(data=json.load(json_file))

    def connected_led_strips(request):
        return JsonResponse(data={'connected_led_strips': connected_led_strips})

    def led_animations(request):
        with open(os.path.join(os.path.dirname(dirname), 'led_animations.json')) as json_file:
            return JsonResponse(data=json.load(json_file))

    def saved_mixes(request):
        with open(os.path.join(os.path.dirname(dirname), 'saved_mixes.json')) as json_file:
            return JsonResponse(data=json.load(json_file))


class Host:

    def index(request):
        # /
        return render(request, 'index.html', {})

    @csrf_exempt
    def signup_led_strip(request):
        # /signup

        print('A new LED strip connected...')
        data = json.loads(request.body.decode('utf-8'))

        # see if LED strip is already saved in database, if yes, update IP, else: create new entry
        Helper.add_update_led_strip(data)

        print(data['name']+' connected (id: '+data['id']+')')
        return HttpResponse(status=200)

    @csrf_exempt
    def mode(request):
        # /mode
        if request.method == 'POST':
            print('Updating LED strips...')
            # process post request
            changes = json.loads(request.body.decode('utf-8'))['changes']
            for change in changes:
                for id in change['led_strip_ids']:
                    # replace IDs in request with current IPs & send requests
                    ip_address = Helper.get_strip_ip_address(id)
                    if ip_address:
                        # send new status to LED strips via /mode POST request
                        request = requests.post(
                            'http://'+ip_address+'/get_mode', json={
                                'new_animation': change['new_animation']
                            })
                        if request.status_code == 200:
                            print('Updated mode for {}'.format(id))

            return HttpResponse(status=200)

    @csrf_exempt
    def restore_all_led_strips(request):
        # make post request to all led strips to restore previous LED mode
        if request.method == 'POST':
            for stripe in connected_led_strips:
                request = requests.post(
                    'http://'+stripe['ip_address']+'/restore_previous_mode')
                if request.status_code == 200:
                    print('Restored previous mode for {}'.format(
                        stripe['id']))
            return HttpResponse(status=200)

    @csrf_exempt
    def shutdown_all_led_strips(request):
        # make post request to all led strips to shutdown
        if request.method == 'POST':
            # shutdown all led strips except host
            for other_stripe in connected_led_strips:
                if other_stripe['ip_address'] != led_strip.ip_address:
                    request = requests.post(
                        'http://'+led_strip['ip_address']+'/shutdown')

            request = requests.post('http://'+led_strip.ip_address+'/shutdown')

            return HttpResponse(status=200)

    @csrf_exempt
    def restart_all_led_strips(request):
        # make post request to all led strips to restart
        if request.method == 'POST':
            # restart all led strips except host
            for other_stripe in connected_led_strips:
                if other_stripe['ip_address'] != led_strip.ip_address:
                    request = requests.post(
                        'http://'+led_strip['ip_address']+'/restart')

            request = requests.post('http://'+led_strip.ip_address+'/restart')

            return HttpResponse(status=200)

    @csrf_exempt
    def update_all_led_strips(request):
        # make post request to all led strips to update
        if request.method == 'POST':
            # update all led strips except host
            full_response = {"led_strips": []}
            for other_stripe in connected_led_strips:
                if other_stripe['ip_address'] != led_strip.ip_address:
                    response = requests.post(
                        'http://'+led_strip['ip_address']+'/update').json()
                    # collect response messages and return to web interface
                    full_response['led_strips'].append({
                        "id": response['id'],
                        "message": response['message'],
                    })

            response = requests.post(
                'http://'+led_strip.ip_address+'/update').json()
            full_response['led_strips'].append({
                "id": response['id'],
                "message": response['message'],
            })

            return JsonResponse(data=full_response)


class Stripe:
    @csrf_exempt
    def get_mode(request):
        # POST request
        data = json.loads(request.body.decode('utf-8'))
        new_animation = data['new_animation']

        # save new animation to stripe config
        led_strip.update_last_animation(new_animation)

        led_strip.glow(
            id=new_animation['id'],
            based_on=new_animation['based_on'],
            customization=new_animation['customization'] if 'customization' in new_animation else None
        )
        return HttpResponse(status=200)

    @csrf_exempt
    def restore_previous_mode(request):
        # POST request
        led_strip.glow()
        return HttpResponse(status=200)

    @csrf_exempt
    def shutdown(request):
        # POST request
        # shutdown server and boot.py
        led_strip.off()
        os.system("shutdown now -h")
        return HttpResponse(status=200)

    @csrf_exempt
    def restart(request):
        # POST request
        # restart device
        led_strip.off()
        os.system("reboot now")
        return HttpResponse(status=200)

    @csrf_exempt
    def update(request):
        # POST request
        # update via git pull
        g = git.cmd.Git(dirname)
        message = g.pull()
        return JsonResponse(data={"id": led_strip.id, "message": message})
