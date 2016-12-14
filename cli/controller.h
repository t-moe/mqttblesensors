#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>
#include <QJsonObject>
#include <QJsonArray>
#include <QHash>
#include <QDateTime>

class MQTT;
class SensorHub;

/**
 * @brief The Controller class exchanges messsages between the Sensorhub and the MQTT Broker.
 *
 * Communication (lowlevel) is handled by other classes.
 */
class Controller : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief Constructs a new Controller with the passed MQTT Broker & Sensorhub
     * @param broker The Broker instance to use. Must be valid & connected
     * @param hub The Sensorhub instance to use. Must be valid & connected
     */
    explicit Controller(MQTT& broker, SensorHub& hub);

signals:

private slots:
    /**
     * @brief brokerMessageReceived Slot that is called when the broker received a new message
     * @param obj Message that was received
     */
    void brokerMessageReceived(const QJsonObject& obj);

    /**
     * @brief hubMessageReceived Slot that is called when the Sensorhub received a new message
     * @param obj
     */
    void hubMessageReceived(const QJsonObject& obj);

private:
    MQTT& _broker;
    SensorHub& _hub;
    bool _scanning; //Whether we're currently scanning or not

    QStringList _scanAdresses; //List of adresses that are in range
    QJsonArray _scanList; //Data object with all devices in range (names & description)


    enum State{
        Connecting,  // Issued connecting
        Connected,   // Connecting ok. Issued first configuration
        Configured1, // First configuration ok. Issued 2nd configuration
        Configured2, // 2nd configuration ok. Issued 3rd configuration
        Configured3  // 3rd configuration ok. Ready to use
    };

    QHash<QString,State> _connectedDevices; //map from Adress => State (for all devices which are connecting or connected)
    QJsonArray _connectedList; //Array with all connected adresses
    QDateTime _lastGyroData; //Time when the last gyro data was sent to broker
    QDateTime _lastAccelData; //Time when the last accel data was sent to broker


    //------- "Event received from Sensorhub" callbacks ------------------
    void hubDeviceDiscovered(const QString&, const QJsonObject& device);
    void hubDeviceConnected(const QString& adress, const QJsonObject&);
    void hubGyroConfigured(const QString& adress, const QJsonObject&);
    void hubAccelConfigured(const QString& adress, const QJsonObject&);
    void hubTempConfigured(const QString& adress, const QJsonObject&);
    void hubDeviceDisconnected(const QString& adress, const QJsonObject&);
    void hubGyroData(const QString& adress, const QJsonObject& data);
    void hubAccelData(const QString& adress, const QJsonObject& data);
    void hubTemperatureData(const QString& adress, const QJsonObject& data);
    void hubMeasureStopped(const QString& adress, const QJsonObject&);

    //----- "Command received from broker" callbacks ---------------------
    void brokerCmdScan(bool scan);
    void brokerCmdConnect(const QString& adress);
    void brokerCmdDisconnect(const QString& adress);
    void brokerResendStatus();
};

#endif // CONTROLLER_H
