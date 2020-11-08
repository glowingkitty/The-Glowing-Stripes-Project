var control

var connected_led_strips = {}
var web_control_config = {}
var num_of_led_strips = 0
var led_strips = []
var led_animations = {}

let Control = class {
    constructor(sync_all) {
        this.sync_all = sync_all;
        this.multi_select = false;
        this.main_window_new_html = ''
    }

    get main_window_content() {
        return document.getElementsByClassName('main_window_content')[0];
    }

    get first_led_strip() {
        return led_strips[0]
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

    show_popup(message) {
        console.log(message)
    }

    update_software() {
        console.log('Control().update_software()')
        axios.post('http://theglowingstripes.local/update_all_led_strips')
            .then(function (response) {
                var message = ''

                var i;
                for (i = 0; i < response.data['led_strips'].length; i++) {
                    message += '<div>'
                    message += 'ID '
                    message += response.data['led_strips'][i]['id']
                    message += ' => '
                    message += response.data['led_strips'][i]['message']
                    message += '</div>'

                }
                this.show_popup(message)
            }
            )
    }

    load_led_strips() {
        // load json of connected_led_strips and create objects
        intr = setInterval(function () {
            console.log('Searching for LED strips...')

            axios.get("http://theglowingstripes.local/connected_led_strips")
                .then(function (response) {
                    connected_led_strips = response.data['connected_led_strips']
                    num_of_led_strips = connected_led_strips.length
                    var i;
                    for (i = 0; i < num_of_led_strips; i++) {

                        // check if LED strip already exists, only add if it doesn't exist yet
                        if (!document.getElementById(connected_led_strips[i]['id'])) {
                            led_strips[i] = new LEDstrip(
                                connected_led_strips[i]['id'],
                                connected_led_strips[i]['name'],
                                connected_led_strips[i]['last_animation'],
                                connected_led_strips[i]['num_of_leds']
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

        // switch back to previous LED mode
        axios.post("http://theglowingstripes.local/restore_all_led_strips")
        // TODO switch back preview to previous LED mode
        num_of_led_strips = led_strips.length
        var i;
        for (i = 0; i < num_of_led_strips; i++) {
            led_strips[i].rainbow_animation()

        }

        // show main control interface
        var control_object = this
        axios.get("http://theglowingstripes.local/led_animations")
            .then(function (response) {
                led_animations = response.data

                axios.get("http://theglowingstripes.local/web_control_config")
                    .then(function (response) {

                        web_control_config = response.data

                        control_object.main_window_new_html += '<div class="left_aligned_maincontent">'

                        //// show "all x LED strips" or name of single LED strip, depending on current mix
                        control_object.main_window_new_html += '<div>'
                        if ('name' in web_control_config['current_mix']) {
                            //// show name of mix
                            control_object.main_window_new_html += '<span class="icon mix">' + web_control_config['current_mix']['name'] + '</span>'

                            //// show "edit name" -> edit mix name
                            control_object.main_window_new_html += '<span onclick="editname(\'mix\',\'' + web_control_config['current_mix']['id'] + '\')" '
                            control_object.main_window_new_html += 'class="text_cta with_icon edit right_positioned">Edit name</span>'

                        } else if (led_strips.length > 1 && web_control_config['sync_all'] == true) {
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
                            if (led_strips[0]['last_animation']['id'] == led_animations['led_animations']['custom'][i]['id']) {
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
                            // exclude Setup mode
                            if (led_animations['led_animations']['default'][i]['id'] != "000000") {
                                control_object.main_window_new_html += '<option value="' + led_animations['led_animations']['default'][i]['id'] + '"'
                                // mark mode as selected if thats the case in "current mix"
                                if (led_strips[0]['last_animation']['id'] == led_animations['led_animations']['default'][i]['id']) {
                                    control_object.main_window_new_html += ' selected'
                                }
                                control_object.main_window_new_html += '>'
                                control_object.main_window_new_html += led_animations['led_animations']['default'][i]['name'] + '</option>'
                            }
                        }
                        control_object.main_window_new_html += '</optgroup>'

                        control_object.main_window_new_html += '</select>'

                        //// add "customize animation" button
                        control_object.main_window_new_html += '<a class="button_customize_animation customize right_positioned"></a>'

                        //// show "sync all" and "multi select" from current mix
                        if (led_strips.length > 1) {
                            control_object.main_window_new_html += '<div>'

                            control_object.main_window_new_html += '<label class="checkbox with_icon sync">Sync all'
                            control_object.main_window_new_html += '<input id="sync_all" onchange="change_sync_all()" type="checkbox"'
                            if (web_control_config['sync_all']) {
                                control_object.main_window_new_html += ' checked="checked"'
                            }
                            control_object.main_window_new_html += '><span class="checkmark"></span></label>'

                            control_object.main_window_new_html += '<label class="checkbox with_icon multi_select right_positioned">Multi select'
                            control_object.main_window_new_html += '<input id="multi_select" onchange="change_multi_select()" type="checkbox"'
                            if (web_control_config['multi_select']) {
                                control_object.main_window_new_html += ' checked="checked"'
                            }
                            control_object.main_window_new_html += '><span class="checkmark"></span></label>'

                            control_object.main_window_new_html += '</div>'
                        }


                        control_object.main_window_new_html += '<div>'

                        //// show "save mix" button
                        if (led_strips.length > 1) {
                            control_object.main_window_new_html += '<div class="fixed_bottom_left"><a class="cta darkmode with_icon save">Save mix</a></div>'
                        }

                        control_object.main_window_new_html += '</div>'

                        control_object.main_window_content.innerHTML = control_object.main_window_new_html

                        control_object.fadein_main_window()
                    })
                    .catch(function (error) {
                        console.log(error);
                    })
            })
            .catch(function (error) {
                console.log(error);
            })



        // show mixes (if they exist)
    }
}

function boot() {
    control = new Control()
    control.load_buildup()
    control.load_led_strips()
}