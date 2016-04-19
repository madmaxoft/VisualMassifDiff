// HistoryModelHierarchyDelegate.h

// Declares the HistoryModelHierarchyDelegate class representing the delegate that expands or collapses a HistoryModel in a QTreeView





#ifndef HISTORYMODELDELEGATE_H
#define HISTORYMODELDELEGATE_H





#include <QStyledItemDelegate>





class HistoryModelHierarchyDelegate:
	public QStyledItemDelegate
{
	typedef QStyledItemDelegate Super;

	Q_OBJECT

public:

	explicit HistoryModelHierarchyDelegate(QObject * a_Parent);

signals:

public slots:

protected:
	/** Specifies the index of the model where mouse button is currently pressed.
	Empty index when mouse button is released. */
	QModelIndex m_CurrentIndex;

	/** The button on which the last MouseDown event occured. */
	enum
	{
		mdbNone,      ///< Not on any button
		mdbExpand,    ///< On the Expand button
		mdbCollapse,  ///< On the Collapse button
	} m_MouseDownButton;


	virtual void paint(
		QPainter * a_Painter,
		const QStyleOptionViewItem & a_Option,
		const QModelIndex & a_Index
	) const override;

	virtual QSize sizeHint(
		const QStyleOptionViewItem & a_Option,
		const QModelIndex & a_Index
	) const override;

	virtual bool editorEvent(
		QEvent * a_Event,
		QAbstractItemModel * a_Model,
		const QStyleOptionViewItem & a_Option,
		const QModelIndex & a_Index
	) override;


	/** Returns the bounding box of the Expand button. */
	QRect getExpandButtonRect(const QStyleOptionViewItem & a_Option) const;

	/** Returns the bounding box of the Collapse button. */
	QRect getCollapseButtonRect(const QStyleOptionViewItem & a_Option) const;
};





#endif // HISTORYMODELDELEGATE_H




