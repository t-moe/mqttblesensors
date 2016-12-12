#include "controller.h"

#include "mqtt.h"
#include "sensorhub.h"
#include <functional>

#define DATA_PER_SEC 4

Controller::Controller(MQTT& broker, SensorHub& hub) :
    _broker(broker),
    _hub(hub),
    _scanning(false)
{
    connect(&_broker,&MQTT::messageReceived,this, &Controller::brokerMessageReceived);
    connect(&_hub,&SensorHub::eventReceived,this, &Controller::hubMessageReceived);

    brokerResendStatus(); //to reset the previous persistent message
}

void Controller::brokerMessageReceived(const QJsonObject &obj)
{
   if(obj["scan"].isBool()) {
        bool shouldScan = obj["scan"].toBool();
        brokerCmdScan(shouldScan);
   } else if(obj["connect"].isString()) {
        QString address = obj["connect"].toString();
        brokerCmdConnect(address);
   } else if(obj["disconnect"].isString()) {
       QString address = obj["disconnect"].toString();
       brokerCmdDisconnect(address);
   } else {
       qDebug() << "Unknown command" << obj;
   }

}

void Controller::hubMessageReceived(const QJsonObject &obj)
{
    const QMap<QString,void (Controller::*)(const QString&, const QJsonObject&)> callbacks = {
        {"DeviceDiscovered",&Controller::hubDeviceDiscovered},
        {"DeviceConnected",&Controller::hubDeviceConnected},
        {"DeviceDisconnected",&Controller::hubDeviceDisconnected},
        {"GyroConfigured",&Controller::hubGyroConfigured},
        {"AccelConfigured",&Controller::hubAccelConfigured},
        {"TempConfigured",&Controller::hubTempConfigured},
        {"MeasurementStopped",&Controller::hubMeasureStopped},
        {"GyroData",&Controller::hubGyroData},
        {"AccelData",&Controller::hubAccelData},
        {"Temperature",&Controller::hubTemperatureData}
    };


    QString type = obj["event"].toString();

    if(callbacks.contains(type)) { //Callback available for this event type
        QJsonObject data = obj["data"].toObject();
        QString adress = obj["device"].toString();

        auto memfcn = callbacks[type]; //get member function out of callback map
        (this->*memfcn)(adress,data); //Call member function on "this" instance
    } else {
        qDebug() << "Unknown event type" << type;
    }
}

void Controller::hubDeviceDiscovered(const QString &, const QJsonObject &device)
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

            brokerResendStatus(); //because the scan list was modified
        }
    }
}

void Controller::hubDeviceConnected(const QString &address, const QJsonObject&)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices[address] = Controller::Connected;
        QJsonObject msg;
        msg["command"] = "ConfigGyro";
        msg["device"] = address;

        QJsonObject data;
        data["on"] = true;
        data["fullscale"] = 500;
        data["odr"] = 95;
        msg["data"] = data;

        _hub.send(msg);
    }
}

void Controller::hubGyroConfigured(const QString &address, const QJsonObject&)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices[address] = Controller::Configured1;
        QJsonObject msg;
        msg["command"] = "ConfigAccel";
        msg["device"] = address;

        QJsonObject data;
        data["on"] = true;
        data["fullscale"] = 4;
        data["odr"] = 12.5;
        msg["data"] = data;

        _hub.send(msg);
    }
}


void Controller::hubAccelConfigured(const QString &address, const QJsonObject &)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices[address] = Controller::Configured2;
        QJsonObject msg;
        msg["command"] = "ConfigTemp";
        msg["device"] = address;

        QJsonObject data;
        data["on"] = true;
        data["odr"] = 1;
        msg["data"] = data;

        _hub.send(msg);
    }
}

void Controller::hubTempConfigured(const QString &address, const QJsonObject&)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices[address] = Controller::Configured3;

        QJsonObject msg;
        msg["command"] = "StartMeasurement";
        msg["device"] = address;
        _hub.send(msg);
        _connectedList.append(address);

        brokerResendStatus(); //because the device is now connected
    }
}

void Controller::hubDeviceDisconnected(const QString &address, const QJsonObject&)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices.remove(address);
        for(int i=0; i<_connectedList.size(); i++) {
            if(_connectedList[i].toString()==address) {
                _connectedList.removeAt(i);
            }
        }
        brokerResendStatus(); //because the device is now disconnected
    }
}

void Controller::hubGyroData(const QString& adress,const QJsonObject &data)
{
    if(_connectedDevices.contains(adress)) {
        if(_lastGyroData.isValid() && ((QDateTime::currentDateTime().toMSecsSinceEpoch() - _lastGyroData.toMSecsSinceEpoch()) < 1000/DATA_PER_SEC)) {
            return; //ignore value
        }
        _lastGyroData = QDateTime::currentDateTime();


        QJsonObject raw;
        raw["x"] = data["x"];
        raw["y"] = data["y"];
        raw["z"] = data["z"];

        QJsonObject dat;
        dat["device"] = adress;
        dat["type"] = "gyro";
        dat["raw"] = raw;

        QJsonObject msg;
        msg["data"] = dat;
        _broker.sendMesage(msg);
    }
}

void Controller::hubAccelData(const QString &adress, const QJsonObject &data)
{
    if(_connectedDevices.contains(adress)) {
        if(_lastAccelData.isValid() && ((QDateTime::currentDateTime().toMSecsSinceEpoch() - _lastAccelData.toMSecsSinceEpoch()) < 1000/DATA_PER_SEC)) {
            return; //ignore value
        }
        _lastAccelData = QDateTime::currentDateTime();


        QJsonObject raw;
        raw["x"] = data["x"];
        raw["y"] = data["y"];
        raw["z"] = data["z"];

        QJsonObject dat;
        dat["device"] = adress;
        dat["type"] = "accelerate";
        dat["raw"] = raw;

        QJsonObject msg;
        msg["data"] = dat;
        _broker.sendMesage(msg);
    }
}

void Controller::hubTemperatureData(const QString &address, const QJsonObject &data)
{
    if(_connectedDevices.contains(address)) {
        QJsonObject dat;
        dat["device"] = address;
        dat["type"] = "temperature";
        dat["raw"] = data["value"];

        QJsonObject msg;
        msg["data"] = dat;
        _broker.sendMesage(msg);
    }
}

void Controller::hubMeasureStopped(const QString &address, const QJsonObject&)
{
    if(_connectedDevices.contains(address)) {
        QJsonObject msg;
        msg["command"] = "Disconnect";
        msg["device"] = address;
        _hub.send(msg);
    }
}



void Controller::brokerCmdScan(bool shouldScan)
{
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

    brokerResendStatus(); //Because scan has changed it's value
}

void Controller::brokerCmdConnect(const QString &address)
{
    if(!_connectedDevices.contains(address)) {
        _connectedDevices.insert(address,Controller::Connecting);
        QJsonObject msg;
        msg["command"] = "Connect";
        msg["device"] = address;
        _hub.send(msg);
    }
}

void Controller::brokerCmdDisconnect(const QString &address)
{
    if(_connectedDevices.contains(address)) {
        QJsonObject msg;
        msg["command"] = "StopMeasurement";
        //msg["command"] = "Disconnect";
        msg["device"] = address;
        _hub.send(msg);

        //Temporary to fix
        //TODO: remove?
        //hubDeviceDisconnected(address);
    }
}

void Controller::brokerResendStatus()
{
    QJsonObject status;
    status["scan"] = _scanning;
    status["connected"] = _connectedList;
    status["devices"] = _scanList;
    QJsonObject msg;
    msg["status"] = status;
    _broker.sendMesage(msg,true); //Persist message
}
