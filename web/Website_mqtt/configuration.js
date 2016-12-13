/*------------------------------------------------------------------------------+
|	Javascript code for configuration tab										|
+------------------------------------------------------------------------------*/

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

/* function finds listed elements by received adress */
function findListElement(adress){
	var entries = $("#ScanList li:visible");
	for (var i = 0; i < entries.length; i++){
		if ($(entries[i]).data("Adress") == adress){
			return $(entries[i]);
		}
	}
	return false;
}

/* function finds received array element by listed adress */
function findArrayElement(adress, array){
	var arraySize = array.length;
	for (var i = 0; i < arraySize; i++) {
		if (array[i].addr == adress){
			return array[i];
		}
	}
	return false;
}

/* function sets property of scanCB and status and color of all listed devices */
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