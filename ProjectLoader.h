// ProjectLoader.h

// Declares the ProjectLoader class that can load a project's contents from a datastream





#ifndef PROJECTLOADER_H
#define PROJECTLOADER_H





#include <memory>
#include <stdexcept>





class QIODevice;
class Project;
typedef std::shared_ptr<Project> ProjectPtr;





class ProjectLoadException:
	public std::runtime_error
{
	typedef std::runtime_error Super;

public:
	ProjectLoadException(const char * a_Reason):
		Super(a_Reason)
	{
	}
};





class ProjectLoader
{
public:
	/** Returns the new loaded project.
	Throws on failure. */
	static ProjectPtr loadProject(QIODevice & a_Device);

	/** Returns whether the specified file is a VisualMassifDiff project file. */
	static bool isProjectFile(QIODevice & a_IODevice);
};





#endif // PROJECTLOADER_H




