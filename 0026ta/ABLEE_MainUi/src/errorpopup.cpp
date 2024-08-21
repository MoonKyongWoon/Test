#include "errorpopup.hpp"
#include "ui_errorpopup.h"
#include "util.hpp"

CErrorPopup::CErrorPopup(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CErrorPopup)
{
    ui->setupUi(this);
	Init();
}

CErrorPopup::~CErrorPopup()
{
    delete ui;
}

void CErrorPopup::Init()
{
	QFont font;
	font.setPointSize(9);
	font.setFamily("DejaVu Sans Mono");
	ui->treeError->setFont(font);
	ui->treeError->clear();
	ui->treeError->setColumnWidth( EN_TREE_DATE, 180 );

	ui->chkNoPopup->setChecked( false );
	connect( ui->btnClear, &QPushButton::clicked, this, &CErrorPopup::SlotClearList );
	connect( ui->btnClose, &QPushButton::clicked, this, &CErrorPopup::SlotClose     );

}

void CErrorPopup::SlotAppendErrorPopup( QString msg )
{
	QString errMsg = msg;
	errMsg = errMsg.replace("\r","");
	errMsg = errMsg.replace("\n","");

	QTreeWidgetItem* top_item = new QTreeWidgetItem();

	top_item->setText( EN_TREE_DATE, UTIL::DATE::yyMMDDHHmmss() );
	top_item->setText( EN_TREE_DETAIL, errMsg );
	top_item->setTextAlignment( EN_TREE_DATE, Qt::AlignVCenter );
	top_item->setTextAlignment( EN_TREE_DETAIL, Qt::AlignVCenter );
	//top_item->setBackground( EN_TREE_DETAIL , TREE_ALARM_QCOLOR );

	ui->treeError->addTopLevelItem( top_item );
    if( !this->isVisible() )
    {
        if( !ui->chkNoPopup->isChecked() )
            this->show();
    }
}

void CErrorPopup::SlotClearList()
{
	ui->treeError->clear();
}

void CErrorPopup::SlotClose()
{
    accept();
}
