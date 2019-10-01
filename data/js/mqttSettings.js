function init()
{   
    if ($('#mqttEnabled').is(":checked"))
    {
        $("#mqttForm").show();
    }
    else
    {
        $("#mqttForm").hide();
    }

    $("#mqttEnabled").click(function()
    {
        if($(this).is(":checked"))
        {
            $("#mqttForm").show();
        }
        else
        {
            $("#mqttForm").hide();
        }
    });
}

window.addEventListener("load", init, false);