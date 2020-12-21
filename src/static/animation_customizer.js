var reset_mode_and_not_applied_yet = false;

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
        this.based_on_animation = {
            'id':this.animation_id,
            'name':this.animation_name,
            'based_on':document.getElementById('mode_selector').selectedOptions[0].getAttribute('data-based-on'),
            'customization':JSON.parse(document.getElementById('mode_selector').selectedOptions[0].getAttribute('data-default-customization').replaceAll("'",'"'))
        };

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
                'hide':((led_strips[selected_led_strip_id].unsubmitted_mode_change.id==this.animation_id) && (this.equals_default==false)||reset_mode_and_not_applied_yet==true)?false:true,
                'icon':'save',
                'text':'Save new',
                'onclick':'animation_customizer.open_save_field()'
            }
        ];

        // show reset animation button for animations which have unsaved changes
        // TODO see if animation has customized details
        if ((led_strips[selected_led_strip_id].unsubmitted_mode_change.id==this.animation_id) && (this.equals_default==false)){
            popup.buttons.push({
                'style':'secondary',
                'id':'reset_mode_button',
                'hide':false,
                'icon':'undo',
                'text':'Reset',
                'onclick':'animation_customizer.reset_mode()'
            });
        }

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
                'hide':reset_mode_and_not_applied_yet==true?false:true,
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

    get equals_default(){
        // see if animation settings are the same like the default setting for the animation
        return JSON.stringify(this.based_on_animation.customization)==JSON.stringify(this.original_animation.customization);
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
            if (!led_strips[selected_led_strip_id].unsubmitted_mode_change.id || led_strips[selected_led_strip_id].unsubmitted_mode_change.id!=this.animation_id){
                if (document.getElementById('save_mode_button')){
                    document.getElementById('save_mode_button').classList.add('display_none');
                }
                if (document.getElementById('update_mode_button')){
                    document.getElementById('update_mode_button').classList.add('display_none');
                }
            }
            if (document.getElementById('apply_changed_mode_button')){
                document.getElementById('apply_changed_mode_button').classList.add('display_none');
            }
        }
    }

    apply(changes_saved=false){
        reset_mode_and_not_applied_yet = false;
        // update customization of led_strip.unsubmitted_mode_change and run led_strip.apply_changes
        led_strips[selected_led_strip_id].unsubmitted_mode_change = JSON.parse(JSON.stringify(this.original_animation));
        led_strips[selected_led_strip_id].unsubmitted_mode_change.customization = JSON.parse(JSON.stringify(this.updated_animation.customization));
        led_strips[selected_led_strip_id].apply_changes();
        
        // update original_animation
        this.original_animation.customization = JSON.parse(JSON.stringify(this.updated_animation.customization));

        // hide interface
        popup.hide();

        // if changes unsaved, mark this in led strip, to show "Save" and "Reset" button correctly
        if (changes_saved==false){
            led_strips[selected_led_strip_id].unsubmitted_mode_change.id = this.animation_id;
        }
    }


    // save new mode
    open_save_field(){
        // TODO replace headline
        popup.header = 'Save customized mode';
        popup.message = '';

        // show name input field (onchange: check if name already exists, if true for custom: show "Update" and "Update & Apply" buttons, if true for default: hide "Save" and "Save & Apply")
        var animation_name_field = new CustomizerField(
            'Name',
            null,
            null,
            null,
            'brightness_customizer.change_brightness_select(this.value)',
            null,
            'Enter a new animation name'
        );
        return this.field.get_select_field();

        // show "Back" button (.open()), "Save" (.save_mode()) and "Save & Apply" (.save_mode();.apply())
    }

    save_mode(){
        // TODO get name from name input field & send "save_new_mode" request to server, to save mode across all connected led strips

        // show main customizer interface again via .open()
    }

    reset_mode(){
        document.getElementById('reset_mode_button').classList.add('display_none');

        var counter;
        for (counter = 0; counter < led_animations.led_animations.custom.length; counter++) {
            if (this.animation_id == led_animations.led_animations.custom[counter].id) {
                document.getElementById('mode_selector').selectedOptions[0].setAttribute('data-customization',JSON.stringify(led_animations.led_animations.custom[counter].customization));
                
                // update original_animation
                this.updated_animation = JSON.parse(JSON.stringify(led_animations.led_animations.custom[counter]));
                break;
            }
        }

        for (counter = 0; counter < led_animations.led_animations.default.length; counter++) {
            if (this.animation_id == led_animations.led_animations.default[counter].id) {
                document.getElementById('mode_selector').selectedOptions[0].setAttribute('data-customization',JSON.stringify(led_animations.led_animations.default[counter].customization));
                
                // update original_animation
                this.updated_animation = JSON.parse(JSON.stringify(led_animations.led_animations.default[counter]));
                break;
            }
        }

        this.check_for_changes();

        led_strips[selected_led_strip_id].unsubmitted_mode_change = JSON.parse(JSON.stringify(this.original_animation));
        led_strips[selected_led_strip_id].unsubmitted_mode_change.customization = JSON.parse(JSON.stringify(this.updated_animation.customization));
        
        // reset shown customizations to default values
        var select_fields = document.getElementsByName('customizer_select_fields');
        select_fields.forEach(field => 
            field.value = field.getAttribute('data-default')
            );

        var checkboxes = document.getElementsByName('customizable_checkbox_fields');
        checkboxes.forEach(box => 
            box.checked = field.getAttribute('data-default')=='true'?true:false
            );

        reset_mode_and_not_applied_yet=true;
    }

    update_mode(){
        // TODO
    }

    back_to_edit(){
        // TODO
    }
};


var animation_customizer = new AnimationCustomizer();

