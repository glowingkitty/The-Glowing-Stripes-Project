import json

from django.http import HttpResponse, JsonResponse, request
from django.shortcuts import render
from django.template import loader
from django.views.decorators.csrf import csrf_exempt

connected_led_strips = []


class Helper:
    def add_update_led_strip(json_data):
        print('add_update_led_strip...')
        for entry in connected_led_strips:
            if entry['id'] == json_data['id']:
                entry = json_data
        else:
            connected_led_strips.append(json_data)

        print('Connected LED strips: '+str(connected_led_strips))


class Settings:
    def current_mix(request):
        with open('current_mix.json') as json_file:
            return JsonResponse(data=json.load(json_file))

    def led_animations(request):
        with open('led_animations.json') as json_file:
            return JsonResponse(data=json.load(json_file))

    def saved_mixes(request):
        with open('saved_mixes.json') as json_file:
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
                # TODO replace IDs in request with current IPs & send requests
                print(change)

                # TODO send new status to LED strips via /mode POST request
            return HttpResponse(status=200)


class Stripe:
    def get_mode(request):
        return HttpResponse("Process a new LED mode")
