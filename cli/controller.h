#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>

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


    enum State{Connecting,Connected,Configured1,Configured2};

    QHash<QString,State> _connectedDevices;
    QJsonArray _connectedList;



    void hubDeviceDiscovered(const QJsonObject& device);
    void hubDeviceConnected(const QString& address);
    void hubGyroConfigured(const QString& address);
    void hubTempConfigured(const QString& address);
    void hubDeviceDisconnected(const QString& address);

    void brokerCmdScan(bool scan);
    void brokerCmdConnect(const QString& address);
    void brokerCmdDisconnect(const QString& address);
    void brokerResendStatus();
};

#endif // CONTROLLER_H
