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

    change_timing_select(){

    }

    generate_bpm_timing_subfield(){

    }

    generate_microphone_timing_subfield(){

    }

    generate_manual_timing_subfield(){
        // TODO optimize for both single pause field and multiple pause fields
        this.subfield_html = ''
        this.duration_ms_field = new CustomizerField(
            'Duration',
            '100ms-5000ms',
            this.duration_ms,
            'timing_customizer.change_duration(this.value)'
        )
        this.subfield_html += this.duration_ms_field.get_select_field()

        if (this.pause_ms){
            this.pause_ms_field = new CustomizerField(
                'Pause',
                '100ms-5000ms',
                this.pause_ms,
                'timing_customizer.change_pause(this.value)'
            )
            this.subfield_html += this.pause_ms_field.get_select_field()
        }

        if (this.pause_a_ms){
            this.pause_a_ms_field = new CustomizerField(
                'Pause A',
                '100ms-5000ms',
                this.pause_a_ms,
                'timing_customizer.change_pause(this.value,\'pause_a\')'
            )
            this.subfield_html += this.pause_a_ms_field.get_select_field()
        }

        if (this.pause_b_ms){
            this.pause_b_ms_field = new CustomizerField(
                'Pause B',
                '100ms-5000ms',
                this.pause_a_ms,
                'timing_customizer.change_pause(this.value,\'pause_b\')'
            )
            this.subfield_html += this.pause_b_ms_field.get_select_field()
        }
        
    }

    change_duration(){

    }

    change_pause(new_pause,which_pause=null){

    }

    start_timing_tap_to_bpm(){

    }
}

timing_customizer = new TimingCustomizer()