function update_mode(new_mode) {
    axios.defaults.xsrfCookieName = 'csrftoken';
    axios.defaults.xsrfHeaderName = 'X-CSRFToken';
    axios.post("/", {
            "mode": new_mode
        })
        .then(function (response) {

        })
        .catch(function (error) {
            console.log(error);
        })
        .finally(function () {
            // always executed
        });

}