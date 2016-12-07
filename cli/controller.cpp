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
        brokerCmdScan(shouldScan);
   } if(obj["connect"].isString()) {
        QString address = obj["connect"].toString();
        brokerCmdConnect(address);
   } else if(obj["disconnect"].isString()) {
       QString address = obj["disconnect"].toString();
       brokerCmdDisconnect(address);
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
    } else if(type=="DeviceConnected") {
        QString address = obj["device"].toString();
        hubDeviceConnected(address);
    } else if(type=="DeviceDisconnected") {
        QString address = obj["device"].toString();
        hubDeviceDisconnected(address);
    } else if(type=="GyroConfigured") {
        QString address = obj["device"].toString();
        hubGyroConfigured(address);
    } else if(type=="TempConfigured") {
        QString address = obj["device"].toString();
        hubTempConfigured(address);
    } else {
        qDebug() << "Unknown event type" << type;
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

void Controller::hubDeviceConnected(const QString &address)
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

void Controller::hubGyroConfigured(const QString &address)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices[address] = Controller::Configured1;
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

void Controller::hubTempConfigured(const QString &address)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices[address] = Controller::Configured2;

        QJsonObject msg;
        msg["command"] = "StartMeasurement";
        msg["device"] = address;
        _hub.send(msg);
        _connectedList.append(address);

        brokerResendStatus();
    }
}

void Controller::hubDeviceDisconnected(const QString &address)
{
    if(_connectedDevices.contains(address)) {
        _connectedDevices.remove(address);
        for(int i=0; i<_connectedList.size(); i++) {
            if(_connectedList[i].toString()==address) {
                _connectedList.removeAt(i);
            }
        }
        brokerResendStatus();
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

    brokerResendStatus();
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
        msg["command"] = "Disconnect";
        msg["device"] = address;
        _hub.send(msg);
    }
}

void Controller::brokerResendStatus()
{
    QJsonObject status;
    status["scan"] = _scanning;
    status["devices"] = _connectedList;
    QJsonObject msg;
    msg["status"] = status;
    _broker.sendMesage(msg,true); //Persist message
}
