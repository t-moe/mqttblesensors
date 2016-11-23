#ifndef SENSORHUB_H
#define SENSORHUB_H

#include <QObject>
#include <QLocalSocket>
#include <QJsonObject>

class SensorHub : public QObject
{
    Q_OBJECT
public:
    explicit SensorHub(QObject *parent = 0);
    void send(const QJsonObject& obj);

signals:
    void eventReceived(QJsonObject obj);

public slots:

private slots:
    void dataReady();

private:
    QLocalSocket _socket;
    QByteArray _unparsedBytes;
    int findEndOfJson(QString inp, QString* error);
};

#endif // SENSORHUB_H
