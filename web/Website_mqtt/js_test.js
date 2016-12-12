// JavaScript

/*------------------------------------------------------------------------------+
|	Menu Toolbar																|
+------------------------------------------------------------------------------*/
function openMenuItem(Item) {
  $(".menuItem").hide();
  $("#"+Item).show(); 
  if (Item == "Measure") {
  	drawAllCharts();
  }
}

/*------------------------------------------------------------------------------+
|	Menu Item Configuration														|
+------------------------------------------------------------------------------*/
// Called when the client connects
function onConnect() {
    // Once a connection has been made, make a subscription and send a message.
    console.log("onConnect");
    client.subscribe("EmbSy/gruppe_11/web");
}

// Called when the client could not connect
function onConnectionFailure() {
    console.log("onConnectionFailure");
}

// Called when the client loses its connection
function onConnectionLost(responseObject) {
    if (responseObject.errorCode !== 0) {
        console.log("onConnectionLost:" + responseObject.errorMessage);
    }
}

// Called when a message arrives
function onMessageArrived(message) {
    var obj = JSON.parse(message.payloadString);
    console.log("Message arrived: topic=" + message.destinationName + ", message=",obj);
	if ("status" in obj) {
		statusChange(obj.status);
	}
	else if("data" in obj){
		dataArrived(obj.data); 
	}
	else {
		console.log("no devices available");
	}
	/*var span = document.getElementById('message');*/
	/*span.textContent= "topic:	" + message.destinationName + "		message:	" + message.payloadString;*/
}

function send(obj) {
    if (!client) {
        return;
    }
    var message = new Paho.MQTT.Message(JSON.stringify(obj));
    message.destinationName = "EmbSy/gruppe_11/cli";
    client.send(message);
}

/* function lists the detected devices*/
function devicesDetect(devices){
	var numOfDevices = devices.length;
	
	/* add new list element */
	for (var i = 0; i < numOfDevices; i++) {
		if (findListElement(devices[i].addr) == false){
			console.log("Adress: " + devices[i].addr + " Description: " + devices[i].desc);
			var newDev = $("#ScanList li:first").clone(true);
			newDev.find("span:first").text(devices[i].desc + " (" + devices[i].addr + ")");
			newDev.data("Adress", devices[i].addr );
			newDev.show();
			newDev.appendTo("#ScanList");
		}
	}
	/* remove list element */
	var entries = $("#ScanList li:visible");
	for (var i = 0; i < entries.length; i++){
		if(findArrayElement(($(entries[i]).data("Adress")), devices) == false){
			$(entries[i]).remove();
		}
	}
}

/* function finds listed elements by adress */
function findListElement(adress){
	var entries = $("#ScanList li:visible");
	for (var i = 0; i < entries.length; i++){
		if ($(entries[i]).data("Adress") == adress){
			return $(entries[i]);
		}
	}
	return false;
}

function findArrayElement(adress, array){
	var arraySize = array.length;
	for (var i = 0; i < arraySize; i++) {
		if (array[i].addr == adress){
			return array[i];
		}
	}
	return false;
}

function statusChange(status) {
	$( "#scanCB" ).prop("checked", status.scan);
	devicesDetect(obj.devices);
	$( "#ScanList li" ).removeClass("w3-green w3-orange").find("span:eq(1)").text("Disconnected");
	for (var i = 0; i < status.connected.length; i++ ) {
		var element = findListElement(status.connected[i]);
		if (element != false) {
			element.addClass("w3-green");
            element.find("span:eq(1)").text("Connected");
		}
	}
}

function dataArrived(data) { 
	if ($("#Measure").is(":visible")){
		switch (data.type) {
			case "temperature":{
				//console.log(data.raw);
				$("#tempChart").data("array").addRows([
					['', data.raw]
				]);
				$("#tempChart").data("chart").draw($("#tempChart").data("array"), $("#tempChart").data("options"));
			}
			break;
			case "accelerate":{
				//console.log(data.raw.x);
				//console.log(data.raw.y);
				//console.log(data.raw.z);
				
				/* collect data and calculate average values */
				
				/*if(int i >= maxMeasurements)
				{
					average value of the last measurements is calculated
					add new data to chart
				}*/
				$("#accelChart").data("array").addRows([
					['', data.raw.x, data.raw.y, data.raw.z]
				]);
				$("#accelChart").data("chart").draw($("#accelChart").data("array"), $("#accelChart").data("options"));
			}
			break;
			case "gyro":{
				//console.log(data.raw.x);
				//console.log(data.raw.y);
				//console.log(data.raw.z);
				
				$("#gyroChart").data("array").addRows([
					['', data.raw.x, data.raw.y, data.raw.z]
				]);
				$("#gyroChart").data("chart").draw($("#gyroChart").data("array"), $("#gyroChart").data("options"));
			}
			break;
		}
	}
}

