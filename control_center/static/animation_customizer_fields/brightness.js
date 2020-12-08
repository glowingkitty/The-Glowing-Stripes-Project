let BrightnessCustomizer = class {
    get default_selected(){
        if ('brightness' in animation_customizer.updated_animation.customization && animation_customizer.updated_animation.customization.brightness){
            return animation_customizer.updated_animation.customization.brightness;
        } else {
            return null;
        }
    }

    get_brightness_field(){
        this.field = new CustomizerField(
            'Brightness',
            [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0],
            this.default_selected,
            'brightness_customizer.change_brightness_select(this.value)'
        );
        return this.field.get_select_field();
    }

    change_brightness_select(new_selected){
        new_selected = parseFloat(new_selected);
        animation_customizer.updated_animation.customization.brightness = new_selected;

        // check if settings are different now in comparison to current animation, if yes, show "apply"/"save animation"/"update" buttons
        animation_customizer.check_for_changes();
        
        // TODO update preview animation
    }
};

var brightness_customizer = new BrightnessCustomizer();