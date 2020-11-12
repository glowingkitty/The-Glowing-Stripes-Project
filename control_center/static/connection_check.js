let ConnectionCheck = class {
    constructor() {
        this.disconnect_context=null;
        this.connect_check_interval
        this.online_check_interval
    }

    check_if_host_connected_again(){
        this.connect_check_interval = setInterval(function () {
            axios
                .get('http://theglowingstripes.local')
                .then(function(response){
                    clearInterval(connection_check.connect_check_interval)
                    connection_check.show_connected_again_info()
                })
                .catch(function(error){
                    console.log('Host still disconnected...')
                })
        },1000)
    }


    show_disconnected_warning(){
        // fade out LED strip, hide LED strip control interface, hide all buttons and saved mixes show "Disconnected from Host" warning
        document.getElementsByClassName('leds_preview_block')[0].style.top='50px'
        document.getElementsByClassName('leds_preview_block')[0].style.opacity='0'

        document.getElementsByClassName('main_window_content')[0].style.height = '0px'
        document.getElementsByClassName('main_window_content')[0].style.opacity = '0'
        setTimeout(function(){
            document.getElementsByClassName('main_window_content')[0].style.display='none'
        },200)

        document.getElementsByClassName('header_overlay')[0].style.display = 'none'

        document.getElementsByClassName('mixes')[0].style.display = 'none'

        document.getElementById('software_update_button').style.display = 'none'
        document.getElementById('power_button').style.display = 'none'
        document.getElementById('show_wifi_networks_button').style.display = 'none'

        document.getElementById('disconnected_info').innerHTML = 'Disconnected from "The Glowing Stripes Project"<br>In case this is not supposed to happen: check the battery status of the LED strips and if you are connected to the correct Wi-Fi.'
    }

    show_connected_again_info(){
        // Fade in LED strip, show LED strip control interface
        document.getElementsByClassName('leds_preview_block')[0].style.removeProperty('top')
        document.getElementsByClassName('leds_preview_block')[0].style.removeProperty('opacity')

        document.getElementsByClassName('main_window_content')[0].style.removeProperty('display')
        setTimeout(function(){
            document.getElementsByClassName('main_window_content')[0].style.removeProperty('height')
            document.getElementsByClassName('main_window_content')[0].style.removeProperty('opacity')
        },200)

        document.getElementsByClassName('header_overlay')[0].style.removeProperty('display')

        document.getElementsByClassName('mixes')[0].style.removeProperty('display')

        document.getElementById('software_update_button').style.removeProperty('display')
        document.getElementById('power_button').style.removeProperty('display')
        document.getElementById('show_wifi_networks_button').style.removeProperty('display')

        document.getElementById('disconnected_info').innerHTML = ''
    }
}

var connection_check = new ConnectionCheck()