#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>

class MQTT;
class SensorHub;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(MQTT& broker, SensorHub& hub);

signals:

private slots:
    void brokerMessageReceived(QJsonObject obj);
    void hubMessageReceived(QJsonObject obj);

private:
    MQTT& _broker;
    SensorHub& _hub;
    bool _scanning;

    QStringList _scanAddresses;
    QJsonArray _scanList;
};

#endif // CONTROLLER_H
