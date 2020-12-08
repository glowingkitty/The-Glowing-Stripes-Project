let SectionsCustomizer = class {
    constructor() {
        this.i=0;
    }

    get sections(){
        this.sections_list = [];
        for (this.i=1;this.i<=led_strips[selected_led_strip_id].num_of_parts;this.i++){
            this.sections_list.push(`Section ${this.i}`);
        }
    }
    
    get default_selected(){
        if ('sections' in animation_customizer.updated_animation.customization && animation_customizer.updated_animation.customization.sections){
            // replace 'all' with real sections
            if (animation_customizer.updated_animation.customization.sections=='all'){
                return this.sections;
            } else {
                return animation_customizer.updated_animation.customization.sections;
            }
        } else {
            return null;
        }
    }

    get_sections_field(){
        this.field = new CustomizerField(
            'Sections',
            ['Section 4','Section 3','Section 2','Section 1'],
            this.default_selected,
            'sections_customizer.change_section_select(this)'
        );
        return this.field.get_checkbox_field();
    }

    change_section_select(checkbox){
        var section_name = checkbox.parentElement.innerText;
        var checked = checkbox.value=='on'? true : false;
        
        if (checked==false){
            // remove from list if now unchecked 
            if (animation_customizer.updated_animation.customization.sections!='all'){
                animation_customizer.updated_animation.customization.sections = this.sections;
            }
            animation_customizer.updated_animation.customization.sections = animation_customizer.updated_animation.customization.sections.filter(section => section!=section_name);

        } else {
            // else add to list, if list != 'all' and entry not already added
            if (animation_customizer.updated_animation.customization.sections!='all' && animation_customizer.updated_animation.customization.sections.indexOf(section_name)<0){
                animation_customizer.updated_animation.customization.sections.push(section_name);
            }

        }

        // check if settings are different now in comparison to current animation, if yes, show "apply"/"save animation"/"update" buttons
        animation_customizer.check_for_changes();
    }
};

var sections_customizer = new SectionsCustomizer();