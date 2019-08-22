#-------------------------------------------------
#
# Project created by QtCreator 2019-06-14T12:00:00
#
#-------------------------------------------------

QT      += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
#QT       += axcontainer
QT      += charts

#QT      += network

## QCamera
#QT       += multimedia

##QCameraViewfinder
#QT       += multimediawidgets

TARGET = CameraTest
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

#DEFINES += QT_NO_DEBUG_OUTPUT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11
#CONFIG += console

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    myvariable.cpp \
    halcon/myhalcon.cpp \
    opencv/myopencv.cpp \
    thread/imgacqthread.cpp \
    highaccuracytimer.cpp \
    capture/HalconCamera.cpp \
    capture/WebCamera.cpp

HEADERS += \
    mainwindow.h \
    opencv/myopencv.h \
    myvariable.h \
    halcon/myhalcon.h \
    thread/imgacqthread.h \
    highaccuracytimer.h \
    capture/HalconCamera.h \
    capture/WebCamera.h

FORMS += \
    mainwindow.ui

CONFIG += debug_and_release

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#LIBS += Packet.lib wpcap.lib
#LIBS += glu32.lib opengl32.lib
unix:LIBS += -lX11

# Halcon
win32 {
INCLUDEPATH += "$$(HALCONROOT)/include"
INCLUDEPATH += "$$(HALCONROOT)/include/HalconCpp"
QMAKE_LIBDIR += "$$(HALCONROOT)/lib/$$(HALCONARCH)"
LIBS += halconcpp.lib halcon.lib
}

unix {
INCLUDEPATH += "$$(HALCONROOT)/include"
INCLUDEPATH += "$$(HALCONROOT)/include/halconcpp"
LIBS += -L$$(HALCONROOT)/lib/$$(HALCONARCH) -lhalconcpp -lhalcon
}

# OpenCV
win32 {
INCLUDEPATH += "D:/OpenCV/opencv-4.0.0/install/include"
QMAKE_LIBDIR += "D:/OpenCV/opencv-4.0.0/install/x64/vc15/lib"
CONFIG(debug, debug|release){LIBS += opencv_world400d.lib}
else{LIBS += opencv_world400.lib}
}

unix{
INCLUDEPATH += /usr/local/include
LIBS += -L/usr/local/lib -lopencv_core -lopencv_imgproc -lopencv_highgui
}



RESOURCES += InitializationPicture.qrc
