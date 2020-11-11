let WiFi = class {
    show_networks(){
        popup.header = '<span class="icon wifi_three_bars" style="background-size: 25px 25px !important;"></span> Wi-Fi networks'
        popup.message = 'Loading...'
        popup.show()

        // axios
        //     .get('')
        //     .then(function(response){
        //         popup.header = '<span class="icon wifi_three_bars" style="background-size: 25px 25px !important;"></span> Wi-Fi networks'
        //         popup.message = 'Loading...'
        //         popup.show()
        //     })
    }

    connect(essid){
        
    }

    disconnect(){
        
    }

    show_reset_info(){

    }
}

var wifi = new WiFi()
