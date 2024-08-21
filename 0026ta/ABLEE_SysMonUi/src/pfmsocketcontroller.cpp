#include "pfmsocketcontroller.hpp"

CPfmSocketController::CPfmSocketController(QObject *parent) : QObject(parent) {
	qRegisterMetaType<stTcpElement>("stTcpElement");
}

CPfmSocketController::~CPfmSocketController() {
	for (int i = 0; i < 16; i += 1) {
		if (this->sockets[i])
			delete this->sockets[i];
		if (this->timer[i])
			delete this->timer[i];
	}
}

QString		CPfmSocketController::getHostIP(int descriptor) {
	QString ip;
	UTIL::INI::readSettings(
				tr("%1/%2/%3").arg(QString(getenv("ATOS_DIR"))).arg(CONFIG_DIR).arg(SITE_INI),
				tr("SLOT%1/%2").arg(descriptor + 1).arg("CPU1_IP"),
				ip);
	return ip;
}

bool		CPfmSocketController::isMaskedSlot(int descriptor) {
	int isMask;
	UTIL::INI::readSettings(
				tr("%1/%2/%3").arg(QString(getenv("ATOS_DIR"))).arg(CONFIG_DIR).arg(SITE_INI),
				tr("SLOT%1/%2").arg(descriptor + 1).arg("MASK"),
				isMask);
	return isMask == 0 ? false : true;
}

void		CPfmSocketController::sendSystemInfoToSlot(int index) {
	stTcpHeader header;

	header.cmd = PROTOCOL::CMD_SLOT_CPU_INFO_SEND;
	header.src = PROTOCOL::_ATSYSMON_UI;
	header.dest = PROTOCOL::_ATPFMD;
	header.slot = index;
	header.datalen = sizeof(stSlotCpu);

	stSlotCpu slot_cpu;

	slot_cpu.nSlotNo = index + 1;
	slot_cpu.nMaxSlot    = 16;
	slot_cpu.nCpuPerSlot = 2;
	slot_cpu.nPgmPerSlot = 2;

	uint n_size = sizeof(stTcpHeader) + header.datalen;
	char *buffer = new char[n_size];
	memcpy(buffer, &header, sizeof(stTcpHeader));
	memcpy(buffer + sizeof(stTcpHeader), &slot_cpu, header.datalen);

	this->sockets[index]->SendPacket(index, buffer, n_size);
	if (n_size) {
		delete[] buffer;
		buffer = nullptr;
	}
}

void		CPfmSocketController::slotCreateSocket() {
	for (int i = 0; i < 16; i += 1) {
		if (isMaskedSlot(i))
			continue;
		this->timer[i] = new QTimer(this);
		this->sockets[i] = new CTcpSocket(i, 0);
		this->sockets[i]->SetHostInfo(getHostIP(i), TCP_PORT::SYSTEM::PFM_PORT);

		connect(this->sockets[i], &CTcpSocket::SigStatusChanged, this, [this](int m_nId0, int m_nId1, bool m_bConn){this->slotStatusChanged(m_bConn, m_nId0);});
		connect(this->sockets[i], &CTcpSocket::SigReceivePacket, this, [this](int m_nId0, int m_nId1, stTcpElement data){this->slotReceivePacket(data, m_nId0);});
		connect(this->timer[i], &QTimer::timeout, this, [this, i]{this->sockets[i]->SlotConnectToHost(i);});

		this->timer[i]->start(3000);
	}
}

void		CPfmSocketController::slotStatusChanged(bool m_bConn, int index) {
	if (m_bConn) {
		this->timer[index]->stop();
		sendSystemInfoToSlot(index);
	}
	else
		this->timer[index]->start();
	emit sigStatusChanged(m_bConn, index);
}

void		CPfmSocketController::slotReceivePacket(stTcpElement data, int index) {
	pfm_unit_data pfm;
	memcpy(&pfm, data.pData, data.tcpHeader.datalen);

	emit sigReceivePacket(pfm, index, data.tcpHeader.runner);
	delete[] data.pData;
}
