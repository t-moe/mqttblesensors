#include <QCoreApplication>
#include <QDebug>
#include "sensorhub.h"
#include "mqtt.h"
#include "controller.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    SensorHub hub;


    MQTT mqtt;
    mqtt.waitOnConnected();

    Controller cont(mqtt,hub);

    /*QJsonObject tst;
    tst["a"] = 3;
    tst["b"]= "hallo";

    mqtt.sendMesage(tst);


    tst["devices"] = 13;
    mqtt.sendMesage(tst);*/


    return a.exec();
}
