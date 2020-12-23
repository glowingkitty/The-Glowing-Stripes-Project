let DirectionCustomizer = class {
    get selected_options(){
        if ('start' in animation_customizer.updated_animation.customization && animation_customizer.updated_animation.customization.start){
            return animation_customizer.updated_animation.customization.start;
        } else {
            return null;
        }
    }

    get default_options(){
        if ('start' in animation_customizer.based_on_animation.customization && animation_customizer.based_on_animation.customization.start){
            return animation_customizer.based_on_animation.customization.start;
        } else {
            return null;
        }
    }

    get possible_directions(){
        if ('possible_directions' in animation_customizer.updated_animation.customization && animation_customizer.updated_animation.customization.possible_directions){
            var directions = animation_customizer.updated_animation.customization.possible_directions;
            var directions_output = [];
            if (directions.indexOf('start') >= 0){
                directions_output.push({
                    'name':'Bottom to top',
                    'value':'start'
                });
            }
            if (directions.indexOf('end') >= 0){
                directions_output.push({
                    'name':'Top to bottom',
                    'value':'end'
                });
            }
            if (directions.indexOf('start + end') >= 0){
                directions_output.push({
                    'name':'From both sides',
                    'value':'start + end'
                });
            }
            if (directions.indexOf('center') >= 0){
                directions_output.push({
                    'name':'From center',
                    'value':'center'
                });
            }
            return directions_output;
        } else {
            return null;
        }
        
    }

    get_direction_field(){
        this.field = new CustomizerField(
            'Direction',
            this.possible_directions,
            this.selected_options,
            this.default_options,
            'direction_customizer.change_direction_select(this.value)'
        );
        return this.field.get_select_field();
    }

    change_direction_select(new_selected){
        animation_customizer.updated_animation.customization.start = new_selected;

        // check if settings are different now in comparison to current animation, if yes, show "apply"/"save animation"/"update" buttons
        animation_customizer.check_for_changes();
        
        // TODO update preview animation
    }
};

var direction_customizer = new DirectionCustomizer();