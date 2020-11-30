let SectionsCustomizer = class {
    get default_selected(){
        if ('sections' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['sections']){
            return animation_customizer.updated_animation['customization']['sections']
        } else {
            return null
        }
    }

    get_sections_field(){
        this.field = new CustomizerField(
            'Height',
            [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0],
            this.default_selected,
            'sections_customizer.change_section_select(this.checked)'
        )
        return this.field.get_checkbox_field()
    }

    change_section_select(){
        // TODO detect state change (checked/not checked) and save it for correct field
    }
}

sections_customizer = new SectionsCustomizer()