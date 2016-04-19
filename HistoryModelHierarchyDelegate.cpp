// HistoryModelHierarchyDelegate.cpp

// Implements the HistoryModelHierarchyDelegate class representing the delegate that expands or collapses a HistoryModel in a QTreeView





#include "Globals.h"
#include "HistoryModelHierarchyDelegate.h"
#include <QApplication>
#include <QMouseEvent>
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
	auto model = (reinterpret_cast<const HistoryModel *>(a_Index.model()));

	// Draw the "Expand" button:
	if (model->canItemExpand(a_Index))
	{
		QStyleOptionButton btnExpand;
		btnExpand.rect = getExpandButtonRect(a_Option);
		btnExpand.state = QStyle::State_Enabled;
		if (m_CurrentIndex == a_Index)
		{
			btnExpand.state = QStyle::State_Sunken;
		}
		else
		{
			btnExpand.state |= QStyle::State_Raised;
		}
		btnExpand.text = tr("Expand");
		QApplication::style()->drawControl(QStyle::CE_PushButton, &btnExpand, a_Painter);
	}

	// Draw the "Collapse" button:
	if (model->canItemCollapse(a_Index))
	{
		QStyleOptionButton btnCollapse;
		btnCollapse.rect = getCollapseButtonRect(a_Option);
		btnCollapse.state = QStyle::State_Enabled;
		if (m_CurrentIndex == a_Index)
		{
			btnCollapse.state = QStyle::State_Sunken;
		}
		else
		{
			btnCollapse.state |= QStyle::State_Raised;
		}
		btnCollapse.text = tr("Collapse");
		QApplication::style()->drawControl(QStyle::CE_PushButton, &btnCollapse, a_Painter);
	}
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
			auto evt = reinterpret_cast<QMouseEvent *>(a_Event);
			auto expandButtonRect = getExpandButtonRect(a_Option);
			auto collapseButtonRect = getCollapseButtonRect(a_Option);
			if (expandButtonRect.contains(evt->pos()))
			{
				m_MouseDownButton = mdbExpand;
			}
			else if (collapseButtonRect.contains(evt->pos()))
			{
				m_MouseDownButton = mdbCollapse;
			}
			else
			{
				m_MouseDownButton = mdbCollapse;
			}
			return true;
		}  // MouseButtonPress

		case QEvent::MouseButtonRelease:
		{
			m_CurrentIndex = QModelIndex();
			auto model = reinterpret_cast<HistoryModel *>(a_Model);
			switch (m_MouseDownButton)
			{
				case mdbExpand:
				{
					if (model->canItemExpand(a_Index))
					{
						model->expandItem(a_Index);
					}
					break;
				}
				case mdbCollapse:
				{
					if (model->canItemCollapse(a_Index))
					{
						model->collapseItem(a_Index);
					}
					break;
				}
				default: break;
			}
			return true;
		}  // MouseButtonRelease

		default: break;
	}
	return Super::editorEvent(a_Event, a_Model, a_Option, a_Index);
}





QRect HistoryModelHierarchyDelegate::getExpandButtonRect(const QStyleOptionViewItem & a_Option) const
{
	return QRect(
		a_Option.rect.left(),       // x
		a_Option.rect.top(),        // y
		a_Option.rect.width() / 2,  // width
		a_Option.rect.height()      // height
	);
}





QRect HistoryModelHierarchyDelegate::getCollapseButtonRect(const QStyleOptionViewItem & a_Option) const
{
	int mid = a_Option.rect.left() + a_Option.rect.width() / 2;
	return QRect(
		mid + 1,                          // x
		a_Option.rect.top(),              // y
		a_Option.rect.right() - mid - 1,  // width
		a_Option.rect.height()            // height
	);
}
