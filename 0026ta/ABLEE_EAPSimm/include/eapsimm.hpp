#ifndef EAPSIMM_HPP
#define EAPSIMM_HPP

#include <bitset>
#include <QMessageBox>
#include <QMenu>
#include <QTableWidget>
#include <QWidget>

#include "clientsimm.hpp"
#include "commstruct.hpp"
#include "msgdlg.hpp"
#include "eaputil.hpp"
#include "sysini.hpp"
#include "hsmssocket.hpp"
#include "hsmsclient.hpp"
#include "serversimm.hpp"
#include "tcpserver.hpp"
#include "util.hpp"

#define INI_KEY_SML					"SML"
#define INI_KEY_DESCRIPTION			"DESCRIPTION"

namespace Ui {
	class CEapSimm;
}

class CEapSimm : public QWidget
{
	Q_OBJECT

public:
	explicit CEapSimm(QWidget *parent = nullptr);
	~CEapSimm();

private:
	void LoadIni();
	void initUi();
	void initVariable();

	void printControlMessageToBit(stHsmsElement element);
	void hsmsSelect(stHsmsElement element);
	void setButtonEnable(bool enable);
	void appendBinLog(QString log);
	void appendSystemLog(QString log);
	void appendHsmsInfo(QString log);
	void appendSmlLog(QString log);
	void setTcpStatus(bool bConn);
	void setHsmsStatus(bool bSelect);
private:
	void LoadStreamFunction(QString);
private:
	Ui::CEapSimm *ui;
	CHsmsSocket	*socket;
	CServerSimm *server_simm;
	CClientSimm *client_simm;
	HSMS_STATUS hsms_status;
	uint system_bytes;
	uint device_id;
	QByteArray m_baHsmsPacket;
private:
	QString m_sCurrentSF;
	CSecsGem m_secsgem;
	CMsgDlg* m_pMsgDlg;
	QStringList m_listSetions;
public slots:
	void slotSetMode();

	void slotAppendBinLog(QString log);
	void slotAppendSystemLog(QString log);
	void slotAppendSmlLog(QString log);
	void slotAppendHsmsInfo(QString log);
	void slotSetTcpStatus(bool bConn);
	void slotSetHsmsStatus(bool bSelect);

	void slotSelectStreamFunction( const QModelIndex& index);
	void slotSmlReload();
	void slotSmlSave();
	void slotSendDataMessage();

	void slotConvertFromBinToSml();
    void slotRecvSystemBytes(uint);
public slots:
	void SlotCustomContextMenuSFList( const QPoint& );
	void SlotCustomContextMenuSMLView( const QPoint& );
	void AddStreamFunction();
	void CopyStreamFunction();
	void RemoveStreamFunction();
	void SlotSaveMsg( QString, QString, QString );

};

#endif // EAPSIMM_HPP
