#include "mainwindow.hpp"
#include "ui_mainwindow.h"

CMainWindow::CMainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CMainWindow)
{
	ui->setupUi(this);
	InitUi();
}

CMainWindow::~CMainWindow()
{
	delete ui;
}

void CMainWindow::InitUi()
{
	m_pEapSimm = new CEapSimm(this);
	setCentralWidget( m_pEapSimm );
}
