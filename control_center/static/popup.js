let Popup = class {
    constructor(header='', message='', buttons=[]) {
        this.header = header;
        this.message = message;
        this.buttons = buttons;
        this.i=0;
        // buttons example = [{"style":"primary","text":"Continue","icon":"sync","onclick":"control.next_page()"}]
    }

    get popup_active() {
        if (document.getElementById('popup').classList.contains('display_none')){
            return false;
        }
        else {
            return true;
        }
    }

    show() {
        // make sure animation customizer is hidden
        animation_customizer.close();

        // if another popup is open: hide that popup, then continue with showing new popup
        if (this.popup_active){
            this.hide();
        }

        var self = this;
        setTimeout(function(){
            // overwrite headline
            document.getElementById('popup_headline').innerHTML = self.header;

            // overwrite message
            document.getElementById('popup_message').innerHTML = self.message;
            
            // overwrite buttons
            var buttons = '';
            for (this.i = 0; this.i < self.buttons.length; this.i++) {
                // get button values
                if ('style' in self.buttons[this.i] && self.buttons[this.i].style){
                    this.style = self.buttons[this.i].style;
                } else {
                    this.style = 'primary';
                }
                if ('icon' in self.buttons[this.i] && self.buttons[this.i].icon){
                    this.icon = self.buttons[this.i].icon;
                } else {
                    this.icon = null;
                }
                if ('id' in self.buttons[this.i] && self.buttons[this.i].id){
                    this.id = self.buttons[this.i].id;
                } else {
                    this.id = null;
                }
                if ('hide' in self.buttons[this.i] && self.buttons[this.i].hide==true){
                    this.hide = true;
                } else {
                    this.hide = false;
                }
                var text = self.buttons[this.i].text;
                var onclick = self.buttons[this.i].onclick;

                // add button
                buttons+='<a class="cta ';

                if (this.style=='primary'){
                    buttons+='primary';
                } else{
                    buttons+='darkmode';
                }

                if (this.icon){
                    buttons+=' with_icon '+this.icon;
                }

                // if button is supposed to be hidden - hide by default
                if (this.hide==true){
                    buttons+=' display_none';
                }
                
                buttons+='"';
                
                if (this.id){
                    buttons+=' id="'+this.id+'"';
                }

                buttons+=' onclick="'+onclick+'">';

                buttons+=text;
                buttons+='</a>';
            }
            document.getElementById('popup_buttons').innerHTML = buttons;

            // make popup visible
            document.getElementById('popup').classList.remove('display_none');
            document.getElementById('popup').classList.remove('opacity_null');

            // prevent scrolling of background (body)
            document.getElementsByTagName('body')[0].style.overflow='hidden';

        },200);
    }

    hide(){
        document.getElementsByTagName('body')[0].style.removeProperty('overflow');
        document.getElementById('popup').classList.add('opacity_null');
        setTimeout(function(){
            document.getElementById('popup').classList.add('display_none');
        },200);
    }
};

var popup = new Popup();