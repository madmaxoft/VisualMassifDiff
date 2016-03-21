#-------------------------------------------------
#
# Project created by QtCreator 2016-03-14T15:44:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisualMassifDiff
TEMPLATE = app


SOURCES += main.cpp\
		mainwindow.cpp \
    project.cpp \
    snapshot.cpp \
    allocation.cpp \
    massifparser.cpp \
    dlgsnapshotdetails.cpp \
    allocationsgraph.cpp \
    formatnumber.cpp \
    snapshotsgraph.cpp

HEADERS  += mainwindow.h \
    project.h \
    snapshot.h \
    allocation.h \
    massifparser.h \
    parseinteger.h \
    dlgsnapshotdetails.h \
    allocationsgraph.h \
    formatnumber.h \
    globals.h \
    snapshotsgraph.h

FORMS    += mainwindow.ui \
    dlgsnapshotdetails.ui

OTHER_FILES += \
    .gitignore \
    README.md

RESOURCES += \
    VisualMassifDiff.qrc
