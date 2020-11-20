let AnimationCustomizer = class {
    constructor() {
        this.animation_id = null
        this.animation_fields = null
        this.customizable_fields = {
            '9jwnqn8v3i':{
                'name':'Color',
                'fields':[
                    'colors',
                    'brightness',
                    'sections'
                ]
            },
            'b943uee3y7':{
                'name':'Rainbow',
                'fields':[
                    'timing',
                    'direction',
                    'brightness',
                    'sections'
                ]
            },
            '8hsylal9v7':{
                'name':'Beats',
                'fields':[
                    'colors',
                    'timing',
                    'direction',
                    'brightness',
                    'height'
                ]
            },
            'leta9ed5fc':{
                'name':'Moving dot',
                'fields':[
                    'colors',
                    'timing',
                    'direction',
                    'brightness'
                ]
            },
            'kack2555kd':{
                'name':'Light Up',
                'fields':[
                    'colors',
                    'timing',
                    'brightness',
                    'sections'
                ]
            },
            '7u9tjpd0gi':{
                'name':'Transition',
                'fields':[
                    'colors',
                    'timing',
                    'brightness',
                    'sections'
                ]
            }
        }
    }

    open(){
        // show customizing fields, based on animation type 
        this.animation_id = led_strips[selected_led_strip_id].last_animation['id']
        this.animation_name = led_strips[selected_led_strip_id].last_animation['name']
        this.animation_fields= this.customizable_fields[animation_id]['fields']

        // update popup headline
        popup.header = 'Customize "'+this.animation_name+'"'
        popup.message = ''

        // add fields
        if ('colors' in this.animation_fields){
            popup.message += this.get_colors_field()
        }
        if ('timing' in this.animation_fields){
            popup.message += this.get_timing_field()
        }
        if ('direction' in this.animation_fields){
            popup.message += this.get_direction_field()
        }
        if ('brightness' in this.animation_fields){
            popup.message += this.get_brightness_field()
        }
        if ('sections' in this.animation_fields){
            popup.message += this.get_sections_field()
        }
        if ('height' in this.animation_fields){
            popup.message += this.get_height_field()
        }
            

        // show "Save new", "Update" and "Apply" button, depending on if edited mode is a default or custom mode (default modes cannot be updated)

        // make popup background non transparent
        document.getElementById('popup').style.background = 'rgba(0,0,0,1)'

        // limit max width
        document.getElementById('popup_content').style.maxWidth ='400px'

        // get first led strip preview and place it in popup placeholder on the left
        document.getElementById('customized_led_animation_preview').classList.remove('display_none')
        document.getElementById('customized_led_animation_preview_stripes').innerHTML = document.getElementById(selected_led_strip_id).getElementsByClassName('led_strip_parts')[0].innerHTML
        

        // show popup

        
    }

    close(){
        // make popup background partial trasparent again
        document.getElementById('popup').style.removeProperty('background')

        document.getElementById('popup_content').style.removeProperty('max-width')

        document.getElementById('customized_led_animation_preview').classList.add('display_none')
        document.getElementById('customized_led_animation_preview_stripes').innerHTML=''
    }

    apply(){

    }


    // save new mode
    open_save_field(){

    }

    save_mode(){

    }

    back_to_edit(){

    }




    // change Timing
    get_timing_field(){

    }

    change_timing_select(){

    }

    show_timing_manual(){

    }

    show_timing_tap_to_bpm(){

    }

    show_timing_microphone_input(){

    }

    change_duration(){

    }

    change_pause(){

    }

    start_timing_tap_to_bpm(){

    }

    
    // change Direction
    get_direction_field(){

    }

    change_direction_select(){

    }

    // change Max Height
    get_height_field(){

    }

    change_height_select(){

    }


    // change Sections
    get_sections_field(){

    }

    change_section_select(){

    }
}

animation_customizer = new AnimationCustomizer()