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
            // TODO
            // check if host online
            axios
                .get('http://theglowingstripes.local/is_online')
                .catch(function(error){
                    clearInterval(connection_check.online_check_interval)
                    // if offline: make software update icon partically transparent + change to "Offline" warning instead of making request to pi + change wifi icon

                    connection_check.check_if_host_online_again()
                })
        },5000)
    }

    check_if_host_online_again(){
        this.online_check_interval = setInterval(function () {
            // TODO
            // check if host online
            axios
                .get('http://theglowingstripes.local/is_online')
                .then(function(response){
                    clearInterval(connection_check.online_check_interval)
                    // change software_update button and wifi icon back

                    connection_check.check_if_host_still_online()
                }
            )
        },5000)
    }

    show_disconnected_warning(){
        // TODO
    }

    show_connected_again_info(){
        // TODO
    }
}

var connection_check = new ConnectionCheck()
connection_check.check_if_host_still_connected()