#ifndef SYSPFM_HPP
#define SYSPFM_HPP

#include <QMetaType>
#include <QWidget>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QTcpSocket>
#include <QDateTime>
#include <QSharedMemory>
#include "syspwr.hpp"
#include "tcpsocket.hpp"
#include "commstruct.hpp"
#include "pfmsocketcontroller.hpp"
#include "pfmtable.hpp"

class CPfmSocketController;

namespace Ui {
class CSysPfm;
}

class CSysPfm : public QWidget
{
	Q_OBJECT

public:
	explicit		CSysPfm(QWidget *parent = nullptr);
	~CSysPfm();

private:
	Ui::CSysPfm				*ui;
	QSharedMemory			*m_pShmPfm;
	CPfmSocketController	*socket_controller;
	QThread					*thread;
	CPfmTable				*pfm_table[32];
	int						fault[16][2][16];//[slot][cpu][ch]
	pfm_update_data			update_data;
	QTimer					*render_timer;

private:
	void			initUi();
	void			initSocket();
	void			initVariable();
	void			initTimer();
	bool			initSharedMemory();

	void			deleteSharedMemory();
	void			deleteTimer();
	void			deleteThread();

	bool			isMaskedSlot(int descriptor);
	QString			getHostIP(int descriptor);

	void			saveLog(pfm_unit_data data, int index, int runner);
	void			saveFaultIni(pfm_unit_data data, int index, int runner);
	void			saveSharedData(pfm_unit_data data, int index, int runner);
	void			setSharedMemory(sm_cpu_data* ShmData, int cpu_no, int slot_no);

	QString			timeToQString(pfm_time pfm_time, int offset);
	bool			isStatusFault(int status_word, int status_vout);

signals:
	void			sigCreateSocket();

public slots:
	void			slotSocketStateChanged(bool is_socket_conneted, int descriptor);
	void			slotSocketReceivePacket(pfm_unit_data data, int i, int runner);
	void			slotRender();
};

#endif // SYSPFM_HPP
