var websocket;

function init()
{   
    openWebSocket();
}

function updateSensorReadings(messageData)
{
    var readings = messageData.readings;
    $("#sensorTemp").html(readings.temp.toFixed(1) + "&#8451");
    $("#sensorHumidity").html(readings.humidity.toFixed(1) + "%");
    $("#sensorPressure").html(readings.pressure.toFixed(1) + " hPA");
}

function updateInternetCurrent(messageData)
{
    var current = messageData.currentReadings;
    var currentValue;
    var observedTime;
   
    $("#internetWeatherList").empty();

    currentValue = current.temp.toFixed(1) + "C - " + current.description;
    $("#internetWeatherList").append('<li class="list-group-item">' + currentValue + '</li>');
    
    currentValue = "Humidity: " + current.humidity + "%";
    $("#internetWeatherList").append('<li class="list-group-item">' + currentValue + '</li>');
    
    currentValue = "Wind speed: " + current.windSpeed + "m/s from " + current.windDirection + "&#8451";
    $("#internetWeatherList").append('<li class="list-group-item">' + currentValue + '</li>');

    observedTime = new Date(current.time * 1000);
    currentValue = "Observed at: " + observedTime.toLocaleTimeString("en-GB") + " on " + observedTime.toLocaleDateString("en-GB");
    $("#internetWeatherList").append('<li class="list-group-item">' + currentValue + '</li>');
}

function updateWeatherForecast(messageData)
{
    var list = messageData.list;
    var currentValue;
    var forecastTime;
    var dateString;
    var days = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"];

    $("#internetForecastList").empty();

    for(day of list)
    {
        forecastTime = new Date(day.time * 1000);
        dateString = days[forecastTime.getDay()];
    
        currentValue = dateString + " - " + day.description + ", min " + day.tempMin.toFixed(1) + "C, max " + day.tempMax.toFixed(1) + "C";
        $("#internetForecastList").append('<li class="list-group-item">' + currentValue + '</li>');
    }

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

    switch(messageData.type)
    {
        case "sensorReadings":
            updateSensorReadings(messageData);
            break;
        case "currentWeather":
            updateInternetCurrent(messageData);
            break;
        case "weatherForecast":
            updateWeatherForecast(messageData);
            break;
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
