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
    Q_UNUSED(cont);

    return a.exec();
}
