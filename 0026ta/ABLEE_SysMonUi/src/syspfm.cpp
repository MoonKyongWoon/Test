#include "syspfm.hpp"
#include "ui_syspfm.h"
// CSysPfm

CSysPfm::CSysPfm(QWidget *parent) :
	QWidget(parent),
	ui(new Ui::CSysPfm)
{
	qRegisterMetaType<pfm_data>("pfm_data");
	ui->setupUi(this);
	initVariable();
	initSharedMemory();
	initUi();
	initSocket();
	initTimer();
}

CSysPfm::~CSysPfm()
{
	delete ui;
	deleteSharedMemory();
	deleteTimer();
	deleteThread();
}

void	CSysPfm::initVariable() {
	this->socket_controller = nullptr;
	for (int i = 0; i < 32; i +=1)
		this->pfm_table[i] = nullptr;
	this->m_pShmPfm = nullptr;
	this->thread = nullptr;
	this->render_timer = nullptr;
	memset(&this->update_data, 0, sizeof(pfm_update_data));

	this->pfm_table[0] = this->ui->widget_table_1;
	this->pfm_table[1] = this->ui->widget_table_2;
	this->pfm_table[2] = this->ui->widget_table_3;
	this->pfm_table[3] = this->ui->widget_table_4;
	this->pfm_table[4] = this->ui->widget_table_5;
	this->pfm_table[5] = this->ui->widget_table_6;
	this->pfm_table[6] = this->ui->widget_table_7;
	this->pfm_table[7] = this->ui->widget_table_8;
	this->pfm_table[8] = this->ui->widget_table_9;
	this->pfm_table[9] = this->ui->widget_table_10;
	this->pfm_table[10] = this->ui->widget_table_11;
	this->pfm_table[11] = this->ui->widget_table_12;
	this->pfm_table[12] = this->ui->widget_table_13;
	this->pfm_table[13] = this->ui->widget_table_14;
	this->pfm_table[14] = this->ui->widget_table_15;
	this->pfm_table[15] = this->ui->widget_table_16;
	this->pfm_table[16] = this->ui->widget_table_17;
	this->pfm_table[17] = this->ui->widget_table_18;
	this->pfm_table[18] = this->ui->widget_table_19;
	this->pfm_table[19] = this->ui->widget_table_20;
	this->pfm_table[20] = this->ui->widget_table_21;
	this->pfm_table[21] = this->ui->widget_table_22;
	this->pfm_table[22] = this->ui->widget_table_23;
	this->pfm_table[23] = this->ui->widget_table_24;
	this->pfm_table[24] = this->ui->widget_table_25;
	this->pfm_table[25] = this->ui->widget_table_26;
	this->pfm_table[26] = this->ui->widget_table_27;
	this->pfm_table[27] = this->ui->widget_table_28;
	this->pfm_table[28] = this->ui->widget_table_29;
	this->pfm_table[29] = this->ui->widget_table_30;
	this->pfm_table[30] = this->ui->widget_table_31;
	this->pfm_table[31] = this->ui->widget_table_32;
}

void		CSysPfm::initTimer() {
	this->render_timer = new QTimer();
	connect(this->render_timer, &QTimer::timeout, this, &CSysPfm::slotRender);
	this->render_timer->start(1000);
}


bool		CSysPfm::initSharedMemory() {
	this->m_pShmPfm = new QSharedMemory( SHM_KEY::SHM_PFM_MON_KEY, this );
	if (this->m_pShmPfm->isAttached())
		return true;
	this->m_pShmPfm->attach(QSharedMemory::ReadWrite);
	if (this->m_pShmPfm->isAttached())
		return true;
	if( !this->m_pShmPfm->create( sizeof(stShmMainPfm), QSharedMemory::ReadWrite ) )
		return false;
	stShmMainPfm* shm_data = reinterpret_cast<stShmMainPfm*>(this->m_pShmPfm->data());
	memset(shm_data, 0, sizeof(stShmMainPfm));
	return true;
}

void    CSysPfm::initUi() {
	for (int i = 0; i < 32; i += 1)
		this->pfm_table[i]->updateCPU(i % 2 + 1);

	for (int i = 0; i < 16; i += 1) {
		if (isMaskedSlot(i))
			this->ui->tab_pfm->setTabIcon(i, PIXMAP_MASK);
		else
			this->ui->tab_pfm->setTabIcon(i, PIXMAP_FAIL);
	}
}

void	CSysPfm::initSocket() {
	this->socket_controller = new CPfmSocketController();
	this->thread = new QThread;
	this->socket_controller->moveToThread(this->thread);

	connect(this->thread, &QThread::finished, this->socket_controller, &QObject::deleteLater);
	connect(this->socket_controller, &CPfmSocketController::sigStatusChanged, this, &CSysPfm::slotSocketStateChanged);
	connect(this->socket_controller, &CPfmSocketController::sigReceivePacket, this, &CSysPfm::slotSocketReceivePacket);
	connect(this, &CSysPfm::sigCreateSocket, this->socket_controller, &CPfmSocketController::slotCreateSocket);
	this->thread->start();
	emit sigCreateSocket();
}

