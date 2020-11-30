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
            'Sections',
            ['Section 4','Section 3','Section 2','Section 1'],
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