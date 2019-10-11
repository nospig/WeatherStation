function init()
{   
    if ($('#thingSpeakEnabled').is(":checked"))
    {
        $("#thingSpeakForm").show();
    }
    else
    {
        $("#thingSpeakForm").hide();
    }

    $("#thingSpeakEnabled").click(function()
    {
        if($(this).is(":checked"))
        {
            $("#thingSpeakForm").show();
        }
        else
        {
            $("#thingSpeakForm").hide();
        }
    });
}

window.addEventListener("load", init, false);