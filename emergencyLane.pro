QT       += core gui network serialport httpserver core5compat

greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QMAKE_PROJECT_DEPTH = 0

RC_ICONS = titleIcon.ico
VERSION = 2024.09.29_by:ls

SOURCES += \
    lightcontroll.cpp \
    lightcontrollworker.cpp \
    log.cpp \
    logworker.cpp \
    main.cpp \
    myhttpserver.cpp \
    producer.cpp \
    producerworker.cpp \
    screen.cpp \
    screenworker.cpp \
    widget.cpp \
    wudpserver.cpp \
    wudpserverworker.cpp

HEADERS += \
    lightCmdList.h \
    lightcontroll.h \
    lightcontrollworker.h \
    log.h \
    logworker.h \
    myhttpserver.h \
    producer.h \
    producerworker.h \
    screen.h \
    screenworker.h \
    widget.h \
    wudpserver.h \
    wudpserverworker.h

FORMS += \
    widget.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/./ -lrdkafka

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

win32: LIBS += -L$$PWD/./ -lrdkafka++

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.

RESOURCES += \
    res.qrc

win32: LIBS += -L$$PWD/./ -lCtsSdk

INCLUDEPATH += $$PWD/.
DEPENDPATH += $$PWD/.
