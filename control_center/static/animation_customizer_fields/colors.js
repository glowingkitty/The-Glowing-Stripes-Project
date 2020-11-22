let ColorsCustomizer = class {
    get_colors_field(animation_id){
        // get default animation colors
        if ('rgb_color' in led_strips[selected_led_strip_id].last_animation['customization']){
            this.selected_colors = [led_strips[selected_led_strip_id].last_animation['customization']['rgb_color']]
        } else if ('rgb_colors' in led_strips[selected_led_strip_id].last_animation['customization']){
            this.selected_colors = led_strips[selected_led_strip_id].last_animation['customization']['rgb_colors']
        } 

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
        this.default_selected = this.get_default_selected_opton()
        if (this.default_selected == 'manual'){
            this.generate_manual_colors_subfield()
        } else {
            this.generate_random_colors_subfield()
        }

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

    get_default_selected_opton(){
        // if "num_random_colors" in "customization" and != null, then "random", else "manual"
        if ('num_random_colors' in led_strips[selected_led_strip_id].last_animation['customization'] 
            && led_strips[selected_led_strip_id].last_animation['customization']['num_random_colors']){
            return 'random'
        } else {
            return 'manual'
        }
    }

    convert_hex_to_rgb(hex){
        // TODO
    }

    convert_rgb_to_hex(rgb){
        // TODO
    }

    change_colors_select(new_selected){
        if (new_selected == 'manual'){
            this.generate_manual_colors_subfield()
        } else {
            this.generate_random_colors_subfield()
        }
        document.getElementById('colors_subfield').innerHTML=this.subfield_html
    }

    generate_manual_colors_subfield(){
        // generate "manual" subfield_html
        this.subfield_html = ''
        // for every min_num_color, create random color field input field
        var i;
        for (i = 0; i < this.selected_colors.length; i++){
            this.subfield_html+='<input type="color" onchange="colors_customizer.change_color('+i+',this.value)" value="'+this.convert_rgb_to_hex(this.selected_colors[i])+'" class="color_selector">'
        }
        // "remove_color_button"
        if (this.selected_colors.length>this.min_num_colors){
            this.subfield_html+='<div class="remove_color_button" onclick="colors_customizer.remove_color()"></div>'
        }

        // "add_color_button"
        if (this.selected_colors.length<this.max_num_colors){
            this.subfield_html+='<div class="add_color_button" onclick="colors_customizer.add_color()"></div>'
        }
    }

    generate_random_colors_subfield(){
        this.subfield_html = ''
        this.sub_field = new CustomizerField(
            'How many',
            [1,2,3,4,5,6,7,8,9,10],
            led_strips[selected_led_strip_id].last_animation['customization']['num_random_colors'],
            'colors_customizer.change_num_of_random_colors(this.value)'
        )
        this.subfield_html = this.sub_field.get_select_field()
    }

    add_color(){
        // TODO
        // show another color field with a random color
        var new_color
        var new_color_html
        // TODO place new_color_html before remove and add button and after existing colors
        document.getElementById('colors_subfield').innerHTML = document.getElementById('colors_subfield').innerHTML+

        // show "remove_color_button"

        // add new random color to temporary changes

        // update the preview animation
    }

    remove_color(){
        // TODO
    }

    change_color(num_in_list,new_color_hex){
        // convert hex into R,G,B values and save
        this.selected_colors[num_in_list] = this.convert_hex_to_rgb(new_color_hex)

        // TODO update preview animation
    }

    change_num_of_random_colors(new_num){
        // TODO
    }
}

colors_customizer = new ColorsCustomizer()