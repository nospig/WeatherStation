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
    doSend("WebSocket rocks");
}

function onClose(evt)
{
}

function onMessage(evt)
{
    console.log(evt.data);

    var messageData = JSON.parse(evt.data);

    if(messageData.type == "sensorReadings")
    {
        var readings = messageData.readings;
        document.getElementById("sensorTemp").innerHTML = readings.temp.toFixed(1) + "C";
        document.getElementById("sensorHumidity").innerHTML = readings.humidity.toFixed(1) + "%";
        document.getElementById("sensorPressure").innerHTML = readings.pressure + "mb";
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
