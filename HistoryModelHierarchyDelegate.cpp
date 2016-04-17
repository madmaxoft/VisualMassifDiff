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
	button.state = QStyle::State_Enabled;
	if (m_CurrentIndex == a_Index)
	{
		button.state = QStyle::State_Sunken;
	}
	else
	{
		button.state |= QStyle::State_Raised;
	}
	button.text = tr("Expand");
	QApplication::style()->drawControl(QStyle::CE_PushButton, &button, a_Painter);
}






QSize HistoryModelHierarchyDelegate::sizeHint(const QStyleOptionViewItem & a_Option, const QModelIndex & a_Index) const
{
	// TODO
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
			(reinterpret_cast<HistoryModel *>(a_Model))->expandItem(a_Index);
			return true;
		}
		default: break;
	}
	return Super::editorEvent(a_Event, a_Model, a_Option, a_Index);
}
