#include "systemview.hpp"
#include "ui_systemview.h"

CSystemView::CSystemView(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSystemView)
{
	ui->setupUi(this);
}

CSystemView::~CSystemView()
{
	delete ui;
}
