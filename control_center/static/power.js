let Power = class {
    show_options(){
        popup.header = 'Power options'
        popup.message = 'What do you want to do?'
        popup.buttons = [
            {
                'style':'secondary',
                'icon':'power',
                'text':'Shutdown',
                'onclick':'power.shutdown()'
            },
            {
                'style':'secondary',
                'icon':'restart',
                'text':'Restart',
                'onclick':'power.restart()'
            },
            {
                'style':'primary',
                'icon':'sleep',
                'text':'Sleep',
                'onclick':'power.sleep()'
            }
        ]
        popup.show()
    }

    shutdown(){
        popup.header = 'Shut Down'
        popup.message = 'Shutting down all connected LED strips. Please wait...'
        popup.buttons = []
        popup.show()
        axios
            .post("http://theglowingstripes.local/shutdown_all_led_strips")
            .then(function(respone){
                console.log('Shut Down successfull')
            })
            .catch(function(error){
                console.log(error)
                popup.header = 'An error occured'
                popup.message = error
                popup.buttons = []
                popup.show()
            })
        
        // TODO auto check in frontend if host accessible, if not, show popup "Disconnected"
    }

    restart(){
        popup.header = 'Restart'
        popup.message = 'Restarting all connected LED strips. Please wait...'
        popup.buttons = []
        popup.show()
        axios
            .post("http://theglowingstripes.local/restart_all_led_strips")
            .then(function(respone){
                console.log('Restart successfull')
            })
            .catch(function(error){
                console.log(error)
                popup.header = 'An error occured'
                popup.message = error
                popup.buttons = []
                popup.show()
            })
    }

    sleep(){
        // TODO
        console.log('Turn LED strips off, without turning Raspberry Pi off')
    }
}

var power = new Power()