QT += core network
QT -= gui

CONFIG += c++11

TARGET = cli
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    sensorhub.cpp

target.path = /home/pi
INSTALLS+= target

#QMAKE_LFLAGS+=-lpthread

HEADERS += \
    sensorhub.h

