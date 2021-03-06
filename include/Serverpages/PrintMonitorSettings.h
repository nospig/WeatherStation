#include <avr/pgmspace.h>

const char printMonitorSettings_html[] PROGMEM =
{

"<!doctype html><html lang=\"en\"><head> <meta charset=\"utf-8\"> <meta name=\"viewport\" content=\"width=device-width, initial-scale=1, shrink-to-fit=no\"> <link rel=\"stylesheet\" href=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/css/bootstrap.min.css\" integrity=\"sha384-ggOyR0iXCbMQv3Xipma34MD+dH/1fQ784/j6cY/iJTQUOhcWr7x9JvoRxT2MZw1T\" crossorigin=\"anonymous\"> <link rel=\"stylesheet\" href=\"css/station.css\"> <title>Weather Station | OctoPrint Monitor Settings</title></head><body> %NAVBAR% <div class=\"container-fluid\" style=\"margin-top:80px\"> <h3>Settings for OctoPrint Monitor</h3> <div class=\"table-responsive-md\"> <table class=\"table\" style=\"background-color: white\"> <thead class=\"thead-light\"> <tr> <th>OctoPrint server settings</th> </tr></thead> <tbody> <tr> <td> <form action=\"/updatePrintMonitorSettings.html\"> <div class=\"form-check form-group\"> <label class=\"form-check-label\" for=\"check1\"> <input type=\"checkbox\" class=\"form-check-input\" id=\"printMonitorEnabled\" name=\"printMonitorEnabled\" value=\"Enabled\" %PRINTMONITORENABLED%>Enabled </label> </div><div id=\"printMonitorForm\"> <div class=\"form-group\"> <label for=\"octoPrintUrl\">OctoPrint address (don't include http://):</label> <input type=\"text\" class=\"form-control\" id=\"octoPrintUrl\" value=\"%PRINTMONITORURL%\" placeholder=\"OctoPrint address\" maxlength=\"64\" name=\"octoPrintUrl\"> </div><div class=\"form-group\"> <label for=\"octoPrintPort\">Port:</label> <input type=\"number\" class=\"form-control\" id=\"octoPrintPort\" value=\"%PRINTMONITOPORT%\" placeholder=\"Port number\" name=\"octoPrintPort\"> </div><div class=\"form-group\"> <label for=\"octoPrintUsername\">User name:</label> <input type=\"text\" class=\"form-control\" id=\"octoPrintUsername\" value=\"%PRINTMONITORUSERNAME%\" placeholder=\"User name\" maxlength=\"64\" name=\"octoPrintUsername\"> </div><div class=\"form-group\"> <label for=\"octoPrintPassword\">Password:</label> <input type=\"password\" class=\"form-control\" id=\"octoPrintPassword\" value=\"%PRINTMONITORPASSWORD%\" placeholder=\"Password\" maxlength=\"64\" name=\"octoPrintPassword\"> </div><div class=\"form-group\"> <label for=\"octoPrintAPIKey\">OctoPrint API Key:</label> <input type=\"text\" class=\"form-control\" id=\"octoPrintAPIKey\" value=\"%PRINTMONITORAPIKEY%\" placeholder=\"API key\" name=\"octoPrintAPIKey\"> </div><div class=\"form-group\"> <label for=\"octoPrintDisplayName\">Printer display name (not an OctoPrint setting, e.g. Ender 3)</label> <input type=\"text\" class=\"form-control\" id=\"octoPrintDisplayName\" value=\"%PRINTMONITORDISPLAYNAME%\" placeholder=\"Display name\" maxlength=\"64\" name=\"octoPrintDisplayName\"> </div></div><button type=\"submit\" class=\"btn btn-primary\">Save</button> </form> </td></tr></tbody> </table> </div></div><script src=\"https://code.jquery.com/jquery-3.3.1.slim.min.js\" integrity=\"sha384-q8i/X+965DzO0rT7abK41JStQIAqVgRVzpbzo5smXKp4YfRvH+8abtTE1Pi6jizo\" crossorigin=\"anonymous\"></script> <script src=\"https://cdnjs.cloudflare.com/ajax/libs/popper.js/1.14.7/umd/popper.min.js\" integrity=\"sha384-UO2eT0CpHqdSJQ6hJty5KVphtPhzWj9WO1clHTMGa3JDZwrnQq4sF86dIHNDz0W1\" crossorigin=\"anonymous\"></script> <script src=\"https://stackpath.bootstrapcdn.com/bootstrap/4.3.1/js/bootstrap.min.js\" integrity=\"sha384-JjSmVgyd0p3pXB1rRibZUAYoIIy6OrQ6VrjIEaFf/nJGzIxFDsf4x0xIM+B07jRM\" crossorigin=\"anonymous\"></script> <script src=\"js/printMonitorSettings.js\"></script></body></html>"

};

const char printMonitorSettings_js[] PROGMEM =
{

"function init()\n"
"{   \n"
"    if ($('#printMonitorEnabled').is(\":checked\"))\n"
"    {\n"
"        $(\"#printMonitorForm\").show();\n"
"    }\n"
"    else\n"
"    {\n"
"        $(\"#printMonitorForm\").hide();\n"
"    }\n"
"\n"
"    $(\"#printMonitorEnabled\").click(function()\n"
"    {\n"
"        if($(this).is(\":checked\"))\n"
"        {\n"
"            $(\"#printMonitorForm\").show();\n"
"        }\n"
"        else\n"
"        {\n"
"            $(\"#printMonitorForm\").hide();\n"
"        }\n"
"    });\n"
"}\n"
"\n"
"window.addEventListener(\"load\", init, false);"

};