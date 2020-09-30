var intr

// colors from figma design file
let possible_setup_colors = [
    [231, 191, 88], // brown
    [18, 37, 212], // dark blue
    [0, 229, 92], // green
    [255, 92, 0], // orange
    [250, 255, 0], // yellow
    [56, 207, 255], // bright blue
    [255, 0, 168], // pink
    [255, 0, 0], // red
]

let used_setup_colors = []

let LEDstrip = class {
    constructor(id,
        name,
        mode,
        num_of_parts,
        setup_color = null,
        brightness = 0.97,
        start_point = 0,
        animation_up_and_down = true,
        animation_direction = 'up',
        debug = false
    ) {
        this.id = id
        this.name = name
        this.mode = mode
        this.num_of_parts = num_of_parts
        this.html = '<div class="led_strip" id="' + this.id + '" name="' + this.name + '">'

        // to generate animation preview
        this.time_passed = 0
        this.brightness = brightness;
        this.start_point = start_point;
        this.animation_up_and_down = animation_up_and_down;
        this.animation_direction = animation_direction;
        // get setup_color
        if (setup_color == null) {
            setup_color = this.get_setup_color()
        }
        this.leds = Array(this.num_of_leds).fill(setup_color)

        this.debug = debug
    }
    // Getter
    get num_of_leds() {
        return this.num_of_parts * 15;
    }

    get_setup_color() {
        if (this.debug) {
            console.log('LEDstrip().get_setup_color()')
        }
        for (const color_num in possible_setup_colors) {
            console.log(possible_setup_colors[color_num])
            if (!used_setup_colors.includes(possible_setup_colors[color_num])) {
                used_setup_colors.push(possible_setup_colors[color_num])
                return possible_setup_colors[color_num]
            }
        }
        return [255, 255, 255]

    }

    connect() {
        if (this.debug) {
            console.log('LEDstrip().connect()')
        }
        // create LED strip inside DIV block

        // generate LED strip parts
        let num_of_parts_counter = 0
        let total_num_of_leds = 0
        let r, g, b
        while (num_of_parts_counter < this.num_of_parts) {
            this.html += '<div class="led_strip_part">'

            let num_of_leds = 0
            while (num_of_leds < 15) {
                r = this.leds[total_num_of_leds][0]
                g = this.leds[total_num_of_leds][1]
                b = this.leds[total_num_of_leds][2]
                this.html += '<div class="led" style="background: rgb(' + r + ',' + g + ',' + b + ');'
                this.html += 'box-shadow: 0 0px 35px 0 rgb(' + r + ',' + g + ',' + b + ');'
                this.html += '" id="' + this.id + '-' + total_num_of_leds + '"></div>'
                num_of_leds++;
                total_num_of_leds++;
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

    send_setup_color() {
        if (this.debug) {
            console.log('LEDstrip().send_setup_color()')
        }

        //TODO send new color to strip
    }

    disconnect(inside_div) {
        if (this.debug) {
            console.log('LEDstrip().disconnect()')
        }
        // remove LED strip from DIV block
    }

    make_shorter() {
        if (this.debug) {
            console.log('LEDstrip().make_shorter()')
        }
        // remove LED strip part
    }

    make_longer() {
        if (this.debug) {
            console.log('LEDstrip().make_longer()')
        }
        // add LED strip part
    }

    move_up() {
        if (this.debug) {
            console.log('LEDstrip().move_up()')
        }
        // move LED stip position up
    }

    move_down() {
        if (this.debug) {
            console.log('LEDstrip().move_down()')
        }
        // move LED stip position down
    }

    move_right() {
        if (this.debug) {
            console.log('LEDstrip().move_right()')
        }
        // move LED stip position right
    }

    move_left() {
        if (this.debug) {
            console.log('LEDstrip().move_left()')
        }
        // move LED stip position left
    }

    write(wait_after_wait = (1.0 / 36.0) * 1000) {
        if (this.debug) {
            console.log('LEDstrip().write()')
        }
        // color each led in div block correctly
        let selected_led, r_write, g_write, b_write

        // setTimeout(function () {
        for (const led_write_counter in [...Array(this.num_of_leds).keys()]) {
            if (this.debug) {
                console.log('... for (const led_write_counter in [...Array(this.num_of_leds).keys()]) {}...')
            }
            selected_led = document.getElementById(this.id + '-' + led_write_counter)
            if (selected_led) {
                if (this.debug) {
                    console.log('... selected_led found ...')
                }
                r_write = this.leds[led_write_counter][0]
                g_write = this.leds[led_write_counter][1]
                b_write = this.leds[led_write_counter][2]
                selected_led.style.background = 'rgb(' + r_write + ',' + g_write + ',' + b_write + ')'
                selected_led.style.boxShadow = '0 0px 35px 0 rgb(' + r_write + ',' + g_write + ',' + b_write + ')'
                if (this.debug) {
                    console.log('... selected_led style changed ...')
                }
            }
        }
        // }, wait_after_wait)
    }

    get_led(led_number) {
        if (this.debug) {
            console.log('LEDstrip().get_led()')
        }
        led_number = led_number + this.start_point
        if (led_number < 0) {
            led_number = this.num_of_leds + led_number
        }
        return led_number
    }


    off() {
        if (this.debug) {
            console.log('LEDstrip().off()')
        }
        for (const led_number_off_counter in [...Array(this.num_of_leds).keys()]) {
            this.leds[led_number_off_counter] = (0, 0, 0)
        }
        this.write()
    }

    on(num = null) {
        if (this.debug) {
            console.log('LEDstrip().on()')
        }
        if (isNaN(num)) {
            for (const led_number_on_counter in [...Array(this.num_of_leds).keys()]) {
                this.leds[led_number_on_counter] = (255, 255, 255)
            }
        } else {
            num = this.get_led(num)
            this.leds[num] = (255, 255, 255)
        }

        this.write()
    }

    rainbow_animation(limit = null) {
        if (this.debug) {
            console.log('LEDstrip().rainbow_animation()')
        }
        // turn LEDs rainbow
        let counter = 0
        let rainbow_color
        // while (true) {

        this.time_passed += 0.06
        for (var rainbow_led_counter in [...Array(this.num_of_leds).keys()]) {
            rainbow_led_counter = this.get_led(rainbow_led_counter)
            rainbow_color = this.rainbow_color(this.time_passed, rainbow_led_counter,
                this.brightness)
            this.leds[rainbow_led_counter] = rainbow_color
        }

        if (this.animation_up_and_down) {
            this.brightness, this.animation_direction = this.change_brightness(
                this.brightness, this.animation_direction)
        }

        this.write()

        counter += 1
        // if (limit && counter == limit) {
        //     break
        // }
        // }
    }

    rainbow_color(t, rainbow_color_num, brightness) {
        if (this.debug) {
            console.log('LEDstrip().rainbow_color()')
        }
        let a = (0.5, 0.5, 0.5)
        let b = (0.5, 0.5, 0.5)
        let c = (1.0, 1.0, 1.0)
        let d = (0.00, 0.33, 0.67)

        let k = t + 0.05 * rainbow_color_num

        let r = a[0] + b[0] * Math.cos(6.28318 * (c[0] * k + d[0]))
        let g = a[1] + b[1] * Math.cos(6.28318 * (c[1] * k + d[1]))
        b = a[2] + b[2] * Math.cos(6.28318 * (c[2] * k + d[2]))

        r = Math.round(255.0 * r * brightness)
        g = Math.round(255.0 * g * brightness)
        b = Math.round(255.0 * b * brightness)

        return ((r < 255) ? r : 255, (g < 255) ? g : 255, (b < 255) ? b : 255)
    }

    change_brightness(brightness, direction) {
        if (this.debug) {
            console.log('LEDstrip().change_brightness()')
        }
        if (direction == 'up') {
            brightness = brightness + 0.025
        } else {
            brightness = brightness - 0.025
        }

        if (brightness <= 0.1) {
            direction = 'up'
        } else if (brightness >= 0.7) {
            direction = 'down'
        }

        return brightness, direction
    }
};