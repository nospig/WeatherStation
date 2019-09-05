var websocket;

function init()
{
    openWebSocket();
}

function openWebSocket() 
{
    websocket = new WebSocket('ws://' + document.location.host + '/ws');
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    websocket.onerror = function(evt) { onError(evt) };
}

function onOpen(evt)
{    
    $("#dashboardTitle").html("Dashboard - connected");
}

function onClose(evt)
{
    $("#dashboardTitle").html("Dashboard - not connected");
}

function onMessage(evt)
{
    console.log(evt.data);

    var messageData = JSON.parse(evt.data);

    if(messageData.type == "sensorReadings")
    {
        var readings = messageData.readings;
        $("#sensorTemp").html(readings.temp.toFixed(1) + "C");
        $("#sensorHumidity").html(readings.humidity.toFixed(1) + "%");
        $("#sensorPressure").html(readings.pressure + "mb");
    }
}

function onError(evt)
{
}

function doSend(message)
{
    websocket.send(message);
}

window.addEventListener("load", init, false);
