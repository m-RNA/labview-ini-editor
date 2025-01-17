QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
greaterThan(QT_MAJOR_VERSION, 5): QT += core5compat

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH +=  \
    Message

SOURCES += \
    inisettings.cpp \
    labviewsetting.cpp \
    main.cpp \
    mainwindow.cpp \
    Message/message.cpp \
    test_item_interface.cpp \
    test_result_interface.cpp

HEADERS += \
    inisettings.h \
    labviewsetting.h \
    mylistwidget.h \
    mainwindow.h \
    Message/message.h \
    test_item_interface.h \
    test_result_interface.h

FORMS += \
    mainwindow.ui \
    test_item_interface.ui \
    test_result_interface.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    Resource/main.qrc\
    Message/Message.qrc 

RC_ICONS = Resource/file_type_qml.ico
