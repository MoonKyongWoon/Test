#ifndef CHMBDLG_HPP
#define CHMBDLG_HPP

#include <QDialog>
#include <QThread>
#include "chmbdlgcontroller.hpp"

namespace Ui {
	class CChmbDlg;
}

class CChmbDlg : public QDialog
{
	Q_OBJECT

public:
	explicit CChmbDlg(QWidget *parent = nullptr);
	~CChmbDlg();
private:
	void InitVars();
	void InitUi();
private:
	void InitRequest();
	void BuildRequest(EN_CHKECK_SEQ);
	void CheckStart();
	void CheckDone( EN_CHKECK_SEQ, bool );
	void UpdateSequence( EN_CHKECK_SEQ, bool );
public slots:
	void SlotInitChmbDlg();
	void SlotCheckStart();
public slots:
	void SlotBtnSimmClicked();
	void SlotBtnCloseClicked();
	void SlotControllerSeqDone(EN_CHKECK_SEQ,bool);
	void SlotControllerSeqError(EN_CHKECK_SEQ, QString);
signals:
	void SigChmbCtrlInit();
signals:
	void SigChmbInitCheckReq();
	void SigChmbInitMsg( LOG_TYPE, QString );
private:
	Ui::CChmbDlg *ui;
private:
	QThread* m_pControllerTh;
	CChmbDlgController* m_pController;
private:
	bool m_RequestSeq ;
	bool m_ResponseSeq [EN_MAX_CHECK_SEQ];
	QString m_sCheckSeq[EN_MAX_CHECK_SEQ];
};

#endif // CHMBDLG_HPP
