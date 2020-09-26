var control

let Control = class {
    constructor() {}

    // Getter
    get main_window_content() {
        return document.getElementsByClassName('main_window_content')[0];
    }

    fadeout_main_window() {
        this.main_window_content.style.opacity = 0
    }

    fadein_main_window() {
        this.main_window_content.style.opacity = 1
    }

    clear_main_window() {
        this.fadeout_main_window()
        this.main_window_content.innerHTML = ''
    }

    load_buildup() {
        this.clear_main_window()

        this.main_window_content.innerHTML = `
            <span id="num_strips" class="num_of_connected_leds darkmode">0</span> LED <span id="text_strips">strips</span> connected.<br>
            <span id="ready_to_glow"></span>
            `

        this.fadein_main_window()
    }

    load_led_strips() {
        let led_strips = []

        // load json of connected_led_strips and create objects
        intr = setInterval(function () {
            console.log('Searching for LED strips...')

            axios.get("http://192.168.4.1/connected_led_strips.json")
                .then(function (response) {
                    let led_strips_json = response.data
                    var i;
                    for (i = 0; i < led_strips_json['LED strips'].length; i++) {

                        // check if LED strip already exists, only add if it doesn't exist yet
                        if (!document.getElementById(led_strips_json['LED strips'][i]['id'])) {
                            led_strips[i] = new LEDstrip(
                                led_strips_json['LED strips'][i]['id'],
                                led_strips_json['LED strips'][i]['name'],
                                led_strips_json['LED strips'][i]['mode'],
                                led_strips_json['LED strips'][i]['length']
                            );
                            led_strips[i].connect()
                        }

                    }

                    // show "ready to glow?" if at least one LED strip connected
                    if (num_of_led_strips > 0) {
                        document.getElementById('ready_to_glow').innerHTML = `
                            Ready to <span class="glow_text">glow</span>?<br>
                            <br>
                            <a id="buildup_complete_button" class="cta primary with_icon true" onclick="control.load_control()">
                                Buildup complete
                            </a>
                        `;
                    } else {
                        document.getElementById('ready_to_glow').innerHTML = ''
                    }

                })
                .catch(function (error) {
                    console.log(error);
                })
                .finally(function () {
                    // always executed
                });

        }, 1000)

    }

    load_control() {
        // break loop if "Buildup complete" button is pressed
        clearInterval(intr)

        this.clear_main_window()

        // show main control interface
    }
}

function boot() {
    control = new Control()
    // control.load_buildup()
    control.load_led_strips()
}