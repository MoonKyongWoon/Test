#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include "eapsimm.hpp"

namespace Ui {
	class CMainWindow;
}

class CMainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit CMainWindow(QWidget *parent = nullptr);
	~CMainWindow();
private:
	void InitUi();
private:
	Ui::CMainWindow *ui;
	CEapSimm* m_pEapSimm;
};

#endif // MAINWINDOW_HPP
