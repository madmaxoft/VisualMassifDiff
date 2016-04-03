#-------------------------------------------------
#
# Project created by QtCreator 2016-03-14T15:44:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = VisualMassifDiff
TEMPLATE = app


SOURCES += \
	main.cpp \
	mainwindow.cpp \
	project.cpp \
	snapshot.cpp \
	allocation.cpp \
	massifparser.cpp \
	dlgsnapshotdetails.cpp \
	allocationsgraph.cpp \
	formatnumber.cpp \
	snapshotsgraph.cpp \
	codelocation.cpp \
	codelocationfactory.cpp \
	snapshotdiff.cpp \
	dlgsnapshotdiffs.cpp \
	snapshotdiffmodel.cpp

HEADERS  += \
	mainwindow.h \
	project.h \
	snapshot.h \
	allocation.h \
	massifparser.h \
	parseinteger.h \
	dlgsnapshotdetails.h \
	allocationsgraph.h \
	formatnumber.h \
	globals.h \
	snapshotsgraph.h \
	codelocation.h \
	codelocationfactory.h \
	snapshotdiff.h \
	dlgsnapshotdiffs.h \
	snapshotdiffmodel.h

FORMS += \
	mainwindow.ui \
	dlgsnapshotdetails.ui \
	dlgsnapshotdiffs.ui

OTHER_FILES += \
	.gitignore \
	README.md

RESOURCES += \
	VisualMassifDiff.qrc

CONFIG += C++11

