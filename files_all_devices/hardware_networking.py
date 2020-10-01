import network
import webrepl


class Networking():

    def __init__(self,
                 host_wifi_name='🌟GlowingStripes',
                 host_wifi_password='letsglow',
                 webrepl_password='letsglow'
                 ):

        # Wlan
        self.host_wifi_name = host_wifi_name
        self.host_wifi_password = host_wifi_password
        self.webrepl_password = webrepl_password
        self.wlan = None
        self.ap = None

    @property
    def ip(self):
        print(self.wlan.ifconfig())
        return self.wlan.ifconfig()[0]

    def activate_wifi(self):
        print('Activate WiFi...')
        self.wlan = network.WLAN(network.STA_IF)
        self.wlan.active(True)

    def scan_wifis(self):
        print('Scan for nearby WiFi networks...')
        if not self.wlan:
            self.activate_wifi()

        return self.wlan.scan()

    def connect_to_host(self):
        wifi_networks = self.scan_wifis()

        print('Searching for host nearby...')
        for network_info in wifi_networks:
            if self.host_wifi_name in network_info[0]:
                print('Found existing host nearby. Connecting to host...')

                if not self.wlan.isconnected():
                    print('connecting to network...')
                    self.wlan.connect(self.host_wifi_name,
                                      self.host_wifi_password)
                    while not self.wlan.isconnected():
                        pass
                print('Connected to host:', self.wlan.ifconfig())

                break
        else:
            print('Host not found. Please turn on the host.')

    def start_host_wifi(self):
        print('Starting host wifi...')
        self.ap = network.WLAN(network.AP_IF)
        self.ap.active(True)
        self.ap.config(essid=self.host_wifi_name,
                       authmode=network.AUTH_WPA_WPA2_PSK,
                       password=self.host_wifi_password)
        print('Host wifi accessible now as "{}"'.format(self.host_wifi_name))

    def activate_webrepl(self):
        print('Activate webrepl...')
        if not self.ap:
            print('Host wifi not active yet. Start host wifi...')
            self.start_host_wifi()
        webrepl.start(password=self.webrepl_password)
