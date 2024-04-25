#-------------------------------------------------
#
# Project created by QtCreator 2016-01-16T16:21:05
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = WordLink
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp \
    helpdialog.cpp \
    analysisdialog.cpp

HEADERS  += mainwindow.h \
    utility.h \
    qcustomplot.h \
    helpdialog.h \
    analysisdialog.h

FORMS    += mainwindow.ui \
    helpdialog.ui \
    analysisdialog.ui

#macx:ICON = $${PWD}/Icon.ico
RC_ICONS = Icon.ico

# for JSON parsing
INCLUDEPATH += rapidjson

RESOURCES += \
    wordlink.qrc

# enable c++11, to use to_string function
QMAKE_CXXFLAGS += -std=c++0x

DISTFILES +=


