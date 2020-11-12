let ConnectionCheck = class {
    constructor() {
        this.disconnect_context=null;
        this.connect_check_interval
        this.online_check_interval
    }

    check_if_host_still_connected(){
        // check if host is accessible, if not, hide LED strips, interface and show "Disconnected from LED strip" and start check_if_host_online_again
        this.connect_check_interval = setInterval(function () {
            // if host not online anymore, stop connect_check_interval and start setInterval for check_if_host_online_again
            axios
                .get('http://theglowingstripes.local')
                .catch(function(error){
                    clearInterval(connection_check.connect_check_interval)
                    connection_check.show_disconnected_warning()
                    connection_check.check_if_host_connected_again()
                })
        },1000)
    }

    check_if_host_connected_again(){
        this.connect_check_interval = setInterval(function () {
            axios
                .get('http://theglowingstripes.local')
                .then(function(response){
                    clearInterval(connection_check.connect_check_interval)
                    connection_check.show_connected_again_info()
                    connection_check.check_if_host_still_connected()
                })
                .catch(function(error){
                    console.log('Host still disconnected...')
                })
        },1000)
    }

    check_if_host_still_online(){
        this.online_check_interval = setInterval(function () {
            axios
                .get('http://theglowingstripes.local/is_online')
                .catch(function(error){
                    clearInterval(connection_check.online_check_interval)
                    connection_check.show_host_is_offline()
                    connection_check.check_if_host_online_again()
                })
        },5000)
    }

    check_if_host_online_again(){
        this.online_check_interval = setInterval(function () {
            axios
                .get('http://theglowingstripes.local/is_online')
                .then(function(response){
                    clearInterval(connection_check.online_check_interval)
                    connection_check.show_host_is_online_again()
                    connection_check.check_if_host_still_online()
                }
            )
        },5000)
    }

    show_host_is_offline(){
        // change wifi icon from online to local
        document.getElementById('show_wifi_networks_button').classList.add('opacity_50')
        setTimeout(function(){
            document.getElementById('show_wifi_networks_button').className = 'icon large_inline wifi_local opacity_50'
            document.getElementById('show_wifi_networks_button').classList.remove('opacity_50')
        },200)

        // make software update icon partically transparent + change to "Offline" warning instead of making request to pi
        document.getElementById('software_update_button').style.opacity='0.2'
        document.getElementById('software_update_button').setAttribute('onClick','software_update.no_internet_connection()')
    }

    show_host_is_online_again(){
        // change software_update button and wifi icon back
        document.getElementById('show_wifi_networks_button').classList.add('opacity_50')
        setTimeout(function(){
            document.getElementById('show_wifi_networks_button').className = 'icon large_inline wifi_online opacity_50'
            document.getElementById('show_wifi_networks_button').classList.remove('opacity_50')
        },200)

        // make software update icon partically transparent + change to "Offline" warning instead of making request to pi
        document.getElementById('software_update_button').style.removeProperty('opacity')
        document.getElementById('software_update_button').setAttribute('onClick','software_update.update()')
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
connection_check.check_if_host_still_connected()
connection_check.check_if_host_still_online()