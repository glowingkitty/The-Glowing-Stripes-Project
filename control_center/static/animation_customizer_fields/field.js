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
        this.name = name
        this.options_list = this.process_options_list(options_list)
        this.selected_option = selected_option
        this.onchange_event = onchange_event
        this.sub_field_html = sub_field_html

        this.html = '<div name="field"><div class="main_field" name="main_field">'
    }

    process_options_list(options_list){
        if (options_list){
            if (typeof(options_list)=='string' && options_list.includes('ms')){
                // process "100ms-5000ms" info
                var options_list_start = parseInt(options_list.split('-')[0].replace(' ','').replace('ms',''))
                var options_list_end = parseInt(options_list.split('-')[1].replace(' ','').replace('ms',''))

                // generate steps in between
                var step
                options_list = []
                for (step = options_list_start; step<=options_list_end; step+=100) {
                    options_list[i] = {
                        'name':step.toString()+' ms',
                        'value':step
                    }
                } 
            }
            
            else if (typeof(options_list[1]) == 'number'){
                if (options_list[1]<1 && options_list[1]>0){
                    // generate percentage list
                    var i
                    for (i = 0; i < options_list.length; i++) {
                        options_list[i] = {
                            'name':(options_list[i]*100).toString()+'%',
                            'value':options_list[i]
                        }
                    }

                } else {
                    var i
                    for (i = 0; i < options_list.length; i++) {
                        options_list[i] = {
                            'name':options_list[i].toString(),
                            'value':options_list[i]
                        }
                    }
                }
            }
        }
        return options_list
    }

    add_name_div(){
        this.html+='<div class="field_name">'+this.name+'</div>'
    }

    add_subfield(){
        this.html+='<div id="'+this.name.toLowerCase()+'_subfield" class="subfield">'+this.sub_field_html+'</div>'
    }

    get_select_field(){
        this.add_name_div()

        if (this.options_list){
            // generate select field
            this.html+='<select onchange="'+this.onchange_event+'" class="customize_field_selector">'
            var i;
            for (i = 0; i < this.options_list.length; i++) {
                this.html += '<option value="' + this.options_list[i]['value'] + '"'
                // mark as selected if thats the case 
                if (this.options_list[i]['value'] == this.selected_option) {
                    this.html += ' selected'
                }
                this.html += '>'+this.options_list[i]['name'] + '</option>'
            }
            this.html+='</select>'

            // close main field
            this.html+='</div>'

            // generate field under select field
            if (this.sub_field_html){
                this.add_subfield()
            }

            // close field div
            this.html+='</div>'
        }
        return this.html
    }

    get_checkbox_field(){
        this.add_name_div()

        // generate checkboxes
        if (this.options_list){
            var i;
            for (i = 0; i < this.options_list.length; i++) {
                this.html += '<label class="checkbox">'+this.options_list[i]['name']
                this.html += '<input onchange="'+this.onchange_event+'" type="checkbox"'
                if (this.options_list[i]['checked']==true) {
                    this.html += ' checked="checked"'
                }
                this.html += '><span class="checkmark"></span></label>'
            }

            // generate field under checkbox field
            if (this.sub_field_html){
                this.add_subfield()
            }

            // close field div
            this.html+='</div>'
        }

        return this.html
    }
}