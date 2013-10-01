#-------------------------------------------------
#
# Project created by QtCreator 2013-08-25T13:55:13
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = distroVoronoi
TEMPLATE = app


SOURCES += main.cpp\
        vorowindow.cpp \
    server.cpp \
    point.cpp \
    quadwindow.cpp \
    client.cpp \
    ConvexHullAlgs.cpp \
    Vector.cpp \
    VoronoiDiagramGenerator.cpp \
    voronoidisplayer.cpp

HEADERS  += vorowindow.h \
    server.h \
    point.h \
    quadwindow.h \
    client.h \
    ConvexHullAlgs.h \
    Vector.h \
    VoronoiDiagramGenerator.h \
    voronoidisplayer.h

FORMS    += vorowindow.ui \
    quadwindow.ui \
    voronoidisplayer.ui