void	CSysPfm::deleteThread() {
	if (this->thread) {
		this->thread->quit();
		this->thread->wait();
		this->thread->deleteLater();
	}
}

void	CSysPfm::deleteSharedMemory() {
	if (this->m_pShmPfm) {
		if (this->m_pShmPfm->isAttached())
			this->m_pShmPfm->detach();
		delete this->m_pShmPfm;
		this->m_pShmPfm = nullptr;
	}
}

void	CSysPfm::deleteTimer() {
	if (this->render_timer) {
		if (this->render_timer->isActive())
			this->render_timer->stop();
		delete this->render_timer;
		this->render_timer = nullptr;
	}
}

QString		CSysPfm::getHostIP(int descriptor) {
	QString ip;
	UTIL::INI::readSettings(
				tr("%1/%2/%3").arg(QString(getenv("ATOS_DIR"))).arg(CONFIG_DIR).arg(SITE_INI),
				tr("SLOT%1/%2").arg(descriptor + 1).arg("CPU1_IP"),
				ip);
	return ip;
}

void	CSysPfm::saveLog(pfm_unit_data data, int index, int runner) {
	QString env_path = QString(getenv("ATDATA_DIR"));
	for (int ch = 0; ch < 8; ch += 1) {
		QString log_message = "";

		log_message += timeToQString(data.time, 0);
		log_message += QString::asprintf(",%5s%d", "PPS", ch + 1);
		log_message += QString::asprintf(",%6.2f", data.pps[ch].set_voltage);
		log_message += QString::asprintf(",%6.2f", data.pps[ch].present_voltage);
		log_message += QString::asprintf(",%6.2f", data.pps[ch].present_current);
		log_message += QString::asprintf(",  0x%04x", data.pps[ch].status_word & ~ STATUS_WORD_MASK);
		log_message += QString::asprintf(",  0x%02x", data.pps[ch].status_vout & ~ STATUS_VOUT_MASK);
		log_message += QString::asprintf(", %5s ", isStatusFault(data.pps[ch].status_word, data.pps[ch].status_vout)? "FAULT" : "");
		log_message += QString::asprintf(",%5s%d", "S1PS", ch + 1);
		log_message += QString::asprintf(",%6.2f", data.s1ps[ch].set_voltage);
		log_message += QString::asprintf(",%6.2f", data.s1ps[ch].present_voltage);
		log_message += QString::asprintf(",%6.2f", data.s1ps[ch].present_current);
		log_message += QString::asprintf(",  0x%04x", data.s1ps[ch].status_word & ~STATUS_WORD_MASK);
		log_message += QString::asprintf(",  0x%02x", data.s1ps[ch].status_vout & ~STATUS_VOUT_MASK);
		log_message += QString::asprintf(", %5s ", isStatusFault(data.s1ps[ch].status_word, data.s1ps[ch].status_vout) ? "FAULT" : "");
		UTIL::FILE::AppendFile(tr("%1/pfm/SLOT%2-%3.dat").arg(env_path).arg(index).arg(runner + 1), log_message, true);
	}
}

void	CSysPfm::saveFaultIni(pfm_unit_data data, int index, int runner) {
	QString ini_file = tr("%1/pfm/SLOT%2_pfm_fault.ini").arg(QString(getenv("ATDATA_DIR"))).arg(index);

	for (int ch = 0; ch < 8; ch += 1) {
		if (1 << ch & data.is_fault && !fault[index][runner][ch]) {
//		if (1 << ch & data.is_fault) {
			fault[index][runner][ch] = true;
			UTIL::INI::writeSettings(ini_file, tr("CPU%1/PPS%2").arg(runner + 1).arg(ch + 1), 1);
		}
		if (1 << ch << 8 & data.is_fault && !fault[index][runner][ch + 8]) {
//		if (1 << ch << 8 & data.is_fault) {
			fault[index][runner][ch + 8] = true;
			UTIL::INI::writeSettings(ini_file, tr("CPU%1/S1PS%2").arg(runner + 1).arg(ch + 1), 1);
		}
	}
}

