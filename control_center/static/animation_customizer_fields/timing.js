let TimingCustomizer = class {
    get default_selected(){
        return animation_customizer.updated_animation['customization']['timing_selected']
    }

    get duration_ms(){
        return animation_customizer.updated_animation['customization']['duration_ms']
    }

    get pause_ms(){
        if ('pause_ms' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['pause_ms']){
            return animation_customizer.updated_animation['customization']['pause_ms']
        } else {
            return null
        }
    }

    get pause_a_ms(){
        if ('pause_a_ms' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['pause_a_ms']){
            return animation_customizer.updated_animation['customization']['pause_a_ms']
        } else {
            return null
        }
    }

    get pause_b_ms(){
        if ('pause_b_ms' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['pause_b_ms']){
            return animation_customizer.updated_animation['customization']['pause_b_ms']
        } else {
            return null
        }
    }

    get_timing_field(){
        // generate the field thats displayed under the select button
        this.update_subfield()

        this.field = new CustomizerField(
            'Timing',
            [
                {
                    'name':'Manual',
                    'value':'manual'
                },
                {
                    'name':'Tap to BPM',
                    'value':'bpm'
                },{
                    'name':'Microphone input',
                    'value':'microphone'
                }
            ],
            this.default_selected,
            'timing_customizer.change_timing_select(this.value)',
            this.subfield_html
        )
        return this.field.get_select_field()
    }

    update_subfield(){
        if (this.default_selected == 'bpm') {
            this.generate_bpm_timing_subfield()
        } else if (this.default_selected == 'microphone') {
            this.generate_microphone_timing_subfield()
        } else {
            this.generate_manual_timing_subfield()
        }
    }

    change_timing_select(new_selected){
        animation_customizer.updated_animation['customization']['timing_selected'] = new_selected
        this.update_subfield()
        document.getElementById('timing_subfield').innerHTML=this.subfield_html
    }

    generate_bpm_timing_subfield(){
        // info message on the left side, button on the right side
        this.message = 'Press the “Tap” button or “T” on your keyboard continously for 5 seconds in your disired timing. This will adapt the animation duration and pause.'
        this.subfield_html = '<div id="subfield_message">'+this.message+'</div><a class="cta primary with_icon tap" id="tap_to_bpm_button" onclick="timing_customizer.start_timing_tap_to_bpm()">Tap</a>'
    }

    generate_microphone_timing_subfield(){
        this.message = '“Microphone input” will use the microphone connected to the LED strip, NOT your computers microphone'
        this.subfield_html = this.message
    }

    generate_manual_timing_subfield(){
        // get duration and pause fields
        this.subfield_html = ''
        this.duration_ms_field = new CustomizerField(
            'Duration',
            '100ms-5000ms',
            this.duration_ms,
            'timing_customizer.change_duration(this.value)'
        )
        this.subfield_html += this.duration_ms_field.get_select_field()

        if (this.pause_ms!=null){
            this.pause_ms_field = new CustomizerField(
                'Pause',
                '100ms-5000ms',
                this.pause_ms,
                'timing_customizer.change_pause(this.value)'
            )
            this.subfield_html += this.pause_ms_field.get_select_field()
        }

        if (this.pause_a_ms!=null){
            this.pause_a_ms_field = new CustomizerField(
                'Pause A',
                '100ms-5000ms',
                this.pause_a_ms,
                'timing_customizer.change_pause(this.value,\'pause_a\')'
            )
            this.subfield_html += this.pause_a_ms_field.get_select_field()
        }

        if (this.pause_b_ms!=null){
            this.pause_b_ms_field = new CustomizerField(
                'Pause B',
                '100ms-5000ms',
                this.pause_a_ms,
                'timing_customizer.change_pause(this.value,\'pause_b\')'
            )
            this.subfield_html += this.pause_b_ms_field.get_select_field()
        }
        
    }

    change_duration(new_duration){
        this.duration_ms = new_duration

        // check if settings are different now in comparison to current animation, if yes, show "apply"/"save animation"/"update" buttons
        animation_customizer.check_for_changes()

        // TODO update preview animation
    }

    change_pause(new_pause,which_pause=null){
        if (which_pause=='pause_a'){
            animation_customizer.updated_animation['customization']['pause_a_ms'] = new_pause
        } else if (which_pause=='pause_b'){
            animation_customizer.updated_animation['customization']['pause_b_ms'] = new_pause
        } else {
            animation_customizer.updated_animation['customization']['pause_ms'] = new_pause
        }

        // TODO update preview animation
    }

    start_timing_tap_to_bpm(){
        // prevent triggering multiple times
        var tap_to_bpm_button = document.getElementById("tap_to_bpm_button");
        tap_to_bpm_button.onclick = ''

        var duration_ms_counter_list = []
        var duration_ms_counter = 0
        var duration_s_counter = 5
        var pause_ms_counter_list = []
        var pause_ms_counter = 0

        // count every 100ms
        var duration_ms_counter_interval = setInterval(function(){
            duration_ms_counter += 100;
        }, 100);

        // show update every second
        var duration_ms = this.duration_ms
        var pause_ms = this.pause_ms
        var pause_a_ms = this.pause_a_ms
        var pause_b_ms = this.pause_b_ms

        var duration_ms_counter_interval
        var pause_ms_counter_interval
        var tap_to_bpm_interval = setInterval(function(){
            duration_s_counter -= 1;
            if (duration_s_counter>0){
                // replace message text every second, to show remaining time
                document.getElementById("subfield_message").innerText = duration_s_counter+' second'+(duration_s_counter!=1 ? 's': '')+' remaining';
            } else {
                clearInterval(tap_to_bpm_interval);
                clearInterval(duration_ms_counter_interval);
                clearInterval(pause_ms_counter_interval);

                // calculate duration_ms
                var duration_ms_total = 0;
                for(var i = 0; i < duration_ms_counter_list.length; i++) {
                    duration_ms_total += duration_ms_counter_list[i];
                }
                duration_ms = duration_ms_total / duration_ms_counter_list.length
                animation_customizer.updated_animation['customization']['duration_ms'] = duration_ms

                // calculate pause_ms
                var pause_ms_total = 0;
                for(var i = 0; i < pause_ms_counter_list.length; i++) {
                    pause_ms_total += pause_ms_counter_list[i];
                }
                pause_ms = pause_ms_total / pause_ms_counter_list.length
                animation_customizer.updated_animation['customization']['pause_ms'] = pause_ms

                // if pause_a_ms and pause_b_ms, also set them to pause_ms
                if (pause_a_ms!=null && pause_b_ms!=null){
                    pause_a_ms = pause_ms
                    pause_b_ms = pause_ms
                    animation_customizer.updated_animation['customization']['pause_a_ms'] = pause_a_ms
                    animation_customizer.updated_animation['customization']['pause_b_ms'] = pause_b_ms
                }

                // show duration_ms and pause_ms and edit button
                var subfield_html = '<div>Duration '+duration_ms+'ms</div>'
                subfield_html += '<div>Pause '+pause_ms+'ms</div>'
                subfield_html += '<div class="cta secondary with_icon edit" onclick="timing_customizer.change_timing_select(\'bpm\')">Edit</div>'
                document.getElementById('timing_subfield').innerHTML = subfield_html
            }
            
        }, 1000);


        // count pause when not pressed
        var mouseup_hander = function() {
            // stop counting and add duration to list
            duration_ms_counter_list.push(duration_ms_counter)
            clearInterval(duration_ms_counter_interval);
            duration_ms_counter = 0

            pause_ms_counter_interval = setInterval(function(){
                pause_ms_counter += 100;
            }, 100);
        }
        var mousedown_hander = function() {
            // stop counting and add pause to list
            pause_ms_counter_list.push(pause_ms_counter)
            clearInterval(pause_ms_counter_interval);
            pause_ms_counter = 0

            duration_ms_counter_interval = setInterval(function(){
                duration_ms_counter += 100;
            }, 100);
        }

        // make keyboard press also trigger
        tap_to_bpm_button.addEventListener("mouseup", mouseup_hander,false);
        tap_to_bpm_button.addEventListener("keyup", event => {
            if (event.key = 't') {
                mouseup_hander()
            }
          });
        tap_to_bpm_button.addEventListener("mousdown", mousedown_hander,false);
        tap_to_bpm_button.addEventListener("keydown", event => {
            if (event.key = 't') {
                mousedown_hander()
            }
          });
    }
}

timing_customizer = new TimingCustomizer()