// ProjectLoader.cpp

// Implements the ProjectLoader class that can load a project's contents from a datastream
// Also implements the individual classes for each version of the saved file





#include "Globals.h"
#include "ProjectLoader.h"
#include <cstring>
#include <QIODevice>
#include "Project.h"
#include "BinaryIOStream.h"
#include "CodeLocationFactory.h"
#include "CodeLocation.h"
#include "Snapshot.h"
#include "Allocation.h"





/*
Project loading works by first checking if the file is a project file (using the magic string).
Then the file format version is read from the file and based on this version, the proper class,
ProjectLoaderV<N> is used to do the actual loading.
When a new format version is added, a new ProjectLoaderV<N> class is implemented (possibly based on
a previous format class).
*/





////////////////////////////////////////////////////////////////////////////////
// ProjectLoaderV0:

/** Loads the version 0 of the project format. */
class ProjectLoaderV0
{
public:
	static ProjectPtr loadProject(QIODevice & a_IODevice)
	{
		BinaryIOStream s(a_IODevice);
		auto res = std::make_shared<Project>();
		res->setCommand(s.readString());
		res->setTimeUnit(s.readString());
		readCodeLocations(s, *(res->getCodeLocationFactory()));
		readSnapshots(s, *res);
		return res;
	}

protected:

	static void readCodeLocations(BinaryIOStream & a_IOS, CodeLocationFactory & a_CodeLocations)
	{
		auto numCLs = a_IOS.readUInt64();
		if (numCLs >= std::numeric_limits<quint32>::max())
		{
			throw ProjectLoadException("Failed sanity check on CodeLocation count");
		}
		for (auto i = numCLs; i > 0; --i)
		{
			bool unused;
			auto cl = a_CodeLocations.getCodeLocation(a_IOS.readUInt64(), unused);
			cl->setFunctionName(a_IOS.readQString());
			cl->setFileName(a_IOS.readQString());
			cl->setFileLineNum(a_IOS.readUInt32());
		}
	}


	static void readSnapshots(BinaryIOStream & a_IOS, Project & a_Project)
	{
		auto numSnapshots = a_IOS.readUInt64();
		for (auto i = numSnapshots; i > 0; --i)
		{
			auto snapshot = std::make_shared<Snapshot>();
			snapshot->setTimestamp(a_IOS.readUInt64());
			snapshot->setHeapSize(a_IOS.readUInt64());
			snapshot->setHeapExtraSize(a_IOS.readUInt64());
			if (a_IOS.readBool())
			{
				// Allocations are present in the file, read them:
				auto rootAllocation = std::make_shared<Allocation>();
				readAllocation(a_IOS, *rootAllocation, *(a_Project.getCodeLocationFactory()));
				snapshot->setRootAllocation(rootAllocation);
			}
			snapshot->updateFlatSums();
			a_Project.addSnapshot(snapshot);
		}
	}


	static void readAllocation(
		BinaryIOStream & a_IOS,
		Allocation & a_Allocation,
		CodeLocationFactory & a_CodeLocationFactory
	)
	{
		a_Allocation.setAllocationSize(a_IOS.readUInt64());
		auto address = a_IOS.readUInt64();
		if (address != 0)
		{
			bool unused;
			a_Allocation.setCodeLocation(a_CodeLocationFactory.getCodeLocation(address, unused));
		}
		auto typeInt = a_IOS.readUInt32();
		Allocation::Type type = Allocation::atUnknown;
		switch (typeInt)
		{
			case Allocation::atBelowThreshold: type = Allocation::atBelowThreshold; break;
			case Allocation::atRegular:        type = Allocation::atRegular; break;
			case Allocation::atRoot:           type = Allocation::atRoot; break;
		}
		a_Allocation.setType(type);

		auto childrenCount = a_IOS.readUInt64();
		for (auto i = childrenCount; i > 0; --i)
		{
			auto child = a_Allocation.addChild();
			readAllocation(a_IOS, *child, a_CodeLocationFactory);
		}
	}
};





////////////////////////////////////////////////////////////////////////////////
// ProjectLoader:

ProjectPtr ProjectLoader::loadProject(QIODevice & a_IODevice)
{
	// Check if the project file magic string is present at the beginning of the file:
	char fileMagic[ARRAYCOUNT(g_ProjectFileMagic)];
	auto bytesRead = a_IODevice.read(fileMagic, sizeof(fileMagic));
	if (
		(bytesRead != sizeof(fileMagic)) ||
		(std::memcmp(fileMagic, g_ProjectFileMagic, sizeof(g_ProjectFileMagic)) != 0)
	)
	{
		throw ProjectLoadException("File is not a VisualMassifDiff project file");
	}

	// Read the format version number:
	BinaryIOStream s(a_IODevice);
	auto versionNumber = s.readUInt32();

	// Load the specific version:
	switch (versionNumber)
	{
		case 0: return ProjectLoaderV0::loadProject(a_IODevice);
	}
	throw ProjectLoadException("File version is not supported");
	return nullptr;
}




