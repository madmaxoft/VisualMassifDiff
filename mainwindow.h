// mainwindow.h

// Declares the MainWindow class representing the app's main UI window

#ifndef MAINWINDOW_H
#define MAINWINDOW_H





#include <memory>
#include <QMainWindow>





// fwd:
class Project;
typedef std::shared_ptr<Project> ProjectPtr;
class Snapshot;
typedef std::shared_ptr<Snapshot>SnapshotPtr;
class QTreeWidgetItem;





namespace Ui
{
	class MainWindow;
}





class MainWindow:
	public QMainWindow
{
	Q_OBJECT

public:

	explicit MainWindow(QWidget * parent = nullptr);

	~MainWindow();


public slots:

	/** Opens a file dialog to choose file, then adds the file contents as new snapshots. */
	void addSnapshotsFromFile(void);

	/** Adds snapshots from the specified file into the project. */
	void addSnapshotsFromFile(const QString & a_FileName);

	/** Displays an error message containing the error. */
	void parseError(quint32 a_LineNum, const char * a_Msg, const char * a_Line);

	/** Adds the specified new snapshot to current project. */
	void newSnapshotParsed(SnapshotPtr a_Snapshot);

	/** The command used for creating the Massif file has been parsed.
	Checks with the project if it is the same, aborts parse if not. */
	void parsedCommand(const char * a_Command);

	/** The time unit used for creating the Massif file has been parsed.
	Checks with the project if it is the same, aborts parse if not. */
	void parsedTimeUnit(const char * a_TimeUnit);

private:

	/** UI, managed by QTCreator */
	Ui::MainWindow * m_UI;

	/** Current project. */
	ProjectPtr m_Project;

	/**Icon displayed in the tree view if the snapshot has detailed allocations attached to it.  */
	QIcon m_IcoAllocations;


	/** Creates a treeview item for the specified new snapshot.
	The item is not inserted into any treewidget. */
	QTreeWidgetItem * createSnapshotTreeItem(SnapshotPtr a_Snapshot);

};





#endif // MAINWINDOW_H




