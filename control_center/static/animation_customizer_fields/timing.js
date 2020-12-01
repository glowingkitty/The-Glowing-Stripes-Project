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
        this.duration_ms_counter_list = []
        this.duration_ms_counter = 0
        this.duration_s_counter = 5
        if (this.pause_ms!=null){
            this.pause_ms_counter_list = []
            this.pause_ms_counter = 0
        }
        
        if (this.pause_a_ms!=null){
            this.pause_a_ms_counter_list = []
            this.pause_a_ms_counter = 0
        }

        if (this.pause_b_ms!=null){
            this.pause_b_ms_counter_list = []
            this.pause_b_ms_counter = 0
        }

        this.tap_to_bpm_button = document.getElementById("tap_to_bpm_button");

        // count every 100ms
        this.duration_ms_counter_interval = setInterval(function(){
            this.duration_ms_counter += 100;
        }, 100);

        // show update every second
        this.tap_to_bpm_interval = setInterval(function(){
            this.duration_s_counter -= 1;
            if (this.duration_s_counter>0){
                // replace message text every second, to show remaining time
                document.getElementById("subfield_message").innerText = this.duration_s_counter+' second'+(this.duration_s_counter!=1 ? 's': '')+' remaining';
            } else {
                clearInterval(this.tap_to_bpm_interval);
                clearInterval(this.duration_ms_counter_interval);
                clearInterval(this.pause_ms_counter_interval);

                // calculate duration_ms
                this.duration_ms_total = 0;
                for(var i = 0; i < this.duration_ms_counter_list.length; i++) {
                    this.duration_ms_total += this.duration_ms_counter_list[i];
                }
                animation_customizer.updated_animation['customization']['duration_ms'] = this.duration_ms_total / this.duration_ms_counter_list.length

                // calculate pause_ms
                this.pause_ms_total = 0;
                for(var i = 0; i < this.pause_ms_counter_list.length; i++) {
                    this.pause_ms_total += this.pause_ms_counter_list[i];
                }
                animation_customizer.updated_animation['customization']['pause_ms'] = this.pause_ms_total / this.pause_ms_counter_list.length

                // if pause_a_ms and pause_b_ms, also set them to pause_ms
                if (this.pause_a_ms!=null && this.pause_b_ms!=null){
                    animation_customizer.updated_animation['customization']['pause_a_ms'] = this.pause_ms
                    animation_customizer.updated_animation['customization']['pause_b_ms'] = this.pause_ms
                }

                // show duration_ms and pause_ms and edit button
                this.subfield_html = '<div>Duration '+this.duration_ms+'ms</div>'
                this.subfield_html += '<div>Pause '+this.pause_ms+'ms</div>'
                this.subfield_html += '<div class="cta secondary with_icon edit" onclick="timing_customizer.change_timing_select(\'bpm\')">Edit</div>'

            }
            
        }, 1000);


        // count pause when not pressed
        this.mouseup_hander = function() {
            // stop counting and add duration to list
            this.duration_ms_counter_list.push(this.duration_ms_counter)
            clearInterval(this.duration_ms_counter_interval);
            this.duration_ms_counter = 0

            this.pause_ms_counter_interval = setInterval(function(){
                this.pause_ms_counter_interval += 100;
            }, 100);
        }
        this.mousedown_hander = function() {
            // stop counting and add pause to list
            this.pause_ms_counter_list.push(this.pause_ms_counter)
            clearInterval(this.pause_ms_counter_interval);
            this.pause_ms_counter = 0

            this.duration_ms_counter_interval = setInterval(function(){
                this.duration_ms_counter += 100;
            }, 100);
        }

        // make keyboard press also trigger
        this.tap_to_bpm_button.addEventListener("mouseup", this.mouseup_hander,false);
        this.tap_to_bpm_button.addEventListener("keyup", event => {
            if (event.key = 't') {
                this.mouseup_hander()
            }
          });
        this.tap_to_bpm_button.addEventListener("mousdown", this.mousedown_hander,false);
        this.tap_to_bpm_button.addEventListener("keydown", event => {
            if (event.key = 't') {
                this.mousedown_hander()
            }
          });
    }
}

timing_customizer = new TimingCustomizer()