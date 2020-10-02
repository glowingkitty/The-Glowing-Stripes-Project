import urequests
from hardware import ESP
from machine import Pin

from neopixel_plus import NeoPixel
from stripe_details import StripeDetails


class Stripe():
    def __init__(self,
                 led_strip_data_pin_num=4,
                 num_of_leds=60,
                 ):

        # setting up the LEDs
        self.led_strip_data_pin_num = led_strip_data_pin_num
        self.led_strip_data_pin = Pin(self.led_strip_data_pin_num, Pin.OUT)
        self.num_of_leds = num_of_leds
        self.leds = NeoPixel(pin_num=self.led_strip_data_pin_num,
                             n=self.num_of_leds, start_point=0, brightness=1, bpp=3)

        self.esp = ESP()
        self.details = StripeDetails()

    def start_server(self):
        print('Starting strip server...')
        from MicroWebSrv2 import MicroWebSrv2

        # Define web routes
        @WebRoute(POST, '/change', name='Change LED strip mode')
        def PostNewLEDmode(microWebSrv2, request):
            print('Updating LED strip mode...')
            data = request.GetPostedJSONObject()

            if data['new_mode']['name'] == 'install_updates':
                request.Response.ReturnOk()
                # TODO

            else:
                # TODO
                print('Process LED mode...')

            request.Response.ReturnOk()

        mws2 = MicroWebSrv2()
        mws2.SetEmbeddedConfig()

        mws2._slotsCount = 4
        mws2._allowAllOrigins = True
        mws2.StartManaged()

        # Tell host that LED strip is online and ready for requests via POST request to /signup
        print('Sign up to host...')
        led_strip_data = {
            "id": self.details.id,
            "name": self.details.name,
            "ip_address": self.esp.networking.ip
        }
        print(led_strip_data)
        response = urequests.post(
            'http://192.168.4.1/signup', headers={'content-type': 'application/json'}, data=led_strip_data)
        response.close()

        # Main program loop until keyboard interrupt,
        try:
            while True:
                # TODO start glowing
                sleep(1)
        except KeyboardInterrupt:
            mws2.Stop()

    def on(self):
        self.esp.device_info()
        self.esp.networking.connect_to_host()

        self.start_server()
