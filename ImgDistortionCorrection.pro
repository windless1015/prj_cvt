#-------------------------------------------------
#
# Project created by QtCreator 2017-09-06T17:02:34
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ImgDistortionCorrection
TEMPLATE = app

INCLUDEPATH += E:\lib\Eigen
INCLUDEPATH+=D:/lib/lib_src/opencv/build/include

LIBS+=D:\lib\lib_src\opencv\build\x86\vc11\lib\opencv_core2413d.lib
LIBS+=D:\lib\lib_src\opencv\build\x86\vc11\lib\opencv_imgproc2413d.lib
LIBS+=D:\lib\lib_src\opencv\build\x86\vc11\lib\opencv_highgui2413d.lib


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui
