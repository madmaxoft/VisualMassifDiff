// ProjectSaver.cpp

// Declares the ProjectSaver class that can save a project's contents to a datastream






#include "Globals.h"
#include "ProjectSaver.h"
#include "Project.h"
#include "CodeLocationFactory.h"
#include "Snapshot.h"
#include "Allocation.h"
#include "BinaryIOStream.h"





ProjectSaver::ProjectSaver(QIODevice & a_IODevice):
	m_IODevice(a_IODevice),
	m_IOS(a_IODevice)
{
}





void ProjectSaver::saveProject(const Project & a_Project, QIODevice & a_IODevice)
{
	ProjectSaver s(a_IODevice);
	s.saveProject(a_Project);
}





void ProjectSaver::saveProject(const Project & a_Project)
{
	// Write the file header: magic and version:
	m_IOS.writeConst(g_ProjectFileMagic);
	m_IOS.writeUInt32(0);

	// Write settings:
	m_IOS.writeString(a_Project.getCommand());
	m_IOS.writeString(a_Project.getTimeUnit());

	// Write code locations:
	saveCodeLocations(*(a_Project.getCodeLocationFactory()));

	// Write snapshots:
	m_IOS.writeUInt64(a_Project.getNumSnapshots());
	for (const auto & s: a_Project.getSnapshots())
	{
		saveSnapshot(*s);
	}  // for s - m_Snapshots[]
}





void ProjectSaver::saveCodeLocations(const CodeLocationFactory & a_CodeLocationFactory)
{
	const auto & codeLocations = a_CodeLocationFactory.getAllCodeLocations();
	m_IOS.writeUInt64(codeLocations.size());
	for (const auto & l: codeLocations)
	{
		const auto & cl = l.second;
		m_IOS.writeUInt64(cl->getAddress());
		m_IOS.writeString(cl->getFunctionName());
		m_IOS.writeString(cl->getFileName());
		m_IOS.writeUInt32(cl->getFileLineNum());
	}
}





void ProjectSaver::saveSnapshot(const Snapshot & a_Snapshot)
{
	m_IOS.writeUInt64(a_Snapshot.getTimestamp());
	m_IOS.writeUInt64(a_Snapshot.getHeapSize());
	m_IOS.writeUInt64(a_Snapshot.getHeapExtraSize());
	m_IOS.writeBool(a_Snapshot.hasAllocations());
	if (a_Snapshot.hasAllocations())
	{
		saveAllocation(*(a_Snapshot.getRootAllocation()));
	}
}





void ProjectSaver::saveAllocation(const Allocation & a_Allocation)
{
	m_IOS.writeUInt64(a_Allocation.getAllocationSize());
	const auto cl = a_Allocation.getCodeLocation();
	auto address = (cl == nullptr) ? 0 : cl->getAddress();
	m_IOS.writeUInt64(address);
	quint32 type = 0;
	switch (a_Allocation.getType())
	{
		case Allocation::atBelowThreshold: type = 1; break;
		case Allocation::atRegular:        type = 2; break;
		case Allocation::atRoot:           type = 3; break;
		case Allocation::atUnknown:        type = 0; break;
	}
	m_IOS.writeUInt32(type);
	const auto & children = a_Allocation.getChildren();
	m_IOS.writeUInt64(children.size());
	for (const auto & ch: children)
	{
		saveAllocation(*ch);
	}
}




