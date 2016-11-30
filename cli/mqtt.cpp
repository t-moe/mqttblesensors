#include "mqtt.h"
#include <QJsonDocument>
#include <QDebug>



#define ADDRESS     "tcp://iot.i3s.bfh.ch:1883"

#define CLIENTID    "ExampleClientPub"
#define TOPIC       "EmbSy/gruppe_11/aui"
#define QOS         1
#define TIMEOUT     10000L

#define USERNAME    "gruppe_11"
#define PASSWORD    "P8G42XEe"

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTAsync.h"

#if !defined(WIN32)
#include <unistd.h>
#else
#include <windows.h>
#endif



void MQTT::connlost(void *context, char *cause)
{
    MQTT* inst = static_cast<MQTT*>(context);

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    qDebug() << "Connection lost. cause " << cause;

    qDebug() << "Reconnecting";

    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    if ((rc = MQTTAsync_connect(inst->_client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        qDebug() << "Failed to start connect, return code " << rc;
        inst->_connected = false;
    }
}


void MQTT::onDisconnect(void* context, MQTTAsync_successData* response)
{
     MQTT* inst = static_cast<MQTT*>(context);
     inst->_connected = false;
    qDebug() << "Successful disconnection";

}


void MQTT::onSend(void* context, MQTTAsync_successData* response)
{
    MQTT* inst = static_cast<MQTT*>(context);
    qDebug() << "Message with token value " << response->token << "delivery confirmed";
}


void MQTT::onConnectFailure(void* context, MQTTAsync_failureData* response)
{
    MQTT* inst = static_cast<MQTT*>(context);
    qDebug() << "Connect failed, rc" <<(response ? response->code : 0);
}




void MQTT::onConnect(void* context, MQTTAsync_successData* response)
{
    MQTT* inst = static_cast<MQTT*>(context);

    qDebug() <<"Successful connection";
    inst->_connected = true;
}





MQTT::MQTT(QObject *parent) : QObject(parent)
{
    _connected = false;

    MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
    int rc;

    MQTTAsync_create(&_client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTAsync_setCallbacks(_client, NULL, connlost, NULL, NULL);

    conn_opts.username = USERNAME;
    conn_opts.password = PASSWORD;
    conn_opts.keepAliveInterval = 20;
    conn_opts.cleansession = 1;
    conn_opts.onSuccess = onConnect;
    conn_opts.onFailure = onConnectFailure;
    conn_opts.context = this;
    if ((rc = MQTTAsync_connect(_client, &conn_opts)) != MQTTASYNC_SUCCESS)
    {
        qDebug() << "Failed to start connect, return code" << rc;
        return;
    }


}

void MQTT::sendMesage(const QJsonObject &msg)
{
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
    MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
    int rc;


    opts.onSuccess = onSend;
    opts.context = this;

    QJsonDocument doc;
    doc.setObject(msg);
    QByteArray arr = doc.toJson();

    pubmsg.payload = reinterpret_cast<void*>(arr.data());
    pubmsg.payloadlen = arr.size();
    pubmsg.qos = QOS;
    pubmsg.retained = 0;


    if ((rc = MQTTAsync_sendMessage(_client, TOPIC, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
    {
       qDebug() << "Failed to start sendMessage, return code " << rc;
    }
}

void MQTT::waitOnConnected()
{
    while(!_connected);
}

MQTT::~MQTT()
{
    disconnect();
    MQTTAsync_destroy(&_client);
}

void MQTT::disconnect()
{
    MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
    int rc;

    opts.onSuccess = onDisconnect;
    opts.context =  this;

    if ((rc = MQTTAsync_disconnect(_client, &opts)) != MQTTASYNC_SUCCESS)
    {
        qDebug() << "Failed to start sendMessage, return code" << rc;
    }
}
