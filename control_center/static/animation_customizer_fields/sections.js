let SectionsCustomizer = class {
    constructor() {
        this.i=0;
        this.subfield_html='';
    }

    get sections(){
        this.sections_list = [];
        for (this.i=1;this.i<=led_strips[selected_led_strip_id].num_of_parts;this.i++){
            this.sections_list.push(`Section ${this.i}`);
        }
        return this.sections_list;
    }
    
    get default_selected(){
        return animation_customizer.updated_animation.customization.sections_selected;
    }

    get selected_sections(){
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
        // generate the field thats displayed under the select button
        this.update_subfield();

        this.field = new CustomizerField(
            'Sections',
            [   
                {
                    'name':'All',
                    'value':'all'
                },
                {
                    'name':'Random',
                    'value':'random'
                },
                {
                    'name':'Manual',
                    'value':'manual'
                }
            ],
            this.default_selected,
            'sections_customizer.change_sections_select(this.value)',
            this.subfield_html
        );
        return this.field.get_select_field();
    }

    update_subfield(){
        if (this.default_selected == 'manual'){
            this.generate_manual_sections_subfield();
        } else {
            this.subfield_html = '';
        }
    }

    generate_manual_sections_subfield(){
        this.sub_field = new CustomizerField(
            'Sections',
            ['Section 4','Section 3','Section 2','Section 1'],
            this.selected_sections,
            'sections_customizer.change_section(this)'
        );
        this.subfield_html = this.sub_field.get_checkbox_field();
    }

    change_sections_select(new_selected){
        animation_customizer.updated_animation.customization.sections_selected = new_selected;
        // if new_selected=='all' or 'random', save that info also in .sections
        if (new_selected=='all'||new_selected=='random'){
            animation_customizer.updated_animation.customization.sections = new_selected;
        }

        this.update_subfield();
        document.getElementById('sections_subfield').innerHTML=this.subfield_html;

        // check if settings are different now in comparison to current animation, if yes, show "apply"/"save animation"/"update" buttons
        animation_customizer.check_for_changes();
    }

    change_section(checkbox){
        var section_name = checkbox.parentElement.innerText;
        // replace 'all' or 'random' with real sections
        if (animation_customizer.updated_animation.customization.sections=='all' || animation_customizer.updated_animation.customization.sections=='random'){
            animation_customizer.updated_animation.customization.sections = this.sections;
        }
        if (checkbox.checked==false){
            // remove from list if now unchecked 
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