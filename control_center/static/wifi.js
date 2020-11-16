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
            .get('/wifi_networks_nearby')
            .then(function(response){
                popup.message = ''

                // if not connected to any wifi, show list of wifis with "Connect" buttons
                if (response.data['networks'][0]['current_wifi']==false || response.data['networks'][0]['is_default_wifi']==true){
                    popup.message+= '<div style="overflow-y: scroll;max-height: 30vh;">'
                    var i;
                    for (i = 0; i < response.data['networks'].length; i++) {
                        self.network = response.data['networks'][i]
                        self.cta = 'connect'
                        popup.message += self.get_wifi_div()
                    }
                    popup.message+= '</div>'
                }
                // else show connected wifi at the top, "Disconnect" button and list other wifis WITH NO "Connect" buttons
                else {
                    self.network = response.data['networks'][0]
                    self.cta = 'disconnect'
                    popup.message += self.get_wifi_div()

                    popup.message+= '<div id="other_wifis">'
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

                    popup.message+= '</div>'

                    // also add hidden default wifi network (which will be visible when user disconnects from wifi)
                    popup.message+= '<div id="default_wifi" style="display: none">'
                    self.network = response.data['default_wifi']
                    self.cta = null
                    popup.message += self.get_wifi_div()
                    popup.message+= '</div>'
                }

                popup.header = '<span class="icon wifi_three_bars" style="background-size: 25px 25px !important; padding-left: 35px !important;"></span> Wi-Fi networks'
                popup.buttons = []
                popup.show()
            })
            .catch(function(error){
                console.log(error)
                connection_check.show_disconnected_warning()
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
        div += '<div class="inline_block text_max_width_40_percent">'
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
            div += '<a data-button-type="wifi_disconnect_button" onclick="wifi.disconnect(\''+this.network['essid']+'\')" class="cta primary with_icon close">Disconnect</a>'
        } else if (this.cta=='connect'){
            if (this.network['encryption']=='off'){
                div += '<a data-button-type="wifi_connect_button" id="'+this.network['essid']+'_connect_button" onclick="wifi.connect(\''+this.network['essid']+'\',null,null)" class="cta primary with_icon true">Connect</a>'
            } else {
                div += '<a data-button-type="wifi_connect_button" id="'+this.network['essid']+'_connect_button" onclick="wifi.ask_for_password(\''+this.network['essid']+'\',\''+this.network['encryption']+'\')" class="cta primary with_icon true">Connect</a>'
            }
        }
        div += '</div>'
        
        div += '</div>'

        return div
    }

    connect(essid,password,encryption){
        if (essid!=null){
            popup.header = 'Connecting to...'
            popup.message = document.getElementById(essid).outerHTML
            popup.message += '<br>Please wait...'
            popup.message+= '<br><br>This was a mistake?<br>Press the “Reset Wi-Fi” button on the LED strip case.'
            popup.buttons = []
            popup.show()

            axios
                .post('/connect_to_wifi_all_led_strips',{
                    'essid':essid,
                    'password':password, // TODO yeah yeah, I know. Don't worry, password will be submitted only if you are on the "TheGlowingStripes" wifi. And if anyone has a better suggestion on how to safely submit the wifi password on the local wifi without ssl issues, let me know
                    'encryption':encryption
                })
                .catch(function(error){
                    console.log(error);
                    popup.header = 'Connected to new Wi-Fi'
                    popup.message = 'You can access the LED strips now by connecting your device to:<br>'
                    popup.message+= document.getElementById(essid).outerHTML
                    popup.message+= '<br><br>This was a mistake?<br>Press the “Reset Wi-Fi” button on the LED strip case.'
                    popup.buttons = []
                    popup.show()
                })
        }
    }

    ask_for_password(essid,encryption){
        if (essid!=null && encryption!=null){
            popup.header = 'Enter Wi-Fi password for...'

            // remove connect button
            document.getElementById(essid+'_connect_button').outerHTML = ''

            // show wifi details to 
            popup.message = document.getElementById(essid).outerHTML

            // show 
            popup.message+='<div>'
            popup.message+='<input oninput="wifi.show_connect_button()" class="input_field" id="wifipassword" type="password" placeholder="Enter the Wi-Fi password here">'

            popup.message+='<div class="right_aligned inline_block">'
            popup.message+='<a id="connect_to_wifi_button" style="display: none" onclick="wifi.connect(\''+essid+'\',document.getElementById(\'wifipassword\').value,\''+encryption+'\')" class="cta primary">Connect</a>'
            popup.message+='</div>'
            
            popup.message+='</div>'

            popup.buttons = []
            popup.show()

            setTimeout(function(){
                var wifipasswordfield = document.getElementById('wifipassword');
                wifipasswordfield.focus();
                wifipasswordfield.select();
            },200)
            

        }
    }

    show_connect_button(){
        if (document.getElementById('wifipassword').value.length > 0){
            document.getElementById('connect_to_wifi_button').style.removeProperty('display')
        } else {
            document.getElementById('connect_to_wifi_button').style.display='none'
        }
        
    }

    disconnect(essid){
        // show new popup and send disconnect request
        popup.header = '<span class="icon wifi_three_bars" style="background-size: 25px 25px !important; padding-left: 35px !important;"></span> Disconnecting from ...'
        // hide "Disconnect" button
        document.getElementById('popup_message').getElementsByClassName('cta primary')[0].outerHTML=''

        // replace "other wifis" div with "Please wait..."
        document.getElementById('other_wifis').outerHTML='Please wait...'
        
        popup.message = document.getElementById('popup_message').innerHTML
        popup.show()

        // send disconnect request
        // TODO testing
        axios
            .post('/disconnect_from_wifi_all_led_strips',{
                'essid':essid
            },{timeout:10})
            .catch(function(error){
                console.log(error);
                popup.header = 'Disconnected from Wi-Fi'
                popup.message = 'You can access the LED strips now by connecting your device to:<br>'
                popup.message+= document.getElementById('default_wifi').innerHTML
                popup.message+= '<br><br>This was a mistake?<br>Press the “Reset Wi-Fi” button on the LED strip case.'
                popup.buttons = []
                popup.show()
            })
    }

}

var wifi = new WiFi()
