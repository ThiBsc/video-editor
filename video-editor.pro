#-------------------------------------------------
#
# Project created by QtCreator 2018-03-16T18:06:47
#
#-------------------------------------------------

QT       += core gui multimedia multimediawidgets printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS += -std=c++11

TARGET = video-editor
TEMPLATE = app

LIBS += -lavformat

SOURCES += src/main.cpp\
    src/mainwindow.cpp \
    src/rushlistmodel.cpp \
    src/media.cpp \
    src/videoplayer.cpp \
    src/playercontrol.cpp \
    src/mediafileinfo.cpp \
    src/actions.cpp \
    src/tracktool.cpp \
    src/track.cpp \
    src/qcustomplot.cpp

HEADERS  += src/mainwindow.h \
    src/media.h \
    src/rushlistmodel.h \
    src/videoplayer.h \
    src/playercontrol.h \
    src/mediafileinfo.h \
    src/actions.h \
    src/tracktool.h \
    src/track.h \
    src/qcustomplot.h

TRANSLATIONS = video_editor_fr.ts

FORMS    += mainwindow.ui

RESOURCES += \
    icon.qrc
