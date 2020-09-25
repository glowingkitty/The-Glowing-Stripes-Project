let LEDstrip = class {
    constructor(id, name, mode, num_of_parts) {
        this.id = id
        this.name = name
        this.mode = mode
        this.num_of_parts = num_of_parts
        this.html = '<div class="led_strip" id="' + this.id + '" name="' + this.name + '">'
    }
    // Getter
    get num_of_leds() {
        return this.num_of_parts * 15;
    }

    connect() {
        // create LED strip inside DIV block

        // generate LED strip parts
        let num_of_parts_counter = 0
        while (num_of_parts_counter < this.num_of_parts) {
            this.html += '<div class="led_strip_part">'

            let num_of_leds = 0
            while (num_of_leds < 15) {
                this.html += '<div class="led"></div>'
                num_of_leds++;
            }

            this.html += '</div>'
            num_of_parts_counter++;
        }

        this.html += '<div class="led_strip_base"></div>'
        this.html += '</div>'

        // attatch LED strip to div
        document.getElementsByClassName('leds_preview_block')[0].innerHTML = document.getElementsByClassName('leds_preview_block')[0].innerHTML + this.html

        // update number of LEDs
        document.getElementById('num_strips').innerText = parseInt(document.getElementById('num_strips').innerText) + 1;

        if (parseInt(document.getElementById('num_strips').innerText) == 1) {
            document.getElementById('text_strips').innerText = 'strip'
        } else {
            document.getElementById('text_strips').innerText = 'strips'
        }

        // show "ready to glow?" if at least one LED strip connected
        if (parseInt(document.getElementById('num_strips').innerText) > 0) {
            document.getElementById('ready_to_glow').innerHTML = 'Ready to <span class="glow_text">glow</span>?<br><br><a id="buildup_complete_button" class="cta primary with_icon true">Buildup complete</a>'
        }

    }

    disconnect(inside_div) {
        // remove LED strip from DIV block
    }

    make_shorter() {
        // remove LED strip part
    }

    make_longer() {
        // add LED strip part
    }

    move_up() {
        // move LED stip position up
    }

    move_down() {
        // move LED stip position down
    }

    move_right() {
        // move LED stip position right
    }

    move_left() {
        // move LED stip position left
    }

    glow_rainbow() {}
};


function show_connected_led_strips() {
    let led_strips = []

    // load json of connected_led_strips and create objects
    var intr = setInterval(function () {
        console.log('Searching for LED strips...')

        axios.get("http://192.168.4.1/connected_led_strips.json")
            .then(function (response) {
                let led_strips_json = response.data
                var i;
                for (i = 0; i < led_strips_json['LED strips'].length; i++) {

                    // check if LED strip already exists, only add if it doesn't exist yet
                    if (!document.getElementById(led_strips_json['LED strips'][i]['id'])) {
                        led_strips[i] = new LEDstrip(
                            id = led_strips_json['LED strips'][i]['id'],
                            name = led_strips_json['LED strips'][i]['name'],
                            mode = led_strips_json['LED strips'][i]['mode'],
                            num_of_parts = led_strips_json['LED strips'][i]['length']
                        );
                        led_strips[i].connect()
                    }

                }

                // break loop if "Buildup complete" button is pressed
                if (!document.getElementById('buildup_complete_button')) {
                    clearInterval(intr)
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

function boot() {
    show_connected_led_strips()
}