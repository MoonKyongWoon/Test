#include "../include/monitor.hpp"

CMonitor::CMonitor( uint slot, uint cpu, QObject *parent) : QObject(parent), m_pio(nullptr), m_bOpen(false)
{
	m_SLOT = slot;
	m_CPU = cpu;
	m_FAULT_MASK.status_vout = STATUS_VOUT_MASK;
	m_FAULT_MASK.status_word = STATUS_WORD_MASK;
	m_bRun = false;
	m_bFirst = true;
}

CMonitor::~CMonitor()
{
	CloseHW();
}

bool CMonitor::OpenHW()
{
	m_pio = new CPcie();
	if( m_CPU == CPU00 )
		m_pio->SetPcieTPG( PCIE_DEVICE::PCIE_TPG0 );
	else
		m_pio->SetPcieTPG( PCIE_DEVICE::PCIE_TPG1 );
	return m_pio->pcie_open();
}

bool CMonitor::CloseHW()
{
	if( m_pio )
	{
		if( m_pio->pcie_isopen() )
		{
			m_pio->pcie_close();
		}
		delete m_pio;
		m_pio = nullptr;
	}
	return true;
}

bool CMonitor::SetFaultMask( uint unit, UFAULT_MASK mask )
{
	if( !m_bOpen ) return false;
	if( unit == PPS )
	{
		m_pio->write_user( BITCHART::PFM::PFM_PPS_FAULT_MASK , mask.mask );
	}
	else
	{
		m_pio->write_user( BITCHART::PFM::PFM_S1PS_FAULT_MASK, mask.mask );
	}
	return true;
}

void CMonitor::SlotOpenHW()
{
	m_bOpen = OpenHW();
	if( m_bOpen ) { emit SigOpenHWResult( m_CPU, true  ); emit SigLogMessage( m_CPU, _NORMAL_LOG, tr("%1 H/W open pass!").arg(m_CPU+1) ); }
	else          { emit SigOpenHWResult( m_CPU, false ); emit SigLogMessage( m_CPU, _ERROR_LOG , tr("%1 H/W open fail!").arg(m_CPU+1) ); }
	//m_pio->write_user( BITCHART::PFM::PFM_CLEAR, 0x1 );

}

void CMonitor::SetStop()
{
	m_bRun = false;
	emit SigLogMessage( m_CPU, _NORMAL_LOG, tr("%1 Monitoring Stop!").arg(m_CPU+1) );
}

void CMonitor::SlotRun(uint cpu)
{
	if (cpu != this->m_CPU)
		return ;
	emit SigLogMessage( m_CPU, _NORMAL_LOG, tr("%1 Monitoring Start!").arg(m_CPU+1) );
	Run();
}

bool CMonitor::IsMonitoring()
{
	int busy = m_pio->read_user( BITCHART::PFM::PFM_BUSY );
	//emit SigLogMessage( m_CPU, _NORMAL_LOG, tr("CPU%1 BUSY STATUS : 0x%2").arg(m_CPU+1).arg(busy,0,16) );
	if( busy == 0x1 ) return true;
	else              return false;
}

void CMonitor::ClearMemory()
{
	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		for( int m=0;m<PFM_FAULT_MEMORY_DEPTH;m++ )
		{
			m_pio->write_user( BITCHART::PFM::PFM_PPS1_READ_IOUT_VOUT  + (ch*0x100) + (m*4), 0x0 );
			m_pio->write_user( BITCHART::PFM::PFM_PPS1_STAT_SCALE_WORD + (ch*0x100) + (m*4), 0x0 );
			m_pio->write_user( BITCHART::PFM::PFM_PPS1_STAT_VOUT       + (ch*0x100) + (m*4), 0x0 );
			//fprintf( stdout, "PPS%d  %03d  IOUT_VOUT(0x%08X)  SCALE_WORD(0x%08X)  STAT_VOUT(0x%08X)\n",
			//		 ch, m, BITCHART::PFM::PFM_PPS1_READ_IOUT_VOUT  + (ch*0x100) + (m*4), BITCHART::PFM::PFM_PPS1_STAT_SCALE_WORD + (ch*0x100) + (m*4), BITCHART::PFM::PFM_PPS1_STAT_VOUT + (ch*0x100) + (m*4) );
		}
	}
}

void CMonitor::readTPGInfo() {
	m_pio->read_user( BITCHART::GLOBAL_MODE_REG::_VER_YMD );
	m_pio->read_user( BITCHART::GLOBAL_MODE_REG::_VER_FW );
}

void CMonitor::Run()
{
	if( !m_bOpen )
	{
		emit SigLogMessage( m_CPU, _ERROR_LOG , tr("%1 Monitoring start fail, H/W is not open!").arg(m_CPU+1) );
		return;
	}
	//ClearMemory();

	SetFaultMask( PPS , m_FAULT_MASK );
	SetFaultMask( S1PS, m_FAULT_MASK );

	stTcpHeader	tcp_header;

//	pfm_data	data; // hsbae
	pfm_unit_data	data;

//	memset(&data, 0, sizeof(pfm_data));
	memset(&data, 0, sizeof(pfm_unit_data));

	std::string run_path = "";
	char* env_run  = getenv(ATRUN_DIR_ENV);
	run_path = env_run != NULL ? std::string( env_run ) : std::string( DIR_ATRUN_PATH );
	QString set_vs_ini = tr("%1/%2/%3").arg(run_path.c_str()).arg(CONFIG_DIR).arg(m_CPU == 0 ? PFM_SETVS_INI_1 : PFM_SETVS_INI_2);
	QFile ini_file(set_vs_ini);
	if (ini_file.open(QIODevice::ReadWrite | QIODevice::Text))
		ini_file.close();
	else
		qDebug() << "FAIL TO OPEN INI FILE";

	forever
	{
		if( !IsMonitoring() )
		{
			usleep(100);
			continue;
		}

		m_pio->delay( MONITORING_PERIOD );

		m_PFM_STATUS.pfm_status = m_pio->read_user( BITCHART::PFM::PFM_STATUS );
		printf("PPS STATUS : 0x%08X\n", m_PFM_STATUS.pfm_status);
		emit SigLogMessage( m_CPU, _NORMAL_LOG, tr("PFM Status : 0x%1").arg(m_PFM_STATUS.pfm_status,0, 16) );

		memset(&tcp_header, 0, sizeof(stTcpHeader));
		tcp_header.cmd = PROTOCOL::TCP_COMMAND::CMD_PFM_SYSMONUI_SEND;
		tcp_header.src = PROTOCOL::SOURCE_TYPE::_ATPFMD;
		tcp_header.dest = PROTOCOL::SOURCE_TYPE::_ATSYSMON_UI;
		tcp_header.slot = PROTOCOL::SOURCE_TYPE::_ATSLOT01 + this->m_SLOT - 1;
		tcp_header.runner = m_CPU;
		tcp_header.error = 0;
//		tcp_header.datalen = sizeof(pfm_data);
		tcp_header.datalen = sizeof(pfm_unit_data);

		memset(&data, 0, sizeof(data));

		for (int i = 0; i < 8; i += 1) {
			UTIL::INI::readSettings(set_vs_ini, tr("UNIT/PPS%1").arg(i + 1), data.pps[i].set_voltage);
			UTIL::INI::readSettings(set_vs_ini, tr("UNIT/S1PS%1").arg(i + 1), data.s1ps[i].set_voltage);
		}


		printf(" PPS FAULT : [0x%02X]\tS1PS FAULT : [0x%02X]\n", m_PFM_STATUS.status_pps & 0xFF, m_PFM_STATUS.status_s1ps & 0xFF);
//		PpsMonitoring (&data);
//		S1psMonitoring(&data);
		PpsUnitMonitoring(&data);
		S1psUnitMonitoring(&data);

		this->m_bFirst = false;

		/*
		qDebug() << "POWER FAULT MONITORING RESULT";
		for (int ch = 0; ch < 8; ch += 1) {
			for (int i = 0; i >= 0; i -= 1) {
				qDebug("%s PPS CH[%02d] [%02d] SV : %6.2f\tV : %6.2f\tI : %6.2f\tSW : 0x%04X\tSV : 0x%02X",
					   (1 << ch & m_PFM_STATUS.status_pps) ? "*" : " ",
					   ch + 1,
					   data.pps[ch].depth,
					   data.pps[ch].set_voltage,
					   data.pps[ch].present_voltage,
					   data.pps[ch].present_current,
					   data.pps[ch].status_word,
					   data.pps[ch].status_vout);
			}
		}
		qDebug() << "-----------------------------------------------------------";
		for (int ch = 0; ch < 8; ch += 1) {
			for (int i = 0; i >= 0; i -= 1) {
				qDebug("%sS1PS CH[%02d] [%02d] SV : %6.2f\tV : %6.2f\tI : %6.2f\tSW : 0x%04X\tSV : 0x%02X",
					   (1 << ch & m_PFM_STATUS.status_s1ps) ? "*" : " ",
					   ch + 1,
					   data.s1ps[ch].depth,
					   data.s1ps[ch].set_voltage,
					   data.s1ps[ch].present_voltage,
					   data.s1ps[ch].present_current,
					   data.s1ps[ch].status_word,
					   data.s1ps[ch].status_vout);
			}
		}
*/

		if (IsMonitoring()) {

			gettimeofday(&data.time.val, NULL);
			tm* t = localtime(&data.time.val.tv_sec);
			data.time.t = *t;

//			uint send_size = sizeof(stTcpHeader) + sizeof(pfm_data);
			uint send_size = sizeof(stTcpHeader) + sizeof(pfm_unit_data);
			char* send_data = new char[send_size];
			memcpy(send_data, &tcp_header, sizeof(stTcpHeader));
//			memcpy(send_data + sizeof(stTcpHeader), &data, sizeof(pfm_data));
			memcpy(send_data + sizeof(stTcpHeader), &data, sizeof(pfm_unit_data));
			emit sigTCPSend(send_data, send_size);
		}
	}
}

void CMonitor::PpsUnitMonitoring(pfm_unit_data* data) {
	UREAD_IOUT_VOUT		read_iout_vout   [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_SCALE_WORD	scale_status_word[PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_STATUS_VOUT	status_vout      [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];

	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		if( (1 << ch) & this->m_PFM_STATUS.status_pps)
			UTIL::INI::writeSettings("/home/ablee/atsystem/config/pfm_fault.ini", tr("CPU%1/PPS%2").arg(this->m_CPU + 1).arg(ch + 1), 1);

		for( int m=0;m<PFM_FAULT_MEMORY_DEPTH;m++ )
		{
			read_iout_vout[ch][m].read_iout_vout     = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_READ_IOUT_VOUT  + (ch*0x100) + (m*4) );
			scale_status_word[ch][m].read_scale_word = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_STAT_SCALE_WORD + (ch*0x100) + (m*4) );
			status_vout[ch][m].read_status_vout      = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_STAT_VOUT       + (ch*0x100) + (m*4) );
		}
	}

	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		double dVout, dIout;

		int i = 1;
//		int compensated_depth = (1 << ch & data->is_fault) ? (POINTER[ch] - i) : (this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_FAULT_POINTER + ch * 4) - i);
		int compensated_depth = (this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_FAULT_POINTER + ch * 4) - i);

		if (m_bFirst && compensated_depth < 0)
			continue;
		if (compensated_depth < 0)
			compensated_depth += PFM_FAULT_MEMORY_DEPTH;

		dVout = VoutToDouble( read_iout_vout[ch][compensated_depth].read_vout );
		dIout = IoutToDouble( read_iout_vout[ch][compensated_depth].read_iout );

		data->pps[ch].depth = compensated_depth + 1;
		data->pps[ch].present_current = dIout;
		data->pps[ch].present_voltage = dVout;
		data->pps[ch].status_word = scale_status_word[ch][compensated_depth].status_word & 0xFFFF;
		data->pps[ch].status_vout = status_vout[ch][compensated_depth].status_vout & 0xFF;
//		data->pps[ch].status_word[i - 1] = data->pps[ch].status_word[i - 1] & ~STATUS_WORD_MASK;

	}
}

void CMonitor::S1psUnitMonitoring(pfm_unit_data* data)
{
	UREAD_IOUT_VOUT read_iout_vout    [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_SCALE_WORD scale_status_word[PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_STATUS_VOUT status_vout     [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];

	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		if( (1 << ch) & this->m_PFM_STATUS.status_s1ps)
			UTIL::INI::writeSettings("/home/ablee/atsystem/config/pfm_fault.ini", tr("CPU%1/S1PS%2").arg(this->m_CPU + 1).arg(ch + 1), 1);

		for( int m=0;m<PFM_FAULT_MEMORY_DEPTH;m++ )
		{
			read_iout_vout[ch][m].read_iout_vout     = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_READ_IOUT_VOUT  + (ch*0x100) + (m*4) );
			scale_status_word[ch][m].read_scale_word = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_STAT_SCALE_WORD + (ch*0x100) + (m*4) );
			status_vout[ch][m].read_status_vout      = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_STAT_VOUT       + (ch*0x100) + (m*4) );
		}
	}
	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		double dVout, dIout;
		double dAttV;

		int i = 1;

//		int compensated_depth = (1 << 8 << ch & data->is_fault) ? (POINTER[ch] - i) : (this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_FAULT_POINTER + ch * 4) - i);
		int compensated_depth = (this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_FAULT_POINTER + ch * 4) - i);
		if (m_bFirst && compensated_depth < 0)
			continue;
		if (compensated_depth < 0)
			compensated_depth += PFM_FAULT_MEMORY_DEPTH;

		dVout = VoutToDouble( read_iout_vout[ch][compensated_depth].read_vout );
		dIout = ConvS1psIout(dVout, read_iout_vout[ch][compensated_depth].read_iout);
		if (dIout < 0) dIout = 0;
		GetS1psAttVoltage( scale_status_word[ch][compensated_depth].vout_scale_loop & 0xFFFF , dVout, dAttV );

		data->s1ps[ch].depth = compensated_depth + 1;
		data->s1ps[ch].present_current = dIout;
		data->s1ps[ch].present_voltage = dAttV;
		data->s1ps[ch].status_word = scale_status_word[ch][compensated_depth].status_word & 0xFFFF;
		data->s1ps[ch].status_vout = status_vout[ch][compensated_depth].status_vout & 0xFF;
//		data->s1ps[ch].status_word[i - 1] = data->s1ps[ch].status_word[i - 1] & ~STATUS_WORD_MASK;

	}
}

void CMonitor::PpsMonitoring(pfm_data* data)
{
	bool FAULT[PPS_COUNT]   = { false, };
	int  POINTER[PPS_COUNT] = { 0x0, };
	UREAD_IOUT_VOUT read_iout_vout    [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_SCALE_WORD scale_status_word[PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_STATUS_VOUT status_vout     [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];

	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
//		if( !IsMonitoring() ) return;
		if( (1 << ch) & this->m_PFM_STATUS.status_pps)
		{
//			data->is_fault = data->is_fault | (this->m_PFM_STATUS.status_pps & 0xFF);
			FAULT  [ch] = true;
			POINTER[ch] = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_FAULT_POINTER + ch * 4 );
			if (IsMonitoring())
				UTIL::INI::writeSettings("/home/ablee/atsystem/config/pfm_fault.ini", tr("CPU%1/PPS%2").arg(this->m_CPU + 1).arg(ch + 1), 1);
		}
		for( int m=0;m<PFM_FAULT_MEMORY_DEPTH;m++ )
		{
			read_iout_vout[ch][m].read_iout_vout     = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_READ_IOUT_VOUT  + (ch*0x100) + (m*4) );
			scale_status_word[ch][m].read_scale_word = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_STAT_SCALE_WORD + (ch*0x100) + (m*4) );
			status_vout[ch][m].read_status_vout      = this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_STAT_VOUT       + (ch*0x100) + (m*4) );
		}
	}

	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		double dVout, dIout;

		for (int i = 1; i <= 10; i += 1) {
//			int compensated_depth = (1 << ch & data->is_fault) ? (POINTER[ch] - i) : (this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_FAULT_POINTER + ch * 4) - i);
			int compensated_depth = (this->m_pio->read_user( BITCHART::PFM::PFM_PPS1_FAULT_POINTER + ch * 4) - i);

			if (m_bFirst && compensated_depth < 0)
				continue;
			if (compensated_depth < 0)
				compensated_depth += PFM_FAULT_MEMORY_DEPTH;

			dVout = VoutToDouble( read_iout_vout[ch][compensated_depth].read_vout );
			dIout = IoutToDouble( read_iout_vout[ch][compensated_depth].read_iout );

			data->pps[ch].depth[i - 1] = compensated_depth + 1;
			data->pps[ch].present_current[i - 1] = dIout;
			data->pps[ch].present_voltage[i - 1] = dVout;
			data->pps[ch].status_word[i - 1] = scale_status_word[ch][compensated_depth].status_word & 0xFFFF;
			data->pps[ch].status_vout[i - 1] = status_vout[ch][compensated_depth].status_vout & 0xFF;
//			data->pps[ch].status_word[i - 1] = data->pps[ch].status_word[i - 1] & ~STATUS_WORD_MASK;
		}
	}
}

void CMonitor::S1psMonitoring(pfm_data* data)
{
	bool FAULT[PPS_COUNT]   = { false, };
	int  POINTER[PPS_COUNT] = { 0x0, };
	UREAD_IOUT_VOUT read_iout_vout    [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_SCALE_WORD scale_status_word[PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];
	UREAD_STATUS_VOUT status_vout     [PPS_COUNT][PFM_FAULT_MEMORY_DEPTH];

	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
//		if( !IsMonitoring() ) return;
		if( (1 << ch) & this->m_PFM_STATUS.status_s1ps)
		{
//			data->is_fault = (data->is_fault << 8) | (this->m_PFM_STATUS.status_s1ps & 0xFF);
			FAULT  [ch] = true;
			POINTER[ch] = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_FAULT_POINTER + ch * 4);
			if (IsMonitoring())
				UTIL::INI::writeSettings("/home/ablee/atsystem/config/pfm_fault.ini", tr("CPU%1/S1PS%2").arg(this->m_CPU + 1).arg(ch + 1), 1);
		}
		for( int m=0;m<PFM_FAULT_MEMORY_DEPTH;m++ )
		{
			read_iout_vout[ch][m].read_iout_vout     = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_READ_IOUT_VOUT  + (ch*0x100) + (m*4) );
			scale_status_word[ch][m].read_scale_word = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_STAT_SCALE_WORD + (ch*0x100) + (m*4) );
			status_vout[ch][m].read_status_vout      = this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_STAT_VOUT       + (ch*0x100) + (m*4) );
		}
	}
	for( uint ch=0;ch<PPS_COUNT;ch++ )
	{
		double dVout, dIout;
		double dAttV;
		for (int i = 1; i <= 10; i += 1) {
//			int compensated_depth = (1 << 8 << ch & data->is_fault) ? (POINTER[ch] - i) : (this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_FAULT_POINTER + ch * 4) - i);
			int compensated_depth = (this->m_pio->read_user( BITCHART::PFM::PFM_S1PS1_FAULT_POINTER + ch * 4) - i);
			if (m_bFirst && compensated_depth < 0)
				continue;
			if (compensated_depth < 0)
				compensated_depth += PFM_FAULT_MEMORY_DEPTH;

			dVout = VoutToDouble( read_iout_vout[ch][compensated_depth].read_vout );
			dIout = ConvS1psIout(dVout, read_iout_vout[ch][compensated_depth].read_iout);
			if (dIout < 0) dIout = 0;
			GetS1psAttVoltage( scale_status_word[ch][compensated_depth].vout_scale_loop & 0xFFFF , dVout, dAttV );

			data->s1ps[ch].depth[i - 1] = compensated_depth + 1;
			data->s1ps[ch].present_current[i - 1] = dIout;
			data->s1ps[ch].present_voltage[i - 1] = dAttV;
			data->s1ps[ch].status_word[i - 1] = scale_status_word[ch][compensated_depth].status_word & 0xFFFF;
			data->s1ps[ch].status_vout[i - 1] = status_vout[ch][compensated_depth].status_vout & 0xFF;
//			data->s1ps[ch].status_word[i - 1] = data->s1ps[ch].status_word[i - 1] & ~STATUS_WORD_MASK;
		}
	}
}

//L16
double CMonitor::VoutToDouble(ushort L16)
{
	//const int EXP_BITS = 0x17;	//pps.hpp
	char  exponent = EXP_BITS;
	ushort mantissa = L16;
	// sign extend exponent
	if( exponent > 0x0F ) exponent |= 0xE0;
	//UL16 - unsigned mantissaif( !IsMonitoring() )
	//if( mantissa > 0x03FF ) mantissa |= 0xF800;
	// compute value as mantissa * 2^(exponent)
	return mantissa * pow(2,exponent);
}

//L11
double CMonitor::IoutToDouble(ushort L11)
{
	// extract exponent as MS 5 bits
	char exponent = L11 >> 11;
	// extract mantissa as LS 11 bits
	ushort mantissa = L11 & 0x7ff;
	// sign extend exponent from 5 to 8 bits
	if (exponent > 0x0F) exponent |= 0xE0;
	// sign extend mantissa from 11 to 16 bits
	//if (mantissa > 0x03FF) mantissa |= 0xF800;

	// compute value as mantissa * 2^(exponent)
	return mantissa * pow(2, exponent);
}

double	CMonitor::ConvS1psIout(double dVolt, int hex_iout) {
	double iout_org = 0.0;
	double iout_conv = 0.0;

	iout_org = IoutToDouble(hex_iout);
	if (dVolt > 0.1) {
		double dGain = 0.0;
		double dOffset = 0.0;
		if (!GetS1psImGainOffset(dVolt, dGain, dOffset)) {
			iout_conv = iout_org;
		}
		else {
			iout_conv = iout_org - (dVolt * dGain + dOffset);
		}
	}
	else {
		iout_conv = iout_org;
	}
	return iout_conv;
}

bool	CMonitor::GetS1psImGainOffset(double dVset, double& rGain, double& rOffset) {
	if( dVset <= 1.0 ) {
		rGain   = S1PS_IM_VSET_GAIN0;
		rOffset = S1PS_IM_VSET_OFFSET0;
		return true;
	}
	if( dVset <= 7.5 ) {
		rGain   = S1PS_IM_VSET_GAIN1;
		rOffset = S1PS_IM_VSET_OFFSET1;
		return true;
	}
	if( dVset <= 13.0 ) {
		rGain   = S1PS_IM_VSET_GAIN2;
		rOffset = S1PS_IM_VSET_OFFSET2;
		return true;
	}
	if( dVset <= 15.0 ) {
		rGain   = S1PS_IM_VSET_GAIN3;
		rOffset = S1PS_IM_VSET_OFFSET3;
		return true;
	}
	return false;
}

//bool	CMonitor::GetS1psImGainOffset(double dVset, double& rGain, double& rOffset) {
//	if( (dVset >= 0.3) && (dVset <= 1.0) ) {
//		rGain   = S1PS_IM_VSET_GAIN0;
//		rOffset = S1PS_IM_VSET_OFFSET0;
//	}
//	else if( (dVset > 1.0) && (dVset <= 7.5) ) {
//		rGain   = S1PS_IM_VSET_GAIN1;
//		rOffset = S1PS_IM_VSET_OFFSET1;
//	}
//	else if( (dVset > 7.5) && (dVset <= 13.0) ) {
//		rGain   = S1PS_IM_VSET_GAIN2;
//		rOffset = S1PS_IM_VSET_OFFSET2;
//	}
//	else if( (dVset > 13.0) && (dVset <= 15.0) ) {
//		rGain   = S1PS_IM_VSET_GAIN3;
//		rOffset = S1PS_IM_VSET_OFFSET3;
//	}
//	else {
//		return false;
//	}
//	return true;
//}

bool CMonitor::GetS1psAttVoltage( int scale, double v, double& rv)
{
	const double GAIN  [4] = { S1PS_ATT_GAIN0  , S1PS_ATT_GAIN1  , S1PS_ATT_GAIN2  , S1PS_ATT_GAIN3   };
	const double OFFSET[4] = { S1PS_ATT_OFFSET0, S1PS_ATT_OFFSET1, S1PS_ATT_OFFSET2, S1PS_ATT_OFFSET3 };

	switch ( scale )
	{
		case 0xE808:
			rv =  v* GAIN[0] + OFFSET[0];
			break;
		case 0xE804:
			rv =  v* GAIN[1] + OFFSET[1];
			break;
		case 0xE802:
			rv =  v* GAIN[2] + OFFSET[2];
			break;
		case 0xE801:
			rv =  v* GAIN[3] + OFFSET[3];
			break;
		default:
			rv = v;
			//emit SigLogMessage( m_CPU, _ERROR_LOG, tr("S1PS ScaleLoop[0x%04X] Voltage:%6.2f V invalid...")
			//					.arg(scale,0,16).arg(v) );
			return false;
			break;
	}
	return true;
}
