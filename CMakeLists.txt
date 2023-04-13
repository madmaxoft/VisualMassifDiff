cmake_minimum_required (VERSION 3.1)

project (VisualMassifDiff LANGUAGES CXX)

set (CMAKE_CXX_STANDARD 11)

find_package (Qt5 5.8 COMPONENTS Widgets REQUIRED)





if (MSVC)
	# Make build use multiple threads under MSVC:
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP")

	# Make release builds use link-time code generation:
	set (CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /GL")
	set (CMAKE_C_FLAGS_RELEASE   "${CMAKE_C_FLAGS_RELEASE}   /GL")
	set (CMAKE_EXE_LINKER_FLAGS_RELEASE    "${CMAKE_EXE_LINKER_FLAGS_RELEASE}    /LTCG")
	set (CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /LTCG")
	set (CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} /LTCG")
	set (CMAKE_MODULE_LINKER_FLAGS_RELEASE "${CMAKE_MODULE_LINKER_FLAGS_RELEASE} /LTCG")
endif()





set (SOURCES
	Allocation.cpp
	AllocationPath.cpp
	AllocationsGraph.cpp
	BinaryIOStream.cpp
	CodeLocation.cpp
	CodeLocationFactory.cpp
	CodeLocationStats.cpp
	CodeLocationStatsModel.cpp
	DlgLiveCapture.cpp
	DlgLiveCaptureSettings.cpp
	DlgSnapshotDetails.cpp
	DlgSnapshotDiffs.cpp
	FormatNumber.cpp
	HistoryGraph.cpp
	HistoryModel.cpp
	HistoryModelHierarchyDelegate.cpp
	LiveCapture.cpp
	Main.cpp
	MainWindow.cpp
	MassifParser.cpp
	ProcessReader.cpp
	Project.cpp
	ProjectLoader.cpp
	ProjectSaver.cpp
	Snapshot.cpp
	SnapshotDiff.cpp
	SnapshotDiffModel.cpp
	SnapshotModel.cpp
	VgdbComm.cpp
)


SET (HEADERS
	Allocation.h
	AllocationPath.h
	AllocationsGraph.h
	AllocationStats.h
	BinaryIOStream.h
	CodeLocation.h
	CodeLocationFactory.h
	CodeLocationStats.h
	CodeLocationStatsModel.h
	DlgLiveCapture.h
	DlgLiveCaptureSettings.h
	DlgSnapshotDetails.h
	DlgSnapshotDiffs.h
	FormatNumber.h
	Globals.h
	HistoryGraph.h
	HistoryModel.h
	HistoryModelHierarchyDelegate.h
	LiveCapture.h
	LiveCaptureSettings.h
	MainWindow.h
	MassifParser.h
	ParseInteger.h
	ProcessReader.h
	Project.h
	ProjectLoader.h
	ProjectSaver.h
	Snapshot.h
	SnapshotDiff.h
	SnapshotDiffModel.h
	SnapshotModel.h
	VgdbComm.h
)





set (UI
	DlgLiveCapture.ui
	DlgLiveCaptureSettings.ui
	DlgSnapshotDetails.ui
	DlgSnapshotDiffs.ui
	MainWindow.ui
)

set (RESOURCES
	res/VisualMassifDiff.qrc
)





# Tell CMake to run moc / uic / rcc when necessary:
set (CMAKE_AUTOMOC ON)
set (CMAKE_AUTOUIC ON)
set (CMAKE_AUTORCC ON)

# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set (CMAKE_INCLUDE_CURRENT_DIR ON)




# Set up extra compiler warnings:
if (MSVC)
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX")
else ()
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wunknown-pragmas")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wundef")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wold-style-cast")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdisabled-optimization")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Winit-self")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpointer-arith")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wswitch")
endif ()





add_definitions(-DQT_DEPRECATED_WARNINGS)




# Enable message log context information even in Release builds:
add_definitions(-DQT_MESSAGELOGCONTEXT)





add_executable (VisualMassifDiff WIN32 MACOSX_BUNDLE ${SOURCES} ${HEADERS} ${UI} ${RESOURCES} ${QMFILES})

target_link_libraries (${PROJECT_NAME}
	Qt5::Widgets
	${ADDITIONAL_LIBRARIES}
)
