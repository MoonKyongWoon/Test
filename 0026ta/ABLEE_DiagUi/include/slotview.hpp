#ifndef SLOTVIEW_HPP
#define SLOTVIEW_HPP

enum ITEM_TREE
{
	ITEM_SEL   = 0x0,
	ITEM_SEQ        ,
	ITEM_NAME       ,
	ITEM_TYPE       ,
};

#include <QWidget>

namespace Ui {
	class CSlotView;
}

class CSlotView : public QWidget
{
	Q_OBJECT

public:
	explicit CSlotView(QWidget *parent = nullptr);
	~CSlotView();

private:
	Ui::CSlotView *ui;
};

#endif // SLOTVIEW_HPP
