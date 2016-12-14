/*------------------------------------------------------------------------------+
|	Javascript code for configuration tab										|
+------------------------------------------------------------------------------*/

$(function(){ //call code when page is loaded
	
	$("#ScanList li" ).click(function() { //click on a entry in the list of sensors
		var allEntries = $("#ScanList li"); 
		for (var i = 0; i < allEntries.length; i++){ // for every entry in the gui
			var entry = $(allEntries[i]);
			
			if( entry.hasClass("w3-green") || entry.hasClass("w3-orange")) { //entry is connected or connecting => disconnect
				var obj = {
					disconnect: entry.data("Adress")
				};
				send(obj);
				entry.removeClass("w3-green w3-orange").find("span:eq(1)").text("Disconnecting"); //Change text and color (white, Disconnecting)
			} else if(entry.is(this)) { // not orange, not green, but clicked element
				// this device is disconnected: connect it
				var obj = {
					connect: entry.data("Adress")
				};
				send(obj);
				entry.addClass("w3-orange").find("span:eq(1)").text("Connecting"); // change text and color (orange, Connecting)
			}
		}
	});
	
	$( "#scanCB" ).change(function() { // Checkbox state changed => Activate/Deactivate scan
		var obj = {
			scan: $(this).prop("checked") //returns true or false
		};
		send(obj);
	});
});

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
	$( "#scanCB" ).prop("checked", status.scan); //Update checkbox state
	
	var devices = status.devices; //devices in range
	var numOfDevices = devices.length;
	var connectedDevices = status.connected; //actually connected devices
	
	/* add new list element for every item in scan list that is not in gui */
	for (var i = 0; i < numOfDevices; i++) {
		if (findListElement(devices[i].addr) == false){ //check that is not in gui
			console.log("Adress: " + devices[i].addr + " Description: " + devices[i].desc);
			var newDev = $("#ScanList li:first").clone(true);
			newDev.find("span:first").text(devices[i].desc + " (" + devices[i].addr + ")");
			newDev.data("Adress", devices[i].addr );
			newDev.show();
			newDev.appendTo("#ScanList");
		}
	}
	/* remove list elements that are no longer in scanlist or connected list */
	var entries = $("#ScanList li:visible");
	for (var i = 0; i < entries.length; i++){
	    var adress = $(entries[i]).data("Adress");
		if(findArrayElement(adress, devices) == false && 
		   findArrayElement(adress, connectedDevices) == false) {
			$(entries[i]).remove();
		}
	}
	
	/* Update status of connected devices */
	$( "#ScanList li" ).removeClass("w3-green"); //Remove green from all entries, we'll add it later again
	$( '#ScanList li:contains("Disconnecting")').find("span:eq(1)").text("Disconnected"); //Change entries in state Disconnecting to Disconnected
	
	//For every connected device
	for (var i = 0; i < connectedDevices.length; i++ ) {
		var element = findListElement(connectedDevices[i]);
		if (element != false) { //Element already in gui
			element.removeClass("w3-orange").addClass("w3-green"); //Change color to green
            element.find("span:eq(1)").text("Connected"); //Change text to connected
		} else { //Element not in GUI
		
			//Add new element
			var newDev = $("#ScanList li:first").clone(true);
			newDev.find("span:first").text(connectedDevices[i]);
			newDev.find("span:eq(1)").text("Connected");
			newDev.addClass("w3-green");
			newDev.data("Adress", connectedDevices[i]);
			newDev.show();
			newDev.appendTo("#ScanList");
		}
	}
}