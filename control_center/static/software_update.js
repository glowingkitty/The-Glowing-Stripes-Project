let SoftwareUpdate = class {
    update(){
        // check if host is connected to internet
        axios
            .get('http://theglowingstripes.local/is_online').then(
                function(response){
                    // show popup
                    popup.header = '<span class="icon sync" style="background-size: 25px 25px !important;"></span> Updating software'
                    popup.message = 'Updating software. Please wait...'
                    popup.buttons = []
                    popup.show()

                    axios
                        .post('http://theglowingstripes.local/update_all_led_strips',{timeout:120})
                        .then(function (response) {this.restart_services()})
                        .catch(
                            function (error) {
                                console.log(error);
                                popup.header = 'An error occured'
                                popup.message = error
                                popup.buttons = []
                                popup.show()
                            }
                        )
                }
            ).catch(
                function (error) {
                    console.log(error);
                    this.no_internet_connection()
                }
        )
    }

    no_internet_connection(){
        popup.header = '<span class="icon error" style="background-size: 25px 25px !important;"></span> Software update failed'
        popup.message = 'The LED strip is not connected to the internet.<br>Please connect to a Wi-Fi with internet access - and try again.'
        popup.buttons = [
            {
                'style':'primary',
                'text':'Connect to Wi-Fi',
                'icon':'wifi_online',
                'onclick':'new Popup().wifi_options()'
            }
        ]
        popup.show()
    }

    restart_services(){
        popup.header = 'Restarting services'
        popup.message = 'Restarting software services. Please wait...'
        popup.buttons = []
        popup.show()

        axios
            .post('http://theglowingstripes.local/restart_services_all_led_strips',{timeout:120})
            .then(function (response) {
                console.log('Update complete')
                console.log(response.data)
                this.update_complete()
            }
            ).catch(
                function (error) {
                    console.log(error);
                    popup.header = 'An error occured'
                    popup.message = error
                    popup.buttons = []
                    popup.show()
                }
            )
    }

    update_complete(){
        popup.header = '<span class="icon success" style="background-size: 25px 25px !important;"></span> Update complete'
        popup.message = 'The LED strip software on all connected LED strips is updated.'
        popup.buttons = [
            {
                'style':'primary',
                'text':'Ok',
                'onclick':'new Popup().hide()'
            }
        ]
        popup.show()
    }
}


var software_update = new SoftwareUpdate()