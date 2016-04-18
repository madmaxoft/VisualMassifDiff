// HistoryModelHierarchyDelegate.cpp

// Implements the HistoryModelHierarchyDelegate class representing the delegate that expands or collapses a HistoryModel in a QTreeView





#include "Globals.h"
#include "HistoryModelHierarchyDelegate.h"
#include <QApplication>
#include "HistoryModel.h"





HistoryModelHierarchyDelegate::HistoryModelHierarchyDelegate(QObject * a_Parent) :
	Super(a_Parent)
{
}





void HistoryModelHierarchyDelegate::paint(
	QPainter * a_Painter,
	const QStyleOptionViewItem & a_Option,
	const QModelIndex & a_Index
) const
{
	QStyleOptionButton button;
	button.rect = a_Option.rect;
	button.state = 0;
	if (m_CurrentIndex == a_Index)
	{
		button.state = QStyle::State_Sunken;
	}
	else
	{
		button.state |= QStyle::State_Raised;
	}
	auto model = (reinterpret_cast<const HistoryModel *>(a_Index.model()));
	if (model->canItemExpand(a_Index))
	{
		button.state |= QStyle::State_Enabled;
	}
	button.text = tr("Expand");
	QApplication::style()->drawControl(QStyle::CE_PushButton, &button, a_Painter);
}






QSize HistoryModelHierarchyDelegate::sizeHint(const QStyleOptionViewItem &, const QModelIndex &) const
{
	// We don't have a size hint
	return QSize(1, 1);
}





bool HistoryModelHierarchyDelegate::editorEvent(QEvent * a_Event, QAbstractItemModel * a_Model, const QStyleOptionViewItem & a_Option, const QModelIndex & a_Index)
{
	switch (a_Event->type())
	{
		case QEvent::MouseButtonPress:
		{
			m_CurrentIndex = a_Index;
			return true;
		}
		case QEvent::MouseButtonRelease:
		{
			m_CurrentIndex = QModelIndex();
			auto model = reinterpret_cast<HistoryModel *>(a_Model);
			if (model->canItemExpand(a_Index))
			{
				model->expandItem(a_Index);
			}
			return true;
		}
		default: break;
	}
	return Super::editorEvent(a_Event, a_Model, a_Option, a_Index);
}
