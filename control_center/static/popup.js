let Popup = class {
    constructor(header='', message='', buttons=[]) {
        this.header = header;
        this.message = message;
        this.buttons = buttons;
        // buttons example = [{"style":"primary","text":"Continue","icon":"sync","onclick":"control.next_page()"}]
    }

    get popup_active() {
        if (document.getElementById('popup').classList.contains('display_none')){
            return false
        }
        else {
            return true
        }
    }

    show() {
        // if another popup is open: hide that popup, then continue with showing new popup
        if (this.popup_active){
            this.hide()
        }

        var self = this
        setTimeout(function(){
            // overwrite headline
            document.getElementById('popup_headline').innerHTML = self.header

            // overwrite message
            document.getElementById('popup_message').innerHTML = self.message
            
            // overwrite buttons
            var i;
            var buttons = ''
            for (i = 0; i < self.buttons.length; i++) {
                // get button values
                if ('style' in self.buttons[i] && self.buttons[i]['style']){
                    var style = self.buttons[i]['style']
                } else {
                    var style = 'primary'
                }
                if ('icon' in self.buttons[i] && self.buttons[i]['icon']){
                    var icon = self.buttons[i]['icon']
                } else {
                    var icon = null
                }
                var text = self.buttons[i]['text']
                var onclick = self.buttons[i]['onclick']

                // add button
                buttons+='<a class="cta '

                if (style=='primary'){
                    buttons+='primary'
                } else{
                    buttons+='darkmode'
                }

                if (icon){
                    buttons+=' with_icon '
                    buttons+=icon
                }

                buttons+='" onclick="'
                buttons+=onclick
                buttons+='">'

                buttons+=text
                buttons+='</a>'
            }
            document.getElementById('popup_buttons').innerHTML = buttons

            // make popup visible
            document.getElementById('popup').classList.remove('display_none')
            document.getElementById('popup').classList.remove('opacity_null')

        },200)
    }

    hide(){
        document.getElementById('popup').classList.add('opacity_null')
        setTimeout(function(){
            document.getElementById('popup').classList.add('display_none')
        },200)
    }

    updating_software(){
        this.header = '<span class="icon sync" style="background-size: 25px 25px !important;"></span> Updating software'
        this.message = 'Updating software. Please wait...'
        this.show()
    }

    no_internet_connection(){
        this.header = '<span class="icon error" style="background-size: 25px 25px !important;"></span> Software update failed'
        this.message = 'The LED strip is not connected to the internet.<br>Please connect to a Wi-Fi with internet access - and try again.'
        this.buttons = [
            {
                'style':'primary',
                'text':'Connect to Wi-Fi',
                'icon':'wifi_online',
                'onclick':'new Popup().wifi_options()'
            }
        ]
        this.show()
    }

    restartig_services(){
        this.header = 'Restarting services'
        this.message = 'Restarting software services. Please wait...'
        this.show()
    }

    update_complete(){
        this.header = '<span class="icon success" style="background-size: 25px 25px !important;"></span> Update complete'
        this.message = 'The LED strip software on all connected LED strips is updated.'
        this.buttons = [
            {
                'style':'primary',
                'text':'Ok',
                'onclick':'new Popup().hide()'
            }
        ]
        this.show()
    }

    power_options(){
        this.header = 'Power options'
        this.message = 'What do you want to do?'
        this.buttons = [
            {
                'style':'secondary',
                'icon':'power',
                'text':'Shutdown',
                'onclick':'new Popup().shutdown()'
            },
            {
                'style':'secondary',
                'icon':'restart',
                'text':'Restart',
                'onclick':'new Popup().restart()'
            },
            {
                'style':'primary',
                'icon':'sleep',
                'text':'Sleep',
                'onclick':'new Popup().sleep()'
            }
        ]
        this.show()
    }

    shutdown(){
        this.header = 'Shut Down'
        this.message = 'Shutting down all connected LED strips. Please wait...'
        this.show()
        axios
            .post("http://theglowingstripes.local/shutdown_all_led_strips")
        
        // TODO auto check in frontend if host accessible, if not, show popup "Disconnected"
    }

    restart(){
        this.header = 'Restart'
        this.message = 'Restarting all connected LED strips. Please wait...'
        this.show()
        axios
            .post("http://theglowingstripes.local/restart_all_led_strips")
        
    }

    sleep(){
        // TODO
        console.log('Turn LED strips off, without turning Raspberry Pi off')
        
    }

    wifi_options(){
        // TODO
    }
}