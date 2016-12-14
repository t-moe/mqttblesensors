/*------------------------------------------------------------------------------+
|	Javascript code for measure tab												|
+------------------------------------------------------------------------------*/
function dataArrived(data) { 
	if ($("#tempChart").data("chart")){
		var modifiedChart;
		var modifiedDataArray;
		
		switch (data.type) {
			case "temperature":{
				modifiedChart = $("#tempChart");
				modifiedDataArray = $("#tempChart").data("array");
				
				modifiedDataArray.addRow(['', data.raw]);
			}
			break;
			case "accelerate":{
				modifiedDataArray = $("#accelChart").data("array");
				modifiedChart = $("#accelChart");
				
				modifiedDataArray.addRow(['', data.raw.x, data.raw.y, data.raw.z]);
			}
			break;
			case "gyro":{
				modifiedDataArray = $("#gyroChart").data("array");
				modifiedChart = $("#gyroChart");
				
				modifiedDataArray.addRow(['', data.raw.x, data.raw.y, data.raw.z]);
			}
			break;
		}
		
		if(modifiedDataArray.getNumberOfRows() >= (recordTime * measuresPerSec)){
			modifiedDataArray.removeRow(0);
		}
		if(modifiedChart.is(":visible")){
			modifiedChart.data("chart").draw(modifiedDataArray, modifiedChart.data("options"));
		}
	}
}

/* Acceleration Chart */
function drawChartAccel() {
    var data = new google.visualization.DataTable();
    data.addColumn('string', 'time');
    data.addColumn('number', 'X');
    data.addColumn('number', 'Y');
    data.addColumn('number', 'Z');
	
	data.addRows([
				['', 0, 0, 0]
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
				['', 0, 0, 0]
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
				['', 0]
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
