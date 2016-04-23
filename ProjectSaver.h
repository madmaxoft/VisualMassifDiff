// ProjectSaver.h

// Declares the ProjectSaver class that can save a project's contents to a datastream





#ifndef PROJECTSAVER_H
#define PROJECTSAVER_H





#include "BinaryIOStream.h"





class QIODevice;
class Project;
class CodeLocationFactory;
class Snapshot;
class Allocation;





class ProjectSaver
{
public:
	static void saveProject(const Project & a_Project, QIODevice & a_IODevice);

protected:
	/** The IO device to which all the data is sent. */
	QIODevice & m_IODevice;

	/** Helper object over m_IODevice that can write binary data in a platform-independent manner. */
	BinaryIOStream m_IOS;


	ProjectSaver(QIODevice & a_IODevice);

	void saveProject(const Project & a_Project);
	void saveCodeLocations(const CodeLocationFactory & a_CodeLocationFactory);
	void saveSnapshot(const Snapshot & a_Snapshot);
	void saveAllocation(const Allocation & a_Allocation);
};





#endif // PROJECTSAVER_H




