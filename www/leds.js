var intr

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
        document.getElementById('num_strips').innerText = num_of_led_strips;

        if (num_of_led_strips == 1) {
            document.getElementById('text_strips').innerText = 'strip'
        } else {
            document.getElementById('text_strips').innerText = 'strips'
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