#include "slotview.hpp"
#include "ui_slotview.h"

CSlotView::CSlotView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSlotView)
{
	ui->setupUi(this);
}

CSlotView::~CSlotView()
{
	delete ui;
}
