let DirectionCustomizer = class {
    get default_selected(){
        if ('start' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['start']){
            return animation_customizer.updated_animation['customization']['start']
        } else {
            return null
        }
        
    }

    get possible_directions(){
        if ('possible_directions' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['possible_directions']){
            var directions = animation_customizer.updated_animation['customization']['possible_directions']
            var directions_output = []
            if (directions.indexOf('start') >= 0){
                directions_output.push({
                    'name':'Bottom to top',
                    'value':'start'
                })
            }
            if (directions.indexOf('end') >= 0){
                directions_output.push({
                    'name':'Top to bottom',
                    'value':'end'
                })
            }
            if (directions.indexOf('start + end') >= 0){
                directions_output.push({
                    'name':'From both sides',
                    'value':'start + end'
                })
            }
            if (directions.indexOf('center') >= 0){
                directions_output.push({
                    'name':'From center',
                    'value':'center'
                })
            }
            return directions_output
        } else {
            return null
        }
        
    }

    get_direction_field(){
        this.field = new CustomizerField(
            'Direction',
            this.possible_directions,
            this.default_selected,
            'direction_customizer.change_direction_select(this.value)'
        )
        return this.field.get_select_field()
    }

    change_direction_select(new_selected){
        animation_customizer.updated_animation['customization']['start'] = new_selected
        // TODO update preview animation
    }
}

direction_customizer = new DirectionCustomizer()