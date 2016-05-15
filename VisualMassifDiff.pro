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
	CodeLocationStats.cpp \
	CodeLocationStatsModel.cpp \
	AllocationPath.cpp \
	HistoryModel.cpp \
	HistoryModelHierarchyDelegate.cpp \
	BinaryIOStream.cpp \
	ProjectSaver.cpp \
	ProjectLoader.cpp \
	DlgLiveCaptureSettings.cpp \
	VgdbComm.cpp \
	ProcessReader.cpp \
	DlgLiveCapture.cpp \
	LiveCapture.cpp \
	SnapshotModel.cpp

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
	HistoryModelHierarchyDelegate.h \
	BinaryIOStream.h \
	ProjectSaver.h \
	ProjectLoader.h \
	LiveCaptureSettings.h \
	DlgLiveCaptureSettings.h \
	VgdbComm.h \
	ProcessReader.h \
	DlgLiveCapture.h \
	LiveCapture.h \
	SnapshotModel.h

FORMS += \
	MainWindow.ui \
	DlgSnapshotDetails.ui \
	DlgSnapshotDiffs.ui \
	DlgLiveCaptureSettings.ui \
	DlgLiveCapture.ui

OTHER_FILES += \
	.gitignore \
	README.md

RESOURCES += \
	res/VisualMassifDiff.qrc

CONFIG += C++11

