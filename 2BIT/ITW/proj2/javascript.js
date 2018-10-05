$(window).scroll(function () {
    if ($(document).scrollTop() > 430) {
        $('nav').addClass('sticky');
    }
    else {
        $('nav').removeClass('sticky');
    }
});
$("#sluzby").click(function () {
    $('#hide').toggleClass("hidden");
});
$(document).ready(function () {
    var status = getCookie();
    if (status > 0) {
        setCookie(1);
        document.getElementById('cookie_box').style.display = "block";
    } else {
        setCookie(0);
        document.getElementById('cookie_box').style.display = "none";
    }
});
function getCookie() {
    var cookies = document.cookie.split(';');
    var FirstCookie = cookies[0];
    var FirstValue = FirstCookie.split('=')[1];
    return FirstValue;
}
function setCookie(value) {
    var expires = new Date();
    expires.setMonth(expires.getYear() + 1);
    document.cookie = 'divStav' + '=' + value + ';EXPIRES=' + expires.toGMTString();
}
function ShowHideCookieBox() {
    var display = document.getElementById('cookie_box').style.display;
    if (display == "block") {
        document.getElementById('cookie_box').style.display = "none";
        setCookie(0);
    } else {
        document.getElementById('cookie_box').style.display = "block";
        setCookie(1);
    }
}