void	CSysPfm::saveSharedData(pfm_unit_data data, int index, int runner) {
	sm_cpu_data shared_data;
	for (int i = 0; i < 8; i += 1) {
		shared_data.pps_data[i].set_voltage = data.pps[i].set_voltage;
		shared_data.pps_data[i].present_voltage = data.pps[i].present_voltage;
		shared_data.pps_data[i].present_current = data.pps[i].present_current;
		shared_data.pps_data[i].status_word = data.pps[i].status_word;
		shared_data.pps_data[i].status_vout = data.pps[i].status_vout;

		shared_data.s1ps_data[i].set_voltage = data.s1ps[i].set_voltage;
		shared_data.s1ps_data[i].present_voltage = data.s1ps[i].present_voltage;
		shared_data.s1ps_data[i].present_current = data.s1ps[i].present_current;
		shared_data.s1ps_data[i].status_word = data.s1ps[i].status_word;
		shared_data.s1ps_data[i].status_vout = data.s1ps[i].status_vout;
	}
	setSharedMemory(&shared_data, runner, index);
}

void	CSysPfm::setSharedMemory(sm_cpu_data* ShmData, int cpu_no, int slot_no) {
	if( !this->m_pShmPfm->isAttached() )
		this->m_pShmPfm->attach( QSharedMemory::ReadWrite );
	this->m_pShmPfm->lock();
	stShmMainPfm* to = reinterpret_cast<stShmMainPfm*>(this->m_pShmPfm->data());
	sm_cpu_data* read_data = &to->slot_data[slot_no].cpu_data[cpu_no];
	memcpy( read_data, ShmData, sizeof(sm_cpu_data) );
	this->m_pShmPfm->unlock();
}

QString	CSysPfm::timeToQString(pfm_time pfm_time, int offset) {
	pfm_time.val.tv_usec -= offset * 100000;
	if (pfm_time.val.tv_usec >= 0)
		return QString::asprintf("%02d:%02d:%02d.%03ld", pfm_time.t.tm_hour, pfm_time.t.tm_min, pfm_time.t.tm_sec, pfm_time.val.tv_usec / 1000);
	pfm_time.val.tv_usec += 1000000;
	pfm_time.t.tm_sec -= 1;
	if (pfm_time.t.tm_sec >= 0)
		return QString::asprintf("%02d:%02d:%02d.%03ld", pfm_time.t.tm_hour, pfm_time.t.tm_min, pfm_time.t.tm_sec, pfm_time.val.tv_usec / 1000);
	pfm_time.t.tm_sec += 60;
	pfm_time.t.tm_min -= 1;
	if (pfm_time.t.tm_min >= 0)
		return QString::asprintf("%02d:%02d:%02d.%03ld", pfm_time.t.tm_hour, pfm_time.t.tm_min, pfm_time.t.tm_sec, pfm_time.val.tv_usec / 1000);
	pfm_time.t.tm_min += 60;
	pfm_time.t.tm_hour -= 1;
	if (pfm_time.t.tm_hour >= 0)
		return QString::asprintf("%02d:%02d:%02d.%03ld", pfm_time.t.tm_hour, pfm_time.t.tm_min, pfm_time.t.tm_sec, pfm_time.val.tv_usec / 1000);
	pfm_time.t.tm_hour += 24;
	return QString::asprintf("%02d:%02d:%02d.%03ld", pfm_time.t.tm_hour, pfm_time.t.tm_min, pfm_time.t.tm_sec, pfm_time.val.tv_usec / 1000);
}

bool	CSysPfm::isStatusFault(int status_word, int status_vout) {
	if (status_word & ~STATUS_WORD_MASK || status_vout & ~STATUS_VOUT_MASK)
		return true;
	return false;
}

bool	CSysPfm::isMaskedSlot(int descriptor) {
	int isMask;
	UTIL::INI::readSettings(
				tr("%1/%2/%3").arg(QString(getenv("ATOS_DIR"))).arg(CONFIG_DIR).arg(SITE_INI),
				tr("SLOT%1/%2").arg(descriptor + 1).arg("MASK"),
				isMask);
	return isMask == 0 ? false : true;
}

void    CSysPfm::slotSocketStateChanged(bool is_socket_conneted, int descriptor) {
	if (is_socket_conneted)
		this->ui->tab_pfm->setTabIcon(descriptor, PIXMAP_PASS);
	else
		this->ui->tab_pfm->setTabIcon(descriptor, PIXMAP_FAIL);
}

void	CSysPfm::slotSocketReceivePacket(pfm_unit_data data, int index, int runner) {
	saveSharedData(data, index, runner);
	saveLog(data, index, runner);
	saveFaultIni(data, index, runner);

	this->update_data.need_update[index][runner] = true;
	this->update_data.data[index][runner] = data;
}

void	CSysPfm::slotRender() {
	for (int slot_no = 0; slot_no < 16; slot_no += 1) {
		for (int cpu_no = 0; cpu_no < 2; cpu_no += 1) {
			if (this->update_data.need_update[slot_no][cpu_no]) {
				this->update_data.need_update[slot_no][cpu_no] = false;
				this->pfm_table[slot_no * 2 + cpu_no]->updateData(this->update_data.data[slot_no][cpu_no]);
			}
		}
	}
}
