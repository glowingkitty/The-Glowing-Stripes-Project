let AnimationCustomizer = class {
    constructor() {
        this.animation_id = null;
        this.animation_custom = false;
        this.animation_fields = null;
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
                    'brightness'
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
        };
    }

    open(){
        // use details from select field/select options, not "last_animation" - to enable changing modes which aren't active at the moment
        // show customizing fields, based on animation type 
        this.animation_id = document.getElementById('mode_selector').selectedOptions[0].value;
        this.animation_name = document.getElementById('mode_selector').selectedOptions[0].text;
        this.animation_custom = document.getElementById('mode_selector').selectedOptions[0].getAttribute('data-based-on')?true:false;
        this.animation_fields= this.customizable_fields[this.animation_id].fields;
        this.animation = {
            'id':this.animation_id,
            'name':this.animation_name,
            'based_on':document.getElementById('mode_selector').selectedOptions[0].getAttribute('data-based-on'),
            'customization':JSON.parse(document.getElementById('mode_selector').selectedOptions[0].getAttribute('data-customization').replaceAll("'",'"'))
        };

        // save current animation to make it easy to detect changes
        this.original_animation = JSON.parse(JSON.stringify(this.animation));
        this.updated_animation = JSON.parse(JSON.stringify(this.animation));

        // update popup headline
        popup.header = 'Customize "'+this.animation_name+'"';
        popup.message = '';

        // add fields
        if (this.animation_fields.indexOf('colors') >= 0){
            popup.message += colors_customizer.get_colors_field(this.animation_id);
        }
        if (this.animation_fields.indexOf('timing') >= 0){
            popup.message += timing_customizer.get_timing_field();
        }
        if (this.animation_fields.indexOf('direction') >= 0){
            popup.message += direction_customizer.get_direction_field();
        }
        if (this.animation_fields.indexOf('brightness') >= 0){
            popup.message += brightness_customizer.get_brightness_field();
        }
        if (this.animation_fields.indexOf('height') >= 0){
            popup.message += height_customizer.get_height_field();
        }
        if (this.animation_fields.indexOf('sections') >= 0){
            popup.message += sections_customizer.get_sections_field();
        }
            

        // show "Save new", "Update" and "Apply" button, depending on if edited mode is a default or custom mode (default modes cannot be updated)
        popup.buttons = [
            {
                'style':'secondary',
                'id':'save_mode_button',
                'hide':true,
                'icon':'save',
                'text':'Save new',
                'onclick':'animation_customizer.open_save_field()'
            }];


        // if a custom mode is beeing updated, add update button
        if (this.animation_custom==true){
            popup.buttons.push({
                'style':'secondary',
                'id':'update_mode_button',
                'hide':true,
                'icon':'sync',
                'text':'Update',
                'onclick':'animation_customizer.update_mode()'
            });
        }

        popup.buttons.push(
            {
                'style':'primary',
                'id':'apply_changed_mode_button',
                'hide':true,
                'icon':'true',
                'text':'Apply',
                'onclick':'animation_customizer.apply()'
            }
        );

        // make popup background non transparent
        document.getElementById('popup').style.background = 'rgba(0,0,0,1)';

        // limit max width
        document.getElementById('popup_content').style.maxWidth ='400px';

        // get first led strip preview and place it in popup placeholder on the left
        document.getElementById('customized_led_animation_preview').classList.remove('display_none');
        document.getElementById('customized_led_animation_preview_stripes').innerHTML = document.getElementById(selected_led_strip_id+'_led_strip_parts').innerHTML;

        // show popup
        popup.show();
        
    }

    close(){
        // make popup background partial trasparent again
        document.getElementById('popup').style.removeProperty('background');

        document.getElementById('popup_content').style.removeProperty('max-width');

        document.getElementById('customized_led_animation_preview').classList.add('display_none');
        document.getElementById('customized_led_animation_preview_stripes').innerHTML='';
    }

    get has_changed(){
        // see if animation settings have been changed in comparison to original
        return JSON.stringify(this.original_animation.customization)!=JSON.stringify(this.updated_animation.customization);
    }

    check_for_changes(){
        // if changes exist, show "apply","update","save" button
        if (this.has_changed==true){
            // show buttons
            if (document.getElementById('save_mode_button')){
                document.getElementById('save_mode_button').classList.remove('display_none');
            }
            if (document.getElementById('update_mode_button')){
                document.getElementById('update_mode_button').classList.remove('display_none');
            }
            if (document.getElementById('apply_changed_mode_button')){
                document.getElementById('apply_changed_mode_button').classList.remove('display_none');
            }
        } else {
            // hide buttons
            if (document.getElementById('save_mode_button')){
                document.getElementById('save_mode_button').classList.add('display_none');
            }
            if (document.getElementById('update_mode_button')){
                document.getElementById('update_mode_button').classList.add('display_none');
            }
            if (document.getElementById('apply_changed_mode_button')){
                document.getElementById('apply_changed_mode_button').classList.add('display_none');
            }
        }
    }

    apply(){
        // update customization of led_strip.unsubmitted_mode_change and run led_strip.apply_changes
        led_strips[selected_led_strip_id].unsubmitted_mode_change = this.original_animation;
        led_strips[selected_led_strip_id].unsubmitted_mode_change.customization = this.updated_animation.customization;
        led_strips[selected_led_strip_id].apply_changes();
        
        // update original_animation
        this.original_animation.customization = this.updated_animation.customization;

        // hide interface
        popup.hide();
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
};


var animation_customizer = new AnimationCustomizer();

