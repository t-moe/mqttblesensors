#include "mqtt.h"
#include <QJsonDocument>
#include <QDebug>



#define ADDRESS     "tcp://iot.i3s.bfh.ch:1883"

#define CLIENTID    "ExampleClientPub"
#define TOPIC_SEND      "EmbSy/gruppe_11/web"
#define TOPIC_RECV      "EmbSy/gruppe_11/cli"
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
    int rc;

    qDebug() << "Connection lost. cause " << cause;

    qDebug() << "Reconnecting";
    if ((rc = MQTTAsync_connect(inst->_client, &inst->_connOpts)) != MQTTASYNC_SUCCESS)
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
    MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;

    qDebug() <<"Successful connection";


    inst->_connected = true;
    int rc;

    if ((rc = MQTTAsync_subscribe(inst->_client, TOPIC_RECV, QOS, &opts)) != MQTTASYNC_SUCCESS)
    {
        qDebug() << "Failed to start subscribe, return code" << rc;
    }
}


int MQTT::msgarrvd(void *context, char *topicName, int topicLen, MQTTAsync_message *message)
{
    MQTT* inst = static_cast<MQTT*>(context);


    //qDebug()  << "Message arrived. Topic" << topicName;

    char* content = static_cast<char*>(message->payload);
    QByteArray arr = QByteArray::fromRawData(content,message->payloadlen);

    QJsonParseError scanErr;
    QJsonDocument doc = QJsonDocument::fromJson(arr,&scanErr);
    if(scanErr.error!=QJsonParseError::NoError) {
        qDebug() << scanErr.errorString();
        qDebug() << QString(arr);
    } else {
        QJsonObject obj = doc.object();
        qDebug() << "MQQT received" << obj;
        emit inst->messageReceived(obj);
    }

    MQTTAsync_freeMessage(&message);
    MQTTAsync_free(topicName);

    return 1;
}




MQTT::MQTT(QObject *parent) : QObject(parent), _connOpts(MQTTAsync_connectOptions_initializer)
{
    _connected = false;

    _connOpts.username = USERNAME;
    _connOpts.password = PASSWORD;
    _connOpts.keepAliveInterval = 20;
    _connOpts.cleansession = 1;
    _connOpts.onSuccess = onConnect;
    _connOpts.onFailure = onConnectFailure;
    _connOpts.context = this;


    int rc;

    MQTTAsync_create(&_client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    MQTTAsync_setCallbacks(_client, this, connlost, msgarrvd, NULL);


    if ((rc = MQTTAsync_connect(_client, &_connOpts)) != MQTTASYNC_SUCCESS)
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


    qDebug() << "MQQT sending" << msg;

    if ((rc = MQTTAsync_sendMessage(_client, TOPIC_SEND, &pubmsg, &opts)) != MQTTASYNC_SUCCESS)
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
