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
	Main.cpp \
	MainWindow.cpp \
	Project.cpp \
	Snapshot.cpp \
	Allocation.cpp \
	MassifParser.cpp \
	DlgSnapshotDetails.cpp \
	AllocationsGraph.cpp \
	FormatNumber.cpp \
	HistoryGraph.cpp \
	CodeLocation.cpp \
	CodeLocationFactory.cpp \
	SnapshotDiff.cpp \
	DlgSnapshotDiffs.cpp \
	SnapshotDiffModel.cpp \
	CodelocationStats.cpp \
	CodelocationStatsModel.cpp \
	AllocationPath.cpp \
	HistoryModel.cpp \
	HistoryModelHierarchyDelegate.cpp

HEADERS  += \
	Project.h \
	Snapshot.h \
	Allocation.h \
	MassifParser.h \
	ParseInteger.h \
	DlgSnapshotDetails.h \
	AllocationsGraph.h \
	FormatNumber.h \
	Globals.h \
	HistoryGraph.h \
	CodeLocation.h \
	CodeLocationFactory.h \
	SnapshotDiff.h \
	DlgSnapshotDiffs.h \
	SnapshotDiffModel.h \
	CodeLocationStats.h \
	CodeLocationStatsModel.h \
	MainWindow.h \
	AllocationPath.h \
	HistoryModel.h \
	AllocationStats.h \
	HistoryModelHierarchyDelegate.h

FORMS += \
	MainWindow.ui \
	DlgSnapshotDetails.ui \
	DlgSnapshotDiffs.ui

OTHER_FILES += \
	.gitignore \
	README.md

RESOURCES += \
	VisualMassifDiff.qrc

CONFIG += C++11

