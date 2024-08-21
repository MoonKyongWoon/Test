#ifndef PASSDLG_HPP
#define PASSDLG_HPP

#include <QDialog>
#include <QPushButton>
#include <QMessageBox>

namespace Ui {
	class CPassDlg;
}

enum PASSWD_MODE
{
	PASSWD_DC_ON,
	PASSWD_DC_OFF,
};

#define PASSWORD			"ablee"

class CPassDlg : public QDialog
{
	Q_OBJECT

public:
	explicit CPassDlg(QWidget *parent = nullptr);
	~CPassDlg();
public slots:
	void SlotCheckCklicked();
signals:
	void SigAccepted(PASSWD_MODE);
public:
	void SetMode(PASSWD_MODE);
private:
	void Init();
private:
	PASSWD_MODE m_mode;
	Ui::CPassDlg *ui;
};

#endif // PASSDLG_HPP
