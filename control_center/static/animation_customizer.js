let AnimationCustomizer = class {
    constructor() {
        this.animation_id = null
        this.animation_custom = false
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
        this.animation_custom = led_strips[selected_led_strip_id].last_animation['based_on']?true:false
        this.animation_fields= this.customizable_fields[this.animation_id]['fields']

        // save current animation to make it easy to detect changes
        this.original_animation = led_strips[selected_led_strip_id].last_animation
        this.updated_animation = led_strips[selected_led_strip_id].last_animation

        // update popup headline
        popup.header = 'Customize "'+this.animation_name+'"'
        popup.message = ''

        // add fields
        if ('colors' in this.animation_fields){
            popup.message += colors_customizer.get_colors_field(this.animation_id)
        }
        if ('timing' in this.animation_fields){
            popup.message += timing_customizer.get_timing_field()
        }
        if ('direction' in this.animation_fields){
            popup.message += direction_customizer.get_direction_field()
        }
        if ('brightness' in this.animation_fields){
            popup.message += brightness_customizer.get_brightness_field()
        }
        if ('sections' in this.animation_fields){
            popup.message += sections_customizer.get_sections_field()
        }
        if ('height' in this.animation_fields){
            popup.message += height_customizer.get_height_field()
        }
            

        // show "Save new", "Update" and "Apply" button, depending on if edited mode is a default or custom mode (default modes cannot be updated)
        popup.buttons = [
            {
                'style':'secondary',
                'id':'save_mode_button',
                'icon':'save',
                'text':'Save new',
                'onclick':'animation_customizer.open_save_field()'
            }]


        // if a custom mode is beeing updated, add update button
        if (this.animation_custom==true){
            popup.buttons.push({
                'style':'secondary',
                'id':'update_mode_button',
                'icon':'sync',
                'text':'Update',
                'onclick':'animation_customizer.update_mode()'
            })
        }

        popup.buttons.push(
            {
                'style':'primary',
                'id':'apply_changed_mode_button',
                'icon':'true',
                'text':'Apply',
                'onclick':'animation_customizer.apply()'
            }
        )

        // make popup background non transparent
        document.getElementById('popup').style.background = 'rgba(0,0,0,1)'

        // limit max width
        document.getElementById('popup_content').style.maxWidth ='400px'

        // get first led strip preview and place it in popup placeholder on the left
        document.getElementById('customized_led_animation_preview').classList.remove('display_none')
        document.getElementById('customized_led_animation_preview_stripes').innerHTML = document.getElementById(selected_led_strip_id+'_led_strip_parts').innerHTML

        // show popup
        popup.show()
        
    }

    close(){
        // make popup background partial trasparent again
        document.getElementById('popup').style.removeProperty('background')

        document.getElementById('popup_content').style.removeProperty('max-width')

        document.getElementById('customized_led_animation_preview').classList.add('display_none')
        document.getElementById('customized_led_animation_preview_stripes').innerHTML=''
    }

    apply(){
        // TODO
    }


    // save new mode
    open_save_field(){
        // TODO
    }

    save_mode(){
        // TODO
    }

    update_mode(){
        // TODO
    }

    back_to_edit(){
        // TODO
    }
}

animation_customizer = new AnimationCustomizer()