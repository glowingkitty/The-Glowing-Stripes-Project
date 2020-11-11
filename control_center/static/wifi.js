let WiFi = class {
    constructor() {
        this.network;
        this.cta;
    }

    show_networks(){
        popup.header = '<span class="icon wifi_three_bars" style="background-size: 25px 25px !important; padding-left: 35px !important;"></span> Wi-Fi networks'
        popup.message = 'Loading...'
        popup.buttons = []
        popup.show()

        var self = this
        axios
            .get('http://theglowingstripes.local/wifi_networks_nearby')
            .then(function(response){
                popup.message = ''

                // if not connected to any wifi, show list of wifis with "Connect" buttons
                if (response.data['networks'][0]['current_wifi']==false || response.data['networks'][0]['is_default_wifi']==true){
                    var i;
                    for (i = 0; i < response.data['networks'].length; i++) {
                        self.network = response.data['networks'][i]
                        self.cta = 'connect'
                        popup.message += self.get_wifi_div()
                    }
                }
                // else show connected wifi at the top, "Disconnect" button and list other wifis WITH NO "Connect" buttons
                else {
                    self.network = response.data['networks'][0]
                    self.cta = 'disconnect'
                    popup.message += self.get_wifi_div()

                    popup.message+= '<div class="divider_line"></div>'

                    // show all remaining wifis
                    popup.message+= '<div style="overflow-y: scroll;max-height: 30vh;">'
                    
                    var i;
                    for (i = 1; i < response.data['networks'].length; i++) {
                        self.network = response.data['networks'][i]
                        self.cta = null
                        popup.message += self.get_wifi_div()
                    }

                    popup.message+= '</div>'
                }

                popup.header = '<span class="icon wifi_three_bars" style="background-size: 25px 25px !important; padding-left: 35px !important;"></span> Wi-Fi networks'
                popup.buttons = []
                popup.show()
            })
    }

    get_wifi_div(){
        // create the div block for a wifi network in the list
        var div = ''
        if (this.cta!=null){
            div += '<div class="wifi_network" id="'+this.network['essid']+'">'
        } else {
            div += '<div class="wifi_network opacity_50" id="'+this.network['essid']+'">'
        }

        
        // show if connected to wifi
        div += '<div class="inline_block">'
        if (this.network['current_wifi']==true){
            div += '<span class="icon true" style="background-size: 25px 25px !important; padding-left: 35px !important;"></span>'
        }else{
            div += '<span style="padding-left:35px"></span>'
        }
        div += this.network['essid']
        div += '</div>'
        
        div += '<div class="right_aligned inline_block">'
        // show encryption and signal strength
        if (this.network['encryption']!='off'){
            div += '<span class="icon lock"></span>'
        }
        if (this.network['signal_strength']>=50){
            div += '<span class="icon wifi_three_bars"></span>'
        } else if (this.network['signal_strength']>=30){
            div += '<span class="icon wifi_two_bars"></span>'
        } else{
            div += '<span class="icon wifi_one_bar"></span>'
        }
        
        // show CTAs
        if (this.cta=='disconnect'){
            div += '<a onclick="wifi.disconnect()" class="cta primary">Disconnect</a>'
        } else if (this.cta=='connect'){
            div += '<a onclick="wifi.connect(\''+this.network['essid']+'\')" class="cta primary">Connect</a>'
        }
        div += '</div>'
        
        div += '</div>'

        return div
    }

    connect(essid){
        
    }

    disconnect(){
        
    }

    show_reset_info(){

    }
}

var wifi = new WiFi()
