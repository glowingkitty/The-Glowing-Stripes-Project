let Popup = class {
    constructor(header='', message='', buttons=[]) {
        this.header = header;
        this.message = message;
        this.buttons = buttons;
        // buttons example = [{"style":"primary","text":"Continue","icon":"sync","onclick":"control.next_page()"}]
    }

    get popup_active() {
        if (document.getElementById('popup').classList.contains('display_none')){
            return false
        }
        else {
            return true
        }
    }

    show() {
        // if another popup is open: hide that popup, then continue with showing new popup
        if (this.popup_active){
            this.hide()
        }

        var self = this
        setTimeout(function(){
            // overwrite headline
            document.getElementById('popup_headline').innerHTML = self.header

            // overwrite message
            document.getElementById('popup_message').innerHTML = self.message
            
            // overwrite buttons
            var i;
            var buttons = ''
            for (i = 0; i < self.buttons.length; i++) {
                // get button values
                if ('style' in self.buttons[i] && self.buttons[i]['style']){
                    var style = self.buttons[i]['style']
                } else {
                    var style = 'primary'
                }
                if ('icon' in self.buttons[i] && self.buttons[i]['icon']){
                    var icon = self.buttons[i]['icon']
                } else {
                    var icon = null
                }
                if ('id' in self.buttons[i] && self.buttons[i]['id']){
                    var id = self.buttons[i]['id']
                } else {
                    var id = null
                }
                var text = self.buttons[i]['text']
                var onclick = self.buttons[i]['onclick']

                // add button
                buttons+='<a class="cta '

                if (style=='primary'){
                    buttons+='primary'
                } else{
                    buttons+='darkmode'
                }

                if (icon){
                    buttons+=' with_icon '+icon
                }
                buttons+='"'
                
                if (id){
                    buttons+=' id="'+id+'"'
                }

                buttons+=' onclick="'+onclick+'">'

                buttons+=text
                buttons+='</a>'
            }
            document.getElementById('popup_buttons').innerHTML = buttons

            // make popup visible
            document.getElementById('popup').classList.remove('display_none')
            document.getElementById('popup').classList.remove('opacity_null')

            // prevent scrolling of background (body)
            document.getElementsByTagName('body')[0].style.overflow='hidden'

        },200)
    }

    hide(){
        document.getElementsByTagName('body')[0].style.removeProperty('overflow')
        document.getElementById('popup').classList.add('opacity_null')
        setTimeout(function(){
            document.getElementById('popup').classList.add('display_none')
        },200)
    }
}

var popup = new Popup()