var control;
var connected_led_strips = {};
var web_control_config = {};
var num_of_led_strips = 0;
var led_strips = {};
var led_animations = {};
var selected_led_strip_id = null;

let Control = class {
    constructor(sync_all) {
        this.sync_all = sync_all;
        this.multi_select = false;
        this.main_window_new_html = '';
        this.i=0;
    }

    get main_window_content() {
        return document.getElementsByClassName('main_window_content')[0];
    }

    get first_led_strip() {
        return led_strips[0];
    }

    fadeout_main_window() {
        this.main_window_content.style.opacity = 0;
    }

    fadein_main_window() {
        this.main_window_new_html = '';
        this.main_window_content.style.opacity = 1;
    }

    clear_main_window() {
        this.fadeout_main_window();
        this.main_window_content.innerHTML = '';
    }

    load_buildup() {
        this.clear_main_window();

        this.main_window_new_html = `
        <span id="num_strips" class="num_of_connected_leds darkmode">0</span> LED <span id="text_strips">strips</span> connected.<br>
        <span id="ready_to_glow"></span>
        `;
        this.main_window_content.innerHTML = this.main_window_new_html;

        this.fadein_main_window();
    }

    load_led_strips() {
        // load json of connected_led_strips and create objects
        intr = setInterval(function () {
            console.log('Searching for LED strips...');

            axios
                .get("/connected_led_strips")
                .then(function (response) {
                    connected_led_strips = response.data.connected_led_strips;
                    num_of_led_strips = connected_led_strips.length;
                    
                    var counter;
                    for (counter = 0; counter < num_of_led_strips; counter++) {

                        // check if LED strip already exists, only add if it doesn't exist yet
                        if (!document.getElementById(connected_led_strips[counter].id)) {
                            led_strips[connected_led_strips[counter].id] = new LEDstrip(
                                connected_led_strips[counter].id,
                                connected_led_strips[counter].name,
                                connected_led_strips[counter].last_animation,
                                connected_led_strips[counter].num_of_leds
                            );
                            led_strips[connected_led_strips[counter].id].connect();
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
                            document.getElementById('ready_to_glow').innerHTML = '';
                        }
                    }

                })
                .catch(function (error) {
                    console.log(error);
                })
                .finally(function () {
                    // always executed
                });

        }, 1000);

    }

    load_control() {
        // break loop if "Buildup complete" button is pressed
        clearInterval(intr);

        this.clear_main_window();

        // switch back to previous LED mode
        axios
            .post("/restore_all_led_strips");

        // TODO switch back preview to previous LED mode
        num_of_led_strips = Object.keys(led_strips).length;
        selected_led_strip_id = led_strips[[Object.keys(led_strips)[0]]].id;
        
        for (this.i = 0; this.i < num_of_led_strips; this.i++) {
            led_strips[[Object.keys(led_strips)[this.i]]].rainbow_animation();

        }

        var first_led_strip_id = led_strips[[Object.keys(led_strips)[0]]].id;

        // show main control interface
        var control_object = this;
        axios
            .get("/led_animations")
            .then(function (response) {
                led_animations = response.data;

                axios
                    .get("/web_control_config")
                    .then(function (response) {

                        web_control_config = response.data;

                        control_object.main_window_new_html += '<div class="left_aligned_maincontent">';

                        //// show "all x LED strips" or name of single LED strip, depending on current mix
                        control_object.main_window_new_html += '<div>';
                        if ('name' in web_control_config.current_mix) {
                            //// show name of mix
                            control_object.main_window_new_html += '<span class="icon mix">' + web_control_config.current_mix.name + '</span>';

                            //// show "edit name" -> edit mix name
                            control_object.main_window_new_html += '<span id="edit_name_button" onclick="editname(\'mix\',\'' + web_control_config.current_mix.id + '\')" ';
                            control_object.main_window_new_html += 'class="text_cta with_icon edit right_positioned">Edit name</span>';

                        } else if (num_of_led_strips > 1 && web_control_config.sync_all == true) {
                            control_object.main_window_new_html += 'All <span id="num_strips" class="num_of_connected_leds darkmode">';
                            control_object.main_window_new_html += num_of_led_strips + '</span> LED <span id="text_strips"> strips</span>';

                        } else {
                            //// show name and  "edit name" if single LED strip selected
                            
                            control_object.main_window_new_html += '<spam class="led_strip_name">'+led_strips[first_led_strip_id].name+'</spam>';
                            control_object.main_window_new_html += '<span id="edit_name_button" onclick="editname(\'led_strip\',\'' + led_strips[first_led_strip_id].id + '\')" ';
                            control_object.main_window_new_html += 'class="text_cta with_icon edit right_positioned">Edit name</span>';

                        }
                        control_object.main_window_new_html += '</div>';


                        //// show last used mode for all LED strips or mode of last selected led strip (depending on current mix)
                        control_object.main_window_new_html += '<select id="mode_selector" onchange="led_strips.'+led_strips[first_led_strip_id].id+'.check_mode_changed()" data-current-led-strip-id="'+led_strips[first_led_strip_id].id+'" class="mode_selector">';

                        //// show all custom modes
                        control_object.main_window_new_html += '<optgroup label="Custom animations">';
                        
                        var counter;
                        for (counter = 0; counter < led_animations.led_animations.custom.length; counter++) {
                            control_object.main_window_new_html += '<option data-based-on="'+('based_on' in led_animations.led_animations.custom[counter] ? led_animations.led_animations.custom[counter].based_on:'');
                            // if "led_animations.led_animations.custom[counter].unsaved_customization", then show those unsaved customization and save button
                            if (led_animations.led_animations.custom[counter].unsaved_customization){
                                control_object.main_window_new_html += '" data-customization="'+JSON.stringify(led_animations.led_animations.custom[counter].unsaved_customization).replaceAll('"',"'");
                            }else{
                                control_object.main_window_new_html += '" data-customization="'+JSON.stringify(led_animations.led_animations.custom[counter].customization).replaceAll('"',"'");
                            }
                            // add data-default-customization
                            control_object.main_window_new_html += '" data-default-customization="'+JSON.stringify(led_animations.led_animations.custom[counter].customization).replaceAll('"',"'");
                            control_object.main_window_new_html += '" value="' + led_animations.led_animations.custom[counter].id + '"';

                            // mark mode as selected if thats the case in "current mix"
                            if (led_strips[first_led_strip_id].last_animation.id == led_animations.led_animations.custom[counter].id) {
                                control_object.main_window_new_html += ' selected';
                            }
                            control_object.main_window_new_html += '>';
                            control_object.main_window_new_html += led_animations.led_animations.custom[counter].name + '</option>';
                        }
                        control_object.main_window_new_html += '</optgroup>';

                        //// show all default modes
                        control_object.main_window_new_html += '<optgroup label="Default animations">';
                        
                        for (counter = 0; counter < led_animations.led_animations.default.length; counter++) {
                            // exclude Setup mode
                            if (led_animations.led_animations.default[counter].id != "0000000000") {
                                // if "led_animations.led_animations.default[counter].unsaved_customization", then show those unsaved customization and save button
                                if (led_animations.led_animations.default[counter].unsaved_customization){
                                    control_object.main_window_new_html += '<option data-customization="'+JSON.stringify(led_animations.led_animations.default[counter].unsaved_customization).replaceAll('"',"'");
                                    
                                    // also safe id of animation with unsaved changes to led strip
                                    led_strips[led_animations.led_animations.default[counter].unsaved_customization_for_led_strip_id].unsubmitted_mode_change = JSON.parse(JSON.stringify(led_animations.led_animations.default[counter]));
                                    led_strips[led_animations.led_animations.default[counter].unsaved_customization_for_led_strip_id].unsubmitted_mode_change.customization = JSON.parse(JSON.stringify(led_animations.led_animations.default[counter].unsaved_customization));
                                } else {
                                    control_object.main_window_new_html += '<option data-customization="'+('customization' in led_animations.led_animations.default[counter]? JSON.stringify(led_animations.led_animations.default[counter].customization).replaceAll('"',"'"):'{}');
                                }
                                // add data-default-customization
                                control_object.main_window_new_html += '" data-default-customization="'+('customization' in led_animations.led_animations.default[counter]? JSON.stringify(led_animations.led_animations.default[counter].customization).replaceAll('"',"'"):'{}');
                                control_object.main_window_new_html += '" value="' + led_animations.led_animations.default[counter].id + '"';

                                // mark mode as selected if thats the case in "current mix"
                                if (led_strips[first_led_strip_id].last_animation.id == led_animations.led_animations.default[counter].id) {
                                    control_object.main_window_new_html += ' selected';
                                }
                                control_object.main_window_new_html += '>';
                                control_object.main_window_new_html += led_animations.led_animations.default[counter].name + '</option>';
                            }
                        }
                        control_object.main_window_new_html += '</optgroup>';

                        control_object.main_window_new_html += '</select>';

                        //// add "customize animation" button
                        control_object.main_window_new_html += '<a id="open_animation_customizer_button" onclick="animation_customizer.open()" class="button_customize_animation customize right_positioned"></a>';

                        //// show "sync all" and "multi select" from current mix
                        if (num_of_led_strips > 1) {
                            control_object.main_window_new_html += '<div>';

                            control_object.main_window_new_html += '<label class="checkbox with_icon sync">Sync all';
                            control_object.main_window_new_html += '<input id="sync_all" onchange="change_sync_all()" type="checkbox"';
                            if (web_control_config.sync_all) {
                                control_object.main_window_new_html += ' checked="checked"';
                            }
                            control_object.main_window_new_html += '><span class="checkmark"></span></label>';

                            control_object.main_window_new_html += '<label class="checkbox with_icon multi_select right_positioned">Multi select';
                            control_object.main_window_new_html += '<input id="multi_select" onchange="change_multi_select()" type="checkbox"';
                            if (web_control_config.multi_select) {
                                control_object.main_window_new_html += ' checked="checked"';
                            }
                            control_object.main_window_new_html += '><span class="checkmark"></span></label>';

                            control_object.main_window_new_html += '</div>';
                        }


                        control_object.main_window_new_html += '<div>';
                        control_object.main_window_new_html += '<div class="fixed_bottom_left">';

                        //// show "save mix" button
                        if (num_of_led_strips > 1) {
                            control_object.main_window_new_html += '<a id="save_mix_button" class="cta darkmode with_icon save">Save mix</a>';
                        }

                        control_object.main_window_new_html += '</div>';

                        // show undo and apply button (hidden, only visible if change is made)
                        control_object.main_window_new_html += '<div class="fixed_bottom_right">';
                        control_object.main_window_new_html += '<a id="undo_changes_button" onclick="led_strips.'+first_led_strip_id+'.undo_changes()" class="cta darkmode with_icon undo display_none">Undo</a>';
                        control_object.main_window_new_html += '<a id="apply_changes_button" onclick="led_strips.'+first_led_strip_id+'.apply_changes()"class="cta primary with_icon true display_none">Apply</a>';
                        control_object.main_window_new_html += '</div>';

                        control_object.main_window_new_html += '</div>';

                        control_object.main_window_content.innerHTML = control_object.main_window_new_html;

                        control_object.fadein_main_window();
                    })
                    .catch(function (error) {
                        console.log(error);
                    });
            })
            .catch(function (error) {
                console.log(error);
            });



        // show mixes (if they exist)
    }
};

function boot() {
    control = new Control();
    control.load_buildup();
    control.load_led_strips();
}
