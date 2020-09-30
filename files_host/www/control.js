var control

var current_mix = {}
var num_of_led_strips = 0
var led_strips = []
var led_animations = {}

let Control = class {
    constructor(sync_all) {
        this.sync_all = sync_all;
        this.multi_select = false;
        this.main_window_new_html = ''
    }

    // Getter
    get main_window_content() {
        return document.getElementsByClassName('main_window_content')[0];
    }

    fadeout_main_window() {
        this.main_window_content.style.opacity = 0
    }

    fadein_main_window() {
        this.main_window_new_html = ''
        this.main_window_content.style.opacity = 1
    }

    clear_main_window() {
        this.fadeout_main_window()
        this.main_window_content.innerHTML = ''
    }

    load_buildup() {
        this.clear_main_window()

        this.main_window_new_html = `
        <span id="num_strips" class="num_of_connected_leds darkmode">0</span> LED <span id="text_strips">strips</span> connected.<br>
        <span id="ready_to_glow"></span>
        `
        this.main_window_content.innerHTML = this.main_window_new_html

        this.fadein_main_window()
    }

    load_led_strips() {
        // load json of connected_led_strips and create objects
        intr = setInterval(function () {
            console.log('Searching for LED strips...')

            axios.get("http://192.168.4.1/current_mix.json")
                .then(function (response) {
                    current_mix = response.data
                    num_of_led_strips = current_mix['led_strips'].length
                    var i;
                    for (i = 0; i < num_of_led_strips; i++) {

                        // check if LED strip already exists, only add if it doesn't exist yet
                        if (!document.getElementById(current_mix['led_strips'][i]['id'])) {
                            led_strips[i] = new LEDstrip(
                                current_mix['led_strips'][i]['id'],
                                current_mix['led_strips'][i]['name'],
                                current_mix['led_strips'][i]['mode'],
                                current_mix['led_strips'][i]['length']
                            );
                            led_strips[i].connect()
                        }

                    }

                    // show "ready to glow?" if at least one LED strip connected
                    if (document.getElementById('ready_to_glow')) {
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
        var control_object = this
        axios.get("http://192.168.4.1/led_animations.json")
            .then(function (response) {
                led_animations = response.data

                control_object.main_window_new_html += '<div class="left_aligned_maincontent">'

                //// show "all x LED strips" or name of single LED strip, depending on current mix
                control_object.main_window_new_html += '<div>'
                if (current_mix['current_mix']) {
                    //// show name of mix
                    control_object.main_window_new_html += '<span class="icon mix">' + current_mix['current_mix']['name'] + '</span>'

                    //// show "edit name" -> edit mix name
                    control_object.main_window_new_html += '<span onclick="editname(\'mix\',\'' + current_mix['current_mix']['id'] + '\')" '
                    control_object.main_window_new_html += 'class="text_cta with_icon edit right_positioned">Edit name</span>'

                } else if (current_mix['sync_all']) {
                    control_object.main_window_new_html += 'All <span id="num_strips" class="num_of_connected_leds darkmode">'
                    control_object.main_window_new_html += num_of_led_strips + '</span> LED <span id="text_strips">'
                    if (num_of_led_strips == 1) {
                        control_object.main_window_new_html += 'strip'
                    } else {
                        control_object.main_window_new_html += 'strips'
                    }
                    control_object.main_window_new_html += '</span>'

                } else {


                    //// show "edit name" if single LED strip selected

                }
                control_object.main_window_new_html += '</div>'


                //// show last used mode for all LED strips or mode of last selected led strip (depending on current mix)
                control_object.main_window_new_html += '<select class="mode_selector">'

                //// show all custom modes
                control_object.main_window_new_html += '<optgroup label="Custom animations">'
                var num_of_custom_animations = led_animations['led_animations']['custom'].length
                var i;
                for (i = 0; i < num_of_custom_animations; i++) {
                    control_object.main_window_new_html += '<option value="' + led_animations['led_animations']['custom'][i]['id'] + '"'
                    // mark mode as selected if thats the case in "current mix"
                    if (current_mix['selected_led_animation'] == led_animations['led_animations']['custom'][i]['id']) {
                        control_object.main_window_new_html += ' selected'
                    }
                    control_object.main_window_new_html += '>'
                    control_object.main_window_new_html += led_animations['led_animations']['custom'][i]['name'] + '</option>'
                }
                control_object.main_window_new_html += '</optgroup>'

                //// show all default modes
                control_object.main_window_new_html += '<optgroup label="Default animations">'
                var num_of_default_animations = led_animations['led_animations']['default'].length
                var i;
                for (i = 0; i < num_of_default_animations; i++) {
                    control_object.main_window_new_html += '<option value="' + led_animations['led_animations']['default'][i]['id'] + '"'
                    // mark mode as selected if thats the case in "current mix"
                    if (current_mix['selected_led_animation'] == led_animations['led_animations']['default'][i]['id']) {
                        control_object.main_window_new_html += ' selected'
                    }
                    control_object.main_window_new_html += '>'
                    control_object.main_window_new_html += led_animations['led_animations']['default'][i]['name'] + '</option>'
                }
                control_object.main_window_new_html += '</optgroup>'

                control_object.main_window_new_html += '</select>'

                //// add "customize animation" button
                control_object.main_window_new_html += '<a class="button_customize_animation customize right_positioned"></a>'

                //// show "sync all" and "multi select" from current mix
                control_object.main_window_new_html += '<div>'

                control_object.main_window_new_html += '<label class="checkbox with_icon sync">Sync all'
                control_object.main_window_new_html += '<input id="sync_all" onchange="change_sync_all()" type="checkbox"'
                if (current_mix['sync_all']) {
                    control_object.main_window_new_html += ' checked="checked"'
                }
                control_object.main_window_new_html += '><span class="checkmark"></span></label>'

                control_object.main_window_new_html += '<label class="checkbox with_icon multi_select right_positioned">Multi select'
                control_object.main_window_new_html += '<input id="multi_select" onchange="change_multi_select()" type="checkbox"'
                if (current_mix['multi_select']) {
                    control_object.main_window_new_html += ' checked="checked"'
                }
                control_object.main_window_new_html += '><span class="checkmark"></span></label>'

                control_object.main_window_new_html += '</div>'


                control_object.main_window_new_html += '<div>'

                //// show "save mix" button
                control_object.main_window_new_html += '<div class="fixed_bottom_left"><a class="cta darkmode with_icon save">Save mix</a></div>'


                control_object.main_window_new_html += '</div>'

                control_object.main_window_content.innerHTML = control_object.main_window_new_html

                control_object.fadein_main_window()
            })
            .catch(function (error) {
                console.log(error);
            })
            .finally(function () {
                // always executed
            });



        // show mixes (if they exist)
    }
}

function boot() {
    control = new Control()
    control.load_buildup()
    control.load_led_strips()
}