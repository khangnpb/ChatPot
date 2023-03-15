QT       += core gui
QT    += network

#LIBS += C:\Users\Admin\AppData\Local\Programs\Python\Python311\libs\python311.lib

#INCLUDEPATH = C:\Users\Admin\AppData\Local\Programs\Python\Python311\include\ C:\Users\Admin\AppData\Local\Programs\Python\Python311\libs\

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resource.qrc
