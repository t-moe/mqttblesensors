#ifndef MQTT_H
#define MQTT_H

#include <QObject>
#include <QJsonObject>
#include "MQTTAsync.h"

/**
 * @brief The MQTT class handles communication with the MQTT Broker
 */
class MQTT : public QObject
{
    Q_OBJECT
public:

    explicit MQTT(QObject *parent = 0);

    /**
     * @brief sendMesage sends a json message to the MQTT Broker
     * @param msg JsonObject to send
     * @param persist Whether the message should be persisted or not (=> resend this message to new clients)
     */
    void sendMesage(const QJsonObject& msg, bool persist=false);

    /**
     * @brief waitOnConnected blocks the calling thread until the MQTT Broker is connected.
     */
    void waitOnConnected();

    ~MQTT();

signals:
    /**
     * @brief messageReceived is emitted when a new message arrived from the MQTT Broker
     * @param msg The received message
     */
    void messageReceived(QJsonObject msg);

private:
    void disconnect(); //Disconnect from MQTT Broker

    MQTTAsync _client; //MQTT Lowlevel client
    MQTTAsync_connectOptions _connOpts; //MQTT Lowlevel Connect options
    volatile bool _connected; //Whether we're connected or not

    //-------------------MQTT Library callbacks--------------------------
    // context parameter is always a reference to our MQTT class instance ("this")
    static void onConnectionLost(void *context, char *cause);
    static void onDisconnect(void* context, MQTTAsync_successData* response);
    static void onSend(void* context, MQTTAsync_successData* response);
    static void onConnectFailure(void* context, MQTTAsync_failureData* response);
    static void onConnect(void* context, MQTTAsync_successData* response);
    static int onMessageArrived(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
};

#endif // MQTT_H
