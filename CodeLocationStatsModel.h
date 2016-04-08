// CodeLocationStatsModel.h

// Declares the CodeLocationStatsModel class representing the model for visualising CodeLocationStats





#ifndef CODELOCATIONSTATSMODEL_H
#define CODELOCATIONSTATSMODEL_H





#include <memory>
#include <QAbstractTableModel>
#include "CodeLocationStats.h"





// fwd:
class Project;
typedef std::shared_ptr<Project> ProjectPtr;





class CodeLocationStatsModel:
	public QAbstractTableModel
{
	typedef QAbstractTableModel Super;

	Q_OBJECT

public:

	enum
	{
		SortRole = Qt::UserRole + 1,
	};


	explicit CodeLocationStatsModel(ProjectPtr a_Project);

	/** Called after a new snapshot has been added to the project.
	Resets and rebuilds the model. */
	void addedSnapshot();

signals:

public slots:

protected:

	/** The project to which the model belongs. */
	ProjectPtr m_Project;

	/** Local copy of all the stats. */
	std::vector<CodeLocationStats::Stats> m_Stats;


	/** Resets and rebuilds the model. */
	void rebuildModel();

	// QAbstractTableModel overrides:
	virtual int rowCount(const QModelIndex & a_Parent = QModelIndex()) const override;
	virtual int columnCount(const QModelIndex & a_Parent = QModelIndex()) const override;
	virtual QVariant data(const QModelIndex & a_Index, int a_Role) const override;
	virtual QVariant headerData(int a_Section, Qt::Orientation a_Orientation, int a_Role = Qt::DisplayRole) const override;
};





#endif // CODELOCATIONSTATSMODEL_H




