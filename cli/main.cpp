#include <QCoreApplication>
#include <QDebug>
#include "sensorhub.h"
#include "mqtt.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    SensorHub hub;


    MQTT mqtt;


    return a.exec();
}
