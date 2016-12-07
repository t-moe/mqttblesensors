#include "controller.h"

#include "mqtt.h"
#include "sensorhub.h"

Controller::Controller(MQTT& broker, SensorHub& hub) :
    _broker(broker),
    _hub(hub),
    _scanning(false)
{
    connect(&_broker,&MQTT::messageReceived,this, &Controller::brokerMessageReceived);
    connect(&_hub,&SensorHub::eventReceived,this, &Controller::hubMessageReceived);

}

void Controller::brokerMessageReceived(QJsonObject obj)
{
   if(obj["scan"].isBool()) {
        bool shouldScan = obj["scan"].toBool();
        if(shouldScan==_scanning) {
            return;
        }
        _scanning = shouldScan;

        if(_scanning) {
            _scanAddresses.clear();
            _scanList = QJsonArray();
        }

        QJsonObject msg;
        msg["device"]="";
        msg["command"] = (shouldScan)?"StartBleScan":"StopBleScan";
        _hub.send(msg);



   } else {
       qDebug() << "Unknown command" << obj;
   }

}

void Controller::hubMessageReceived(QJsonObject obj)
{
    QString type = obj["event"].toString();
    if(type=="DeviceDiscovered") {
        QJsonObject device = obj["data"].toObject();
        hubDeviceDiscovered(device);
    }
}

void Controller::hubDeviceDiscovered(const QJsonObject &device)
{
    if(_scanning) {
        QString addr = device["id"].toString();
        QString desc = device["name"].toString();
        if(!_scanAddresses.contains(addr)) {
            QJsonObject deviceObject;
            deviceObject["addr"] = addr;
            deviceObject["desc"] = desc;
            _scanAddresses.append(addr);
            _scanList.append(deviceObject);

            QJsonObject msg;
            msg["devices"] = _scanList;
            _broker.sendMesage(msg);
        }
    }
}
