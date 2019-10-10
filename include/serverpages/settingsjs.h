#include <avr/pgmspace.h>

const char settings_js[] PROGMEM =
{

"$(function () {\n"
"    $('.confirmResetSettings').click(function (e) {\n"
"        var options = {\n"
"            messageHeader: 'Reset Settings?',\n"
"            modalVerticalCenter: true,\n"
"            backgroundBlur: ['#header_wrap, #main_content_wrap, #footer_wrap'],\n"
"            autoFocusOnConfirmBtn: false,\n"
"          };\n"
"\n"
"        e.preventDefault();\n"
"        $.confirmModal('Are you sure you want to reset?', options, function (el) {\n"
"            window.location.href = \"/resetSettings.html\";\n"
"        });\n"
"    });\n"
"\n"
"    $('.confirmForgetWifi').click(function (e) {\n"
"        var options = {\n"
"            messageHeader: 'Forget WiFi?',\n"
"            modalVerticalCenter: true,\n"
"            backgroundBlur: ['#header_wrap, #main_content_wrap, #footer_wrap'],\n"
"            autoFocusOnConfirmBtn: false,\n"
"          };\n"
"\n"
"        e.preventDefault();\n"
"        $.confirmModal('Are you sure you want to forget the current Wifi settings?', options, function (el) {\n"
"            window.location.href = \"/forgetWiFi.html\";\n"
"        });\n"
"    });\n"
"});"

};