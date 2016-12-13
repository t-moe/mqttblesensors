// JavaScript

/*------------------------------------------------------------------------------+
|	Menu Toolbar																|
+------------------------------------------------------------------------------*/
var measuresPerSec = 4;
var recordTime = 30;

function openMenuItem(Item) {
  $(".menuItem").hide();
  $("#"+Item).show(); 
  if (Item == "Measure") {
  	//drawAllCharts();
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
	devicesDetect(status.devices);
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
				if($("#tempChart").data("array").getNumberOfRows() >= (recordTime * measuresPerSec)){
					$("#tempChart").data("array").removeRow(0);
				}
				
				$("#tempChart").data("array").addRows([
					['', data.raw]
				]);
				$("#tempChart").data("chart").draw($("#tempChart").data("array"), $("#tempChart").data("options"));
			}
			break;
			case "accelerate":{
				if($("#accelChart").data("array").getNumberOfRows() >= (recordTime * measuresPerSec)){
					$("#accelChart").data("array").removeRow(0);
				}
				
				$("#accelChart").data("array").addRows([
					['', data.raw.x, data.raw.y, data.raw.z]
				]);
				$("#accelChart").data("chart").draw($("#accelChart").data("array"), $("#accelChart").data("options"));
			}
			break;
			case "gyro":{
				/* update chart */
				if($("#gyroChart").data("array").getNumberOfRows() >= (recordTime * measuresPerSec)){
					$("#gyroChart").data("array").removeRow(0);
				}
				
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

/*function drawAllCharts(){
	drawChartAccel();
	drawChartGyro();
	drawChartTemp();
}*/

/* Acceleration Chart */
function drawChartAccel() {
    var data = new google.visualization.DataTable();
    data.addColumn('string', 'time');
    data.addColumn('number', 'X');
    data.addColumn('number', 'Y');
    data.addColumn('number', 'Z');

	// no start values
	/*data.addRows([
		['',0,0,0]
    ]);*/

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

	// no start values
    /*data.addRows([
		['',0,0,0]
    ]);*/

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
   
   // no start values
    /*data.addRows([
		['',	0]
    ]);*/

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
