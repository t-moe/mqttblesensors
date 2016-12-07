#ifndef MQTT_H
#define MQTT_H

#include <QObject>
#include <QJsonObject>
#include "MQTTAsync.h"

class MQTT : public QObject
{
    Q_OBJECT
public:
    explicit MQTT(QObject *parent = 0);
    void sendMesage(const QJsonObject& msg);
    void waitOnConnected();
    ~MQTT();

private:
    void disconnect();

    MQTTAsync _client;
    MQTTAsync_connectOptions _connOpts;
    bool _connected;

    static void connlost(void *context, char *cause);
    static void onDisconnect(void* context, MQTTAsync_successData* response);
    static void onSend(void* context, MQTTAsync_successData* response);
    static void onConnectFailure(void* context, MQTTAsync_failureData* response);
    static void onConnect(void* context, MQTTAsync_successData* response);
    static int msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message);
signals:
    void messageReceived(QJsonObject msg);

public slots:
};

#endif // MQTT_H
