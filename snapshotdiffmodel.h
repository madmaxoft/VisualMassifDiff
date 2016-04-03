// snapshotdiffmodel.h



#ifndef SNAPSHOTDIFFMODEL_H
#define SNAPSHOTDIFFMODEL_H





#include <memory>
#include <QAbstractItemModel>





// fwd:
class SnapshotDiff;
typedef std::shared_ptr<SnapshotDiff> SnapshotDiffPtr;





class SnapshotDiffModel:
	public QAbstractItemModel
{
	typedef QAbstractItemModel Super;

	Q_OBJECT

public:

	enum
	{
		dataRoleSort = 1038,
	};

	explicit SnapshotDiffModel(SnapshotDiffPtr a_Diff);

protected:

	/** The diff represented by this model. */
	SnapshotDiffPtr m_Diff;


	// QAbstractItemModel overrides:
	virtual QVariant data(const QModelIndex & a_Index, int a_Role) const override;
	virtual Qt::ItemFlags flags(const QModelIndex & a_Index) const override;
	QVariant headerData(
		int a_Section,
		Qt::Orientation a_Orientation,
		int a_Role = Qt::DisplayRole
	) const override;
	QModelIndex index(
		int a_Row,
		int a_Column,
		const QModelIndex & a_Parent = QModelIndex()
	) const override;
	QModelIndex parent(const QModelIndex & a_Index) const override;
	int rowCount(const QModelIndex & a_Parent = QModelIndex()) const override;
	int columnCount(const QModelIndex & a_Parent = QModelIndex()) const override;
};

#endif // SNAPSHOTDIFFMODEL_H
