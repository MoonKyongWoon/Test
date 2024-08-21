#ifndef CPFMSOCKETCONTROLLER_HPP
#define CPFMSOCKETCONTROLLER_HPP

#include <QObject>
#include <QTimer>
#include <QMetaType>
#include <QSharedMemory>
#include "tcpsocket.hpp"
#include "commstruct.hpp"
#include "syspfm.hpp"
#include "syspfm.hpp"

class CSysPfm;

enum class PFM_STATUS_TYPE;

class CPfmSocketController : public QObject
{
	Q_OBJECT

private:
	CTcpSocket		*sockets[16];
	QTimer			*timer[16];

public:

private:
	QString			getHostIP(int descriptor);
	bool			isMaskedSlot(int descriptor);
	void			sendSystemInfoToSlot(int index);
	bool			isStatusFault(int status_word, int status_vout);
	PFM_STATUS_TYPE	getPfmStatus(int status_word, int status_vout);
	QString			getQStringFromPfmStatusType(PFM_STATUS_TYPE pfm_status_type);

public:
	explicit CPfmSocketController(QObject *parent = nullptr);
	~CPfmSocketController();

signals:
	void	sigStatusChanged(bool m_bConn, int index);
	void	sigReceivePacket(pfm_unit_data data, int index, int runner);

public slots:
	void	slotStatusChanged(bool m_bConn, int index);
	void	slotReceivePacket(stTcpElement data, int index);

	void	slotCreateSocket();
};

#endif // CPFMSOCKETCONTROLLER_HPP