/*------------------------------------------------------------------------------+
|	Menu Item Measure															|
+------------------------------------------------------------------------------*/

function drawAllCharts(){
	drawChartAccel();
	drawChartGyro();
	drawChartTemp();
}
/* Acceleration Chart */

function drawChartAccel() {
    var data = new google.visualization.DataTable();
    data.addColumn('string', 'time');
    data.addColumn('number', 'X');
    data.addColumn('number', 'Y');
    data.addColumn('number', 'Z');

    data.addRows([
        ['', 37.8, 80.8, 41.8],
        ['', 30.9, 69.5, 32.4],
        ['', 25.4,   57, 25.7],
        ['', 11.7, 18.8, 10.5],
        ['', 11.9, 17.6, 10.4],
        ['',  8.8, 13.6,  7.7],
        ['',  7.6, 12.3,  9.6],
        ['', 12.3, 29.2, 10.6],
        ['', 16.9, 42.9, 14.8],
        ['', 12.8, 30.9, 11.6],
        ['',  5.3,  7.9,  4.7],
        ['',  6.6,  8.4,  5.2],
        ['',  4.8,  6.3,  3.6],
        ['',  4.2,  6.2,  3.4]
    ]);

    var options = {
        chart: {
            title: 'Sensor Data'
        },
        width: 700,
        height: 350
    };
	
	$("#accelChart").data("options", options);
	$("#accelChart").data("array", data);
    var chart = new google.charts.Line($("#accelChart")[0]);
    chart.draw(data, options);
	$("#accelChart").data("chart", chart);
}

/* Gyroscope Chart */

function drawChartGyro() {
    var data = new google.visualization.DataTable();
    data.addColumn('string', 'time');
    data.addColumn('number', 'X');
    data.addColumn('number', 'Y');
    data.addColumn('number', 'Z');

    data.addRows([
        ['', 37.8, 80.8, 41.8],
        ['', 30.9, 69.5, 32.4],
        ['', 25.4,   57, 25.7],
        ['', 11.7, 18.8, 10.5],
        ['', 11.9, 17.6, 10.4],
        ['',  8.8, 13.6,  7.7],
        ['',  7.6, 12.3,  9.6],
        ['', 12.3, 29.2, 10.6],
        ['', 16.9, 42.9, 14.8],
        ['', 12.8, 30.9, 11.6],
        ['',  5.3,  7.9,  4.7],
        ['',  6.6,  8.4,  5.2],
        ['',  4.8,  6.3,  3.6],
        ['',  4.2,  6.2,  3.4]
    ]);

    var options = {
        chart: {
            title: 'Sensor Data'
        },
        width: 700,
        height: 350
    };

    $("#gyroChart").data("options", options);
	$("#gyroChart").data("array", data);
    var chart = new google.charts.Line($("#gyroChart")[0]);
    chart.draw(data, options);
	$("#gyroChart").data("chart", chart);
}

/* Temperature Chart */

function drawChartTemp() {
    var data = new google.visualization.DataTable();
    data.addColumn('string', 'time');
    data.addColumn('number', '°C');
   
    data.addRows([
        ['', 37.8],
        ['', 30.9],
        ['', 25.4],
        ['', 11.7],
        ['', 11.9],
        ['',  8.8],
        ['',  7.6],
        ['', 12.3],
        ['', 16.9],
        ['', 12.8],
        ['',  5.3],
        ['',  6.6],
        ['',  4.8],
        ['',  4.2]
    ]);

    var options = {
        chart: {
            title: 'Sensor Data'
        },
        width: 700,
        height: 350
    };
	
	$("#tempChart").data("options", options);
	$("#tempChart").data("array", data);
    var chart = new google.charts.Line($("#tempChart")[0]);
    chart.draw(data, options);
	$("#tempChart").data("chart", chart);
}
