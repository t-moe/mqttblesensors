/*------------------------------------------------------------------------------+
|	Javascript code for measure tab												|
+------------------------------------------------------------------------------*/
function dataArrived(data) { 
	if ($("#tempChart").data("chart")){
		switch (data.type) {
			case "temperature":{
				if($("#tempChart").data("array").getNumberOfRows() >= (recordTime * measuresPerSec)){
					$("#tempChart").data("array").removeRow(0);
				}
				
				$("#tempChart").data("array").addRows([
					['', data.raw]
				]);
				if($("#tempChart").is(":visible")){
					$("#tempChart").data("chart").draw($("#tempChart").data("array"), $("#tempChart").data("options"));
				}
				
			}
			break;
			case "accelerate":{
				if($("#accelChart").data("array").getNumberOfRows() >= (recordTime * measuresPerSec)){
					$("#accelChart").data("array").removeRow(0);
				}
				
				$("#accelChart").data("array").addRows([
					['', data.raw.x, data.raw.y, data.raw.z]
				]);
				if($("#accelChart").is(":visible")){
					$("#accelChart").data("chart").draw($("#accelChart").data("array"), $("#accelChart").data("options"));
				}
			}
			break;
			case "gyro":{
				if($("#gyroChart").data("array").getNumberOfRows() >= (recordTime * measuresPerSec)){
					$("#gyroChart").data("array").removeRow(0);
				}
				
				$("#gyroChart").data("array").addRows([
					['', data.raw.x, data.raw.y, data.raw.z]
				]);
				if($("#gyroChart").is(":visible")){
					$("#gyroChart").data("chart").draw($("#gyroChart").data("array"), $("#gyroChart").data("options"));
				}
			}
			break;
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
