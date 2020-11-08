function sleep(milliseconds) {
    const date = Date.now();
    let currentDate = null;
    do {
        currentDate = Date.now();
    } while (currentDate - date < milliseconds);
}


let RainbowAnimation = class {
    constructor(led_strip,
        brightness = 1,
        loop_limit = null,
        duration_ms = 1000,
        pause_ms = null
    ) {
        this.led_strip = led_strip
        this.brightness_max = brightness
        this.loop_limit = loop_limit
        this.duration_ms = duration_ms
        this.pause_ms = pause_ms

        this.time_passed_ms = 0
    }

    set_time_passed_ms() {
        // if (duration 1000 ms = 17 loops * 60ms
        // if (duration 500 ms = 8.5 loops 120ms
        // if (duration 250 ms = 4.25 loops 240ms
        // if (duration 100 ms = 1.7 loops 600ms
        var full_duration = 1000
        var default_rate = 60

        var added_ms = (full_duration / this.duration_ms) * default_rate

        this.time_passed_ms += added_ms
    }

    set_brightness(counter, max_counter) {
        // if (counter == 1 => brightness 0.3
        // if (counter == 2 => brightness 0.6
        // if (counter == max_counter-1 => brightness 0.3
        // if (counter == max_counter-2 => brightness 0.6
        if (this.duration_ms && this.pause_ms && counter == 0) {
            this.brightness = 0.3 * this.brightness_max
        }
        else if (this.duration_ms && this.pause_ms && counter == 1) {
            this.brightness = 0.6 * this.brightness_max
        }
        else if (this.duration_ms && this.pause_ms && counter == (max_counter - 2)) {
            this.brightness = 0.6 * this.brightness_max
        }
        else if (this.duration_ms && this.pause_ms && counter == (max_counter - 1)) {
            this.brightness = 0.3 * this.brightness_max
        }
        else {
            this.brightness = 1 * this.brightness_max
        }
    }

    get_max_counter() {
        // if (duration 1000 ms = 17 loops * 60ms
        // if (duration 500 ms = 8.5 loops 120ms
        // if (duration 250 ms = 4.25 loops 240ms
        // if (duration 100 ms = 1.7 loops 600ms
        var full_duration = 1000
        var loops = 17

        return Math.round((this.duration_ms / full_duration) * loops)
    }

    glow() {

        console.log('Rainbow()')
        // if (duration, need to adapt time_passed to make one full color loop (&& then pause if (pause set)
        // turn LEDs rainbow
        var counter = 0
        var loops = 0
        var max_counter = this.get_max_counter()
        // while (true) {
        this.set_brightness(counter, max_counter)

        // turn LEDs black (off) for duration of pause
        if (counter == max_counter) {
            if (this.duration_ms && this.pause_ms) {
                this.led_strip.off()
                // sleep((this.pause_ms - 10) / 1000)
            }

            counter = 0
            loops += 1

            // if (this.loop_limit && this.loop_limit == loops) {
            //     break
            // }
        }

        else {

            this.set_time_passed_ms()
            var i
            for (i = 0; i < this.led_strip.num_of_leds; i++) {
                i = this.led_strip.get_led(i)
                var color = this.rainbow_color(this.time_passed_ms, i,
                    this.brightness)
                this.led_strip.write_led(i, color)
            }

            counter += 1
        }
        // }
    }

    rainbow_color(t, i, brightness) {
        console.log(t)
        console.log(i)
        console.log(brightness)
        var a = [0.5, 0.5, 0.5]
        var b = [0.5, 0.5, 0.5]
        var c = [1.0, 1.0, 1.0]
        var d = [0.00, 0.33, 0.67]

        var k = t + 0.05 * i

        var r = a[0] + b[0] * Math.cos(6.28318 * (c[0] * k + d[0]))
        var g = a[1] + b[1] * Math.cos(6.28318 * (c[1] * k + d[1]))
        b = a[2] + b[2] * Math.cos(6.28318 * (c[2] * k + d[2]))

        r = Math.round(255.0 * r * brightness)
        g = Math.round(255.0 * g * brightness)
        b = Math.round(255.0 * b * brightness)

        return [(r < 255) ? r : 255, (g < 255) ? g : 255, (b < 255) ? b : 255]
    }
}