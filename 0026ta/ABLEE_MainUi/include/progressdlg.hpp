#ifndef PROGRESSDLG_HPP
#define PROGRESSDLG_HPP

#include <QDialog>
#include <QProgressBar>
#include "sysdef.hpp"

enum EN_PROGRESSDLG_COL
{
	COL_SLOT       ,
	COL_PROGRESSBAR,
	COL_TEXT       ,
};

#define PASS_FOREGROUND				QColor(  0,  0,  0)
#define FAIL_FOREGROUND				QColor(255,  0,  0)
#define PASS_BACKGROUND				QColor(255,255,255)
#define FAIL_BACKGROUND				QColor(255,  0,  0)

namespace Ui {
	class CProgressDlg;
}

class CProgressDlg : public QDialog
{
	Q_OBJECT

public:
	explicit CProgressDlg(QWidget *parent = nullptr);
	~CProgressDlg();
protected:
	void resizeEvent(QResizeEvent *event);
private:
	void InitUi();
	void CreateProgressView();
	void InitPrgTable();
	void InitProgress();
	void InitStatusText();
public:
	void ResizeView();
public slots:
	void SlotProgressDlgTitle(QString);
	void SlotProgressDlgInit();
	void SlotProgressDlgValue( int, int);
	void SlotProgressDlgText ( int, QString, bool error);
	void SlotProgressDlgHide();
private:
	Ui::CProgressDlg *ui;
	QProgressBar* m_prgBar[MAX_SLOT];
};

#endif // PROGRESSDLG_HPP
