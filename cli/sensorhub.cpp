#include "sensorhub.h"
#include <QJsonObject>
#include <QJsonDocument>

SensorHub::SensorHub(QObject *parent) : QObject(parent)
{
    //Connect to socket signals

    //On data receive
    connect(&_socket,&QLocalSocket::readyRead,this,&SensorHub::dataReady);

    //On error
    connect(&_socket, static_cast<void (QLocalSocket::*)(QLocalSocket::LocalSocketError)>(&QLocalSocket::error),
            this,[](QLocalSocket::LocalSocketError err) {
       qDebug() << "Sensorhub Socket error" << err;
    });

    //On state change
    connect(&_socket,&QLocalSocket::stateChanged,this,[](QLocalSocket::LocalSocketState state) {
       qDebug() << "Sensorhub Socket Statechange" << state;
    });


    _socket.connectToServer("/tmp/sensor-hub.socket");
    _socket.waitForConnected();
    qDebug() << "Sensorhub Connected";
}

void SensorHub::sendMessage(const QJsonObject& obj)
{
    QJsonDocument doc;
    doc.setObject(obj);
    QByteArray arr = doc.toJson();

    qDebug() << "Sensorhub Sent" << obj;

    _socket.write(arr);
    _socket.waitForBytesWritten();
}

void SensorHub::dataReady()
{

    _unparsedBytes.append(_socket.readAll()); //append bytes from socket to bytebuffer

    while (true) { //parsing loop
        QString str(_unparsedBytes); //Create string from bytebuffer

        QString err;
        int l = SensorHub::findEndOfJson(str, &err); //Check if the string already contains a full jsonobject (=> fast string scanning)


        if (l > 0) { //json object found
            QString m = str.left(l); //Create string only with the required number of bytes
            QByteArray r = m.toUtf8();
            _unparsedBytes.remove(0, r.length()); //Remove bytes from bytebuffer

            //Try to convert String into JsonObject (=> real parsing happens)
            QJsonParseError scanErr;
            QJsonDocument doc = QJsonDocument::fromJson(r,&scanErr);
            if(scanErr.error!=QJsonParseError::NoError) { //Parsing error occured
                qDebug() << scanErr.errorString();
                qDebug() << QString(r);
                continue;
            }

            QJsonObject obj = doc.object();
            qDebug() << "Sensorhub Received" << obj;

            emit messageReceived(obj);
        } else {
            if (l < 0) { //fatal parser error
                qDebug() << err;
            } else { //l==0
                //No object found, waiting on more data
            }

            break;
        }
    }

}


int SensorHub::findEndOfJson(QString inp, QString* error)
{
    static QString e2;

    if (error == nullptr) {
        error = &e2;
    }

    *error = "";

    QList<bool> bracketStack; //a value of true means=open curly braket, false = open normal braket
    bool inStringLiteral = false;
    bool firstChar = true;
    bool isEscaped = false;
    QChar quoteChar;

    for (int i = 0; i < inp.length(); i++) { //foreach char in string
        QChar c = inp.at(i);

        if (inStringLiteral) { //currently in a string literal: ignore everything expected closing quote
            if (isEscaped) {
                isEscaped = false;
                continue;
            } else if (c == '\\') {
                isEscaped = true;
                continue;
            }

            if (c != quoteChar) { //not a unespaced, closing double qoute
                continue;
            }

            inStringLiteral = false;
        } else if (c == '"' || c == '\'') { //start of a string literal
            inStringLiteral = true;
            quoteChar = c; //remember if we have double quotes or not
            continue;
        }

        if (c == ' ' || c == '\r' || c == '\n' || c == '\t') { //whitespace
            continue;
        }

        if (c == '{') { //start of a json object
            firstChar = false;
            bracketStack.append(true);
        } else if (c == '[') { //start of a json array
            firstChar = false;
            bracketStack.append(false);
        } else if (!firstChar) { //we are already in a json object/array
            if (c == '}') { //closing json obj
                if (bracketStack.isEmpty() || !bracketStack.takeLast()) {
                    *error = "Unexpected '}'";
                    return -1;
                }

                if (bracketStack.isEmpty()) {
                    return i + 1;
                }
            } else if (c == ']') { //closing json array
                if (bracketStack.isEmpty() || bracketStack.takeLast()) {
                    *error = "Unexpected ']'";
                    return -1;
                }

                if (bracketStack.isEmpty()) {
                    return i + 1;
                }
            }
        } else { //first non whitespace character was not '{' or '['
            QString before = inp.mid(qMin(0, i - 100), qMin(i, 100) + 1);
            QString after = inp.mid(i, 100);
            *error = QString("Error while parsing JSON: Unexpected character ('%1') in input. Not an Object or Array. Before:\n%2\nAfter:\n%3")
                     .arg(c).arg(before).arg(after);
            return -1;
        }
    }

    *error = "No complete json object/array found";
    return 0;
}
