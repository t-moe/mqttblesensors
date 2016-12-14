/*------------------------------------------------------------------------------+
|   Javascript code for measure tab                                             |
+------------------------------------------------------------------------------*/
function dataArrived(data) { //new data from connected sensor arrived
    if ($("#tempChart").data("chart")){ //only update chart if it already exists
        var modifiedChart;
        var modifiedDataArray;
        
        switch (data.type) { //temperature data received
            case "temperature":{
                modifiedChart = $("#tempChart");
                modifiedDataArray = modifiedChart.data("array");
                
                modifiedDataArray.addRow(['', data.raw]);   //add new temp data
            }
            break;
            case "accelerate":{ //accelerate data received
                modifiedChart = $("#accelChart");
                modifiedDataArray = modifiedChart.data("array"); 
                
                modifiedDataArray.addRow(['', data.raw.x, data.raw.y, data.raw.z]); //add new acceleration data
            }
            break;
            case "gyro":{ //gyro data received
                modifiedChart = $("#gyroChart");
                modifiedDataArray = modifiedChart.data("array"); 
                
                modifiedDataArray.addRow(['', data.raw.x, data.raw.y, data.raw.z]); //add new gyro data
            }
            break;
        }
        
        /* clear oldest data value when buffer is full */
        if(modifiedDataArray.getNumberOfRows() >= (recordTime * measuresPerSec)){
            modifiedDataArray.removeRow(0);
        }
        
        /* update chart */
        if(modifiedChart.is(":visible")){
            console.log(modifiedChart.data("options"));
            modifiedChart.data("chart").draw(modifiedDataArray, modifiedChart.data("options"));
        }
    }
}

/* initialize and draw acceleration chart */
function drawChartAccel() {
    /* generate new data table */
    var data = new google.visualization.DataTable();
    
    /* chart configuration */
    data.addColumn('string', 'time');
    data.addColumn('number', 'X');
    data.addColumn('number', 'Y');
    data.addColumn('number', 'Z');
    
    data.addRow(['', 0, 0, 0]);
    
    var options = {
        chart: {
            title: 'Sensor Data'
        },
        width: 700,
        height: 350
    };
    
    /* set up chart */
    $("#accelChart").data("options", options);
    $("#accelChart").data("array", data);
    var chart = new google.charts.Line($("#accelChart")[0]);
    chart.draw(data, options);
    $("#accelChart").data("chart", chart);
}

/* initialize and draw gyro chart */
function drawChartGyro() {
    /* generate new data table */
    var data = new google.visualization.DataTable();
    
    /* chart configuration */
    data.addColumn('string', 'time');
    data.addColumn('number', 'X');
    data.addColumn('number', 'Y');
    data.addColumn('number', 'Z');
    
    data.addRow(['', 0, 0, 0]);
    
    var options = {
        chart: {
            title: 'Sensor Data'
        },
        width: 700,
        height: 350
    };
    
    /* set up chart */
    $("#gyroChart").data("options", options);
    $("#gyroChart").data("array", data);
    var chart = new google.charts.Line($("#gyroChart")[0]);
    chart.draw(data, options);
    $("#gyroChart").data("chart", chart);
}

/* initialize and draw temperature chart */
function drawChartTemp() {
    /* generate new data table */
    var data = new google.visualization.DataTable();
    
    /* chart configuration */
    data.addColumn('string', 'time');
    data.addColumn('number', '°C');
    
    data.addRow(['', 0]);
    
    var options = {
        chart: {
            title: 'Sensor Data'
        },
        width: 700,
        height: 350
    };
    
    /* set up chart */
    $("#tempChart").data("options", options);
    $("#tempChart").data("array", data);
    var chart = new google.charts.Line($("#tempChart")[0]);
    chart.draw(data, options);
    $("#tempChart").data("chart", chart);
}
