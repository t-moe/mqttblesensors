#include <QCoreApplication>
#include <QDebug>
#include "sensorhub.h"
#include "mqtt.h"
#include "controller.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv); //Create an event loop app

    SensorHub hub; //Create sensor hub. Will autoconnect

    MQTT mqtt; //Create MQQT client
    mqtt.waitOnConnected(); //wait till mqtt is fully connected

    Controller cont(mqtt,hub); //Let Controller Class handle all the communication between mqqt and sensorhub
    Q_UNUSED(cont);

    return a.exec(); //Execute Eventloop
}
