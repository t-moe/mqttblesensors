#include <QCoreApplication>
#include <QDebug>
#include "sensorhub.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);


    SensorHub hub;


    return a.exec();
}
