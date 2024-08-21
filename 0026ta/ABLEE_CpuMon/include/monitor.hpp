#ifndef CMONITOR_HPP
#define CMONITOR_HPP

#include <QObject>
#include <math.h>
#include <QDebug>
#include <chrono>
#include "bitchart.hpp"
#include "commstruct.hpp"
#include "io.hpp"
#include "pps.hpp"
#include "struct.hpp"
#include "sysini.hpp"
#include "sysdef.hpp"
#include "syslog.hpp"
#include "./../../ABLEE_Common/tool/util.hpp"

//#define MONITORING_PERIOD				( 6400 MS)
#define MONITORING_PERIOD				( 100 MS) // hsbae 1000MS -> 100MS
#define STATUS_VOUT_MASK				( 0b01101011)
#define STATUS_WORD_MASK				( 0b1111111111000011)
//#define STATUS_VOUT_MASK				( 0x02)
//#define STATUS_WORD_MASK				( 0x8841)
#define PFM_FAULT_MEMORY_DEPTH			( 64 )

class CMonitor : public QObject
{
	Q_OBJECT
public:
	explicit CMonitor(uint, uint, QObject *parent = nullptr);
	~CMonitor();
private:
	bool OpenHW();
	bool CloseHW();
	bool SetFaultMask( uint, UFAULT_MASK );
	bool IsMonitoring();
	bool GetS1psAttVoltage(int scale, double v, double& rv);
	bool GetS1psImGainOffset(double dVset, double& rGain, double& rOffset);
	double	ConvS1psIout(double dVolt, int hex_iout); // hsbae
	double VoutToDouble(ushort);
	double IoutToDouble(ushort);
	void ClearMemory();
	void Run();
	void PpsMonitoring(pfm_data *data);
	void S1psMonitoring(pfm_data *data);

	void PpsUnitMonitoring(pfm_unit_data *data);
	void S1psUnitMonitoring(pfm_unit_data *data);

	void readTPGInfo();
public:
	void SetStop();

signals:
	void SigOpenHWResult(uint,bool);
	void SigRunResult(uint,bool);
	void SigLogMessage( uint, LOG_TYPE, QString );
	void sigTCPSend(char* data, int data_length);

public slots:
	void SlotOpenHW();
	void SlotRun(uint cpu);
private:
	uint m_CPU;
	uint m_SLOT;
	CPcie* m_pio;
	bool m_bOpen;
	bool m_bRun;
	bool m_bFirst;
	int m_pps_depth[8];
	int m_s1ps_depth[8];
	UFAULT_MASK m_FAULT_MASK;
	UPFM_STAUS m_PFM_STATUS;
};

#endif // CMONITOR_HPP
