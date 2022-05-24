QT -= gui

CONFIG += c++11 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        drawtree.cpp \
        main.cpp \
        mainwindow.cpp \
        rangetree.cpp

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    drawtree.h \
    mainwindow.h \
    rangetree.h

DISTFILES += \
    test.json \
    test2.json \
    test3.json \
    test4.json \
    test5.json \
    test6.json \
    test7.json \
    test8.json

QT += widgets
