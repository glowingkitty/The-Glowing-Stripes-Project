let CustomizerField = class {
    constructor(
        name,
        options_list,
        selected_option,
        onchange_event,
        sub_field_html=null
    ) {
        // options_list example for select:
        // [
        // {
        //    "name":"10%",
        //    "value":0.1
        // },
        // {
        //    "name":"20%",
        //    "value":0.2
        // }
        // ]
        // options_list example for checkbox:
        // [
        // {
        //    "name":"Section 1",
        //    "checked":true
        // },
        // {
        //    "name":"Section 2",
        //    "checked":true
        // }
        // ]
        this.name = name;
        this.options_list = this.process_options_list(options_list);
        this.selected_option = selected_option;
        this.onchange_event = onchange_event;
        this.sub_field_html = sub_field_html;
        this.i=0;

        this.html = '<div name="field"><div class="main_field" name="main_field">';
    }

    process_options_list(options_list){
        if (options_list){
            if (typeof(options_list)=='string' && options_list.includes('ms')){
                // process "100ms-5000ms" info
                var options_list_start = parseInt(options_list.split('-')[0].replace(' ','').replace('ms',''));
                var options_list_end = parseInt(options_list.split('-')[1].replace(' ','').replace('ms',''));

                // generate steps in between
                var step = options_list_start;
                options_list = [];
                for (this.i = 1; step<=options_list_end; this.i++) {
                    options_list[this.i] = {
                        'name':step.toString()+' ms',
                        'value':step
                    };
                    step+=100;
                } 
            }
            
            else if (typeof(options_list[1]) == 'number'){
                if (options_list[1]<1 && options_list[1]>0){
                    // generate percentage list
                    for (this.i = 0; this.i < options_list.length; this.i++) {
                        options_list[this.i] = {
                            'name':(options_list[this.i]*100).toString()+'%',
                            'value':options_list[this.i]
                        };
                    }

                } else {
                    for (this.i = 0; this.i < options_list.length; this.i++) {
                        options_list[this.i] = {
                            'name':options_list[this.i].toString(),
                            'value':options_list[this.i]
                        };
                    }
                }
            }
        }
        return options_list;
    }

    add_name_div(){
        this.html+='<div class="field_name">'+this.name+'</div>';
    }

    add_subfield(){
        this.html+='<div id="'+this.name.toLowerCase()+'_subfield" class="subfield">'+this.sub_field_html+'</div>';
    }

    get_select_field(){
        this.add_name_div();

        if (this.options_list){
            // generate select field
            this.html+='<select onchange="'+this.onchange_event+'" class="customize_field_selector">';
            for (this.i = 0; this.i < this.options_list.length; this.i++) {
                if (this.i in this.options_list){
                    this.html += '<option value="' + this.options_list[this.i].value + '"';
                    // mark as selected if thats the case 
                    if (this.options_list[this.i].value == this.selected_option) {
                        this.html += ' selected';
                    }
                    this.html += '>'+this.options_list[this.i].name + '</option>';
                }
            }
            this.html+='</select>';

            // close main field
            this.html+='</div>';

            // generate field under select field
            if (this.sub_field_html){
                this.add_subfield();
            }

            // close field div
            this.html+='</div>';
        }
        return this.html;
    }

    get_checkbox_field(){
        this.add_name_div();

        // generate checkboxes
        if (this.options_list){
            this.html += '<div class="sections_block">';
            for (this.i = this.options_list.length-1; this.i >=0; this.i--) {
                this.html += '<label class="checkbox block">'+this.options_list[this.i];
                this.html += '<input onchange="'+this.onchange_event+'" type="checkbox"';
                if (this.selected_option.indexOf(this.options_list[this.i]) >= 0) {
                    this.html += ' checked="checked"';
                }
                this.html += '><span class="checkmark"></span></label>';
            }
            this.html += '</div>';

            // generate field under checkbox field
            if (this.sub_field_html){
                this.add_subfield();
            }

            // close field div
            this.html+='</div>';
        }

        return this.html;
    }
};