#ifndef SENSORHUB_H
#define SENSORHUB_H

#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>

/**
 * @brief The SensorHub class handles communication with the SensorHub Socket
 */
class SensorHub : public QObject
{
    Q_OBJECT
public:
    explicit SensorHub(QObject *parent = 0);

    /**
     * @brief sendMessage sends a json message to the SensorHub
     * @param obj JsonObject to send
     */
    void sendMessage(const QJsonObject& obj);

signals:
    /**
     * @brief messageReceived is emitted when a new message arrived from the SensorHub
     * @param obj The received message
     */
    void messageReceived(QJsonObject obj);

private slots:
    /**
     * @brief dataReady Slot that is called when the Socket has new data to read
     */
    void dataReady();

private:
    /**
     * @brief _socket SensorHub Socket instance
     */
    QLocalSocket _socket;

    /**
     * @brief _unparsedBytes Bytes that are not yet parsed into a JsonObject
     */
    QByteArray _unparsedBytes;

    /**
     * @brief findEndOfJson is a Helper which scans a string for JsonObjects.
     *
     * This Helper is needed because it can happen that we only receive a half object from the socket.
     * This method will not transform the object to a JsonObject. It will only scan the string for the End of the object
     *
     * @param inp The String to scan
     * @param error String to fill with the error message
     * @return 0 if no complete object was found, -1 on fatal error, otherwise the number of bytes that form a complete JsonObject
     */
    int findEndOfJson(QString inp, QString* error);
};

#endif // SENSORHUB_H
