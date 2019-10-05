var websocket;

function init()
{   
    openWebSocket();
}

function updateSensorReadings(messageData)
{
    var readings = messageData.readings;
    
    if(readings.metric)
    {
        $("#sensorTemp").html(readings.temp.toFixed(1) + "&#8451");
    }
    else
    {
        var farenheit = (readings.temp * 1.8) + 32;
        $("#sensorTemp").html(farenheit.toFixed(1) + "&#8457");
    }
    $("#sensorHumidity").html(readings.humidity.toFixed(1) + "%");

    if(readings.metric)
    {
        $("#sensorPressure").html(readings.pressure.toFixed(1) + " hPA");
    }
    else
    {
        $("#sensorPressure").html(readings.pressure.toFixed(1) + " mb");
    }
}

function updateInternetCurrent(messageData)
{
    var current = messageData.currentReadings;
    var currentValue;
    var observedTime;
   
    $("#internetWeatherList").empty();

    if(current.metric)
    {
        currentValue = current.temp.toFixed(1) + "C - " + current.description;
    }
    else
    {
        currentValue = current.temp.toFixed(1) + "F - " + current.description;
    }
    $("#internetWeatherList").append('<li class="list-group-item">' + currentValue + '</li>');
    
    currentValue = "Humidity: " + current.humidity + "%";
    $("#internetWeatherList").append('<li class="list-group-item">' + currentValue + '</li>');
    
    currentValue = "Wind speed: " + current.windSpeed + "m/s from " + current.windDirection + "&#176";
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
    
        if(messageData.metric)
        {
            currentValue = dateString + " - " + day.description + ", min " + day.tempMin.toFixed(1) + "C, max " + day.tempMax.toFixed(1) + "C";
        }
        else
        {
            currentValue = dateString + " - " + day.description + ", min " + day.tempMin.toFixed(1) + "F, max " + day.tempMax.toFixed(1) + "F";
        }
            
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
