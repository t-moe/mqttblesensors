#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <QDateTime>

class MQTT;
class SensorHub;

class Controller : public QObject
{
    Q_OBJECT
public:
    explicit Controller(MQTT& broker, SensorHub& hub);

signals:

private slots:
    void brokerMessageReceived(const QJsonObject& obj);
    void hubMessageReceived(const QJsonObject& obj);

private:
    MQTT& _broker;
    SensorHub& _hub;
    bool _scanning;

    QStringList _scanAddresses;
    QJsonArray _scanList;


    enum State{Connecting,Connected,Configured1,Configured2, Configured3};

    QHash<QString,State> _connectedDevices;
    QJsonArray _connectedList;
    QDateTime _lastGyroData;
    QDateTime _lastAccelData;



    void hubDeviceDiscovered(const QString&, const QJsonObject& device);
    void hubDeviceConnected(const QString& address, const QJsonObject&);
    void hubGyroConfigured(const QString& address, const QJsonObject&);
    void hubAccelConfigured(const QString& address, const QJsonObject&);
    void hubTempConfigured(const QString& address, const QJsonObject&);
    void hubDeviceDisconnected(const QString& address, const QJsonObject&);
    void hubGyroData(const QString& adress, const QJsonObject& data);
    void hubAccelData(const QString& adress, const QJsonObject& data);
    void hubTemperatureData(const QString& adress, const QJsonObject& data);
    void hubMeasureStopped(const QString& address, const QJsonObject&);

    void brokerCmdScan(bool scan);
    void brokerCmdConnect(const QString& address);
    void brokerCmdDisconnect(const QString& address);
    void brokerResendStatus();
};

#endif // CONTROLLER_H
