let ColorsCustomizer = class {
    get default_selected(){
        return animation_customizer.updated_animation['customization']['colors_selected']
    }

    get selected_colors(){
        return animation_customizer.updated_animation['customization']['rgb_colors']
    }

    get num_random_colors(){
        if ('num_random_colors' in animation_customizer.updated_animation['customization'] && animation_customizer.updated_animation['customization']['num_random_colors']){
            return animation_customizer.updated_animation['customization']['num_random_colors']
        } else {
            return null
        }
        
    }

    get_colors_field(animation_id){

        // if animation is "color" => 1 color min, 1 max
        if (animation_id=='9jwnqn8v3i'){
            this.min_num_colors = 1 // relevant for .remove_color() function
            this.max_num_colors = 1 // relevant for .add_color() function
        }
        // if animation is "transition" => 2 colors min, 10 max
        else if (animation_id=='7u9tjpd0gi'){
            this.min_num_colors = 2
            this.max_num_colors = 10
        }
        else {
            this.min_num_colors = 1
            this.max_num_colors = 10
        }
        
        // generate the field thats displayed under the select button
        this.update_subfield()

        this.field = new CustomizerField(
            'Colors',
            [
                {
                    'name':'Manual',
                    'value':'manual'
                },
                {
                    'name':'Random',
                    'value':'random'
                }
            ],
            this.default_selected,
            'colors_customizer.change_colors_select(this.value)',
            this.subfield_html
        )
        return this.field.get_select_field()
    }

    componentToHex(c) {
        var hex = c.toString(16);
        return hex.length == 1 ? "0" + hex : hex;
    }
    

    convert_rgb_to_hex(rgb){
        return "#" + this.componentToHex(rgb[0]) + this.componentToHex(rgb[1]) + this.componentToHex(rgb[2]);
    }

    convert_hex_to_rgb(hex){
        var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
        return result ? [
            parseInt(result[1], 16),
            parseInt(result[2], 16),
            parseInt(result[3], 16)
         ] : null;
    }

    update_subfield(){
        if (this.default_selected == 'manual'){
            this.generate_manual_colors_subfield()
        } else {
            this.generate_random_colors_subfield()
        }
    }

    change_colors_select(new_selected){
        animation_customizer.updated_animation['customization']['colors_selected'] = new_selected
        this.update_subfield()
        document.getElementById('colors_subfield').innerHTML=this.subfield_html
    }

    generate_manual_colors_subfield(){
        // generate "manual" subfield_html
        this.subfield_html = '<div id="all_colors">'
        // for every min_num_color, create random color field input field
        var i;
        for (i = 0; i < this.selected_colors.length; i++){
            this.subfield_html+='<input type="color" onchange="colors_customizer.change_color('+i+',this.value)" value="'+this.convert_rgb_to_hex(this.selected_colors[i])+'" class="color_selector">'
        }
        this.subfield_html+='</div>'

        // "remove_color_button"
        this.subfield_html+='<div class="remove_color_button'
        if (this.selected_colors.length>this.min_num_colors){
            this.subfield_html+='" onclick="colors_customizer.remove_color()"></div>'
        } else {
            this.subfield_html+=' display_none" onclick="colors_customizer.remove_color()"></div>'
        }

        // "add_color_button"
        this.subfield_html+='<div class="add_color_button'
        if (this.selected_colors.length<this.max_num_colors){
            this.subfield_html+=' display_none" onclick="colors_customizer.add_color()"></div>'
        }

        
    }

    generate_random_colors_subfield(){
        this.sub_field = new CustomizerField(
            'How many',
            [1,2,3,4,5,6,7,8,9,10],
            this.num_random_colors,
            'colors_customizer.change_num_of_random_colors(this.value)'
        )
        this.subfield_html = this.sub_field.get_select_field()
    }

    add_color(){
        if (this.selected_colors.length<this.max_num_colors){
            // add a new random color to this.selected_colors
            var new_color = [
                Math.round(Math.random()*255),
                Math.round(Math.random()*255),
                Math.round(Math.random()*255)
            ]
            animation_customizer.updated_animation['customization']['rgb_colors'].push(new_color)

            // show another color field with a random color
            var new_color_html = '<input type="color" onchange="colors_customizer.change_color('+(this.selected_colors.length-1)+',this.value)" value="'+this.convert_rgb_to_hex(new_color)+'" class="color_selector">'
            // place new_color_html before remove and add button and after existing colors
            document.getElementById('all_colors').innerHTML = document.getElementById('all_colors').innerHTML + new_color_html

            // show "remove_color_button"
            document.getElementsByClassName('remove_color_button')[0].classList.remove('display_none')

            // hide "add_color" button if maximum level reached
            if (this.selected_colors.length==this.max_num_colors){
                document.getElementsByClassName('add_color_button')[0].classList.add('display_none')
            }

            // TODO update the preview animation
        } else{
            console.log('add_color failed. Cannot go higher then max_num_colors.')
        }
    }

    remove_color(){
        if (this.selected_colors.length>this.min_num_colors){
            // remove last color in selected_colors
            animation_customizer.updated_animation['customization']['rgb_colors'].pop()

            // remove last "color_selector" from 'all_colors' div block (preview list)
            var all_colors = document.getElementById('all_colors');
            all_colors.removeChild(all_colors.lastElementChild);

            // hide "remove_color" button if minimum level reached
            if (this.selected_colors.length==this.min_num_colors){
                document.getElementsByClassName('remove_color_button')[0].classList.add('display_none')
            }

            // show "add_color" button again if not longer maximum level
            if (this.selected_colors.length<this.max_num_colors){
                document.getElementsByClassName('add_color_button')[0].classList.remove('display_none')
            }

            // TODO update the preview animation

        } else{
            console.log('remove_color failed. Cannot go lower then min_num_colors.')
        }
        
    }

    change_color(num_in_list,new_color_hex){
        // convert hex into R,G,B values and save
        animation_customizer.updated_animation['customization']['rgb_colors'][num_in_list] = this.convert_hex_to_rgb(new_color_hex)

        // TODO update preview animation
    }

    change_num_of_random_colors(new_num){
        // if new num_rando_colors is higher then previously, use "add_color", if lower, "remove_color"
        if (new_num > this.num_random_colors){
            var random_colors_difference = new_num - this.num_random_colors
            for (var i = 0; i < random_colors_difference; i++) {
                this.add_color()
            }

        } else if (this.num_random_colors > new_num){
            var random_colors_difference = this.num_random_colors - new_num
            for (var i = 0; i < random_colors_difference; i++) {
                this.remove_color()
            }

        }

        animation_customizer.updated_animation['customization']['num_random_colors'] = new_num
    }
}

colors_customizer = new ColorsCustomizer()