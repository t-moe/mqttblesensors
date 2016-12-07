QT += core network
QT -= gui

CONFIG += c++11

TARGET = cli
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  main.cpp \
            sensorhub.cpp \
            mqtt.cpp \
            controller.cpp

HEADERS +=  sensorhub.h \
            mqtt.h \
            controller.h

target.path = /home/pi
INSTALLS+= target

#QMAKE_LFLAGS+=-lpthread


#LIBS += -lpthread  #-lrt
LIBS += -L$$PWD/paho-1.0.3-Linux/lib/static/ -lpaho-mqtt3a_static
INCLUDEPATH  += $$PWD/paho-1.0.3-Linux/include
DEPENDPATH  += $$PWD/paho-1.0.3-Linux/include

