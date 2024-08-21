/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * TEST PROGRAM MAIN API
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef CAPI_HPP
#define CAPI_HPP

#include <QObject>
#include "testlib_global.h"
#include "bib.hpp"
#include "dbm.hpp"
#include "devsocket.hpp"
#include "dfm.hpp"
#include "dle.hpp"
#include "log.hpp"
#include "hw.hpp"
#include "lexer.hpp"
#include "pg.hpp"
#include "pps.hpp"
#include "io.hpp"
#include "protocol.hpp"
#include "sys.hpp"
#include "systype.hpp"
#include "swcontrol.h"
#include "tg.hpp"
#include "ubm.hpp"
#include "util.hpp"
#include "vector.hpp"
#include "protocol.hpp"

#define DBG_LINE	PRINTF("%s:%d---------------", __FUNCTION__, __LINE__ )

class TESTLIBSHARED_EXPORT CApi : public QObject
{
public:
    CApi();
    virtual ~CApi();
//SYSTEM - no user manual
public:
	bool HwOpen(int, int cpuno);
	bool HwInit(bool bInit=true);
	bool SwInit(bool bInit=true);
	void SetProgramName(const char*);	//2023.05.03 - kwmoon
	void SetAutoMode(bool);	//2023.01.19 - kwmoon
	bool GetAutoMode();		//2023.02.20 - kwmoon
	bool SetToolPgm(bool);
	bool ConnectToEwsMgr();		//2023.04.03 - kwmoon
	bool DisconnectToEwsMgr();	//2023.04.03 - kwmoon
	bool SysWrite(uint32_t,uint32_t);
	int  SysRead(uint32_t);
	bool sysWrite(uint32_t,uint32_t);
	int  sysRead(uint32_t);
	int  Read(uint32_t);
	bool SysWrites( uint, char*, uint32_t);
	bool SysReads ( uint, char*, uint32_t);
	bool GET_BIBID(std::string&);
	bool GET_BIBID(std::string&,std::string&,int&);
	int  GET_BIBID();
	void SetDeviceProgram(std::string);
	void initVars();
	void delVars();
	void clrStepVars();
	void SetDevStatus( PROTOCOL::DEV_STATUS stat );
	PROTOCOL::DEV_STATUS GetDevStatus();
	bool IsDuplicateFailData(uint,uint);
	void SET_UIINITINFO(stUiInitInfo);
	void SET_UITESTINFO(stUiTestInfo);
	void SET_TESTITEM(stTestItem);
	void SET_UICHMBUSE(bool);		//2023.05.22 - kwmoon
	void SET_MAKTEITEM(bool);
	void SET_RUNMODE(bool);
	bool GET_RUNMODE();
	void SET_JUMPMODE(bool);
	void SET_TESTDONE(bool);
	bool IS_TESTDONE();
	void SET_STOPMODE(bool);	//2023.05.22 - KWMOON
	bool GET_STOPMODE();			//2023.05.22 - KWMOON
	void SET_TERMINATED(bool);	//2023.04.03 - KWMOON
	bool IS_TERMINATED();		//2023.04.03 - KWMOON
	bool IS_TEST(char*);
	bool IS_SET_VARIABLES();
	bool ADD_ITEM( int, const char* );
	bool PRINT_ITEM();
	bool SEND_ITEM();
	const char* GET_STEP_NAME();
	int GET_STEP_NO();			//2023.03.16 - KWMOON
	void STEP_BEGIN();
	void STEP_END();
	void SET_CHIPID_STEP(bool);
	bool IS_CHIPID_STEP();
	void Send( int, char*, int );	//for compile
	void SET_PE_READ_ENABLE_SEL(int);	//2022.12.03 - KWMOON(PE READ ENABLE SEL)
	void SET_EDQ_SEL(uint);
	void SET_DRDLY(uint);
	void SET_IODLY(uint,uint);
	void SET_DREDLY(uint,uint);
	void SET_CPEDLY(uint,uint);
	void SET_UCLKDLY(uint);
	void SET_STROBE_ROFFSET(uint,uint);	//for system, row offset
	bool SYS_DBM_CTRL_RESET();
	bool SYS_WRITE_DBM( uint start, char* p, uint size );
	bool SYS_READ_DBM ( uint start, char* p, uint size );
	bool SYS_EXTFM_CTRL_RESET();
	bool SYS_WRITE_EXTFM( uint fc, uint64_t start, char* p, uint size );
	bool SYS_READ_EXTFM ( uint fc, uint64_t start, char* p, uint size );
	void SYS_READ_VECTOR( unsigned char* , unsigned int );
	void SYS_SEND_DEVPGM_VARS();	//2023.06.11 - KWMOON
    //2023.06.29 - KWMOON
    bool GET_CHMB_TEMP( double& sv, double& pv );
    bool GET_CHMB_STAT( bool& bRun, bool& bStop, bool& bAmb );
	//2023.08.17 - kwmoon
	void SET_MATCHCPEDLY(uint);

	//2023.11.17 - KWMOON(DCI Variable)
//	double DCI_OFFSET;	//2023-12-12 fatric
//	double DCI_USE;
	bool   DCI_USE;
	bool   DCI_IO;
	bool   DCI_UCLK;
public:
	//2023.08.02 - KWMOON
	void SET_PFM_ENABLE(bool);
	bool IS_PFM_ENABLE();
	void SET_PFM_STOP();
	void SET_PFM_START();
	bool IS_PFM_START();
	void INIT_PINS_LEVEL();		//2023.10.12 - KWMOON(CE,RE,WE,WP FIXH)
	//void SET_PFM_INI_INIT();
	//void SET_PFM_SETVS(uint, uint, double);
	//void SET_PFM_FAULT_CLEAR();
	//bool GET_PFM_FAULT(uint,uint);

private:
	std::string m_sPfmSetVsIni;
	std::string m_sPfmFalultIni;
//TEST RUN FUNCTION
public:
    virtual void programStart();
    virtual void testEntry();
	virtual void initTest();
	virtual void device_default(const char*);
    virtual void define_flag();
	virtual bool BURST( const char* pattern, int burst );
	void CLR_ALLREG_GLOBAL();
	void DEFINE_FLAG_A();
	void DEFINE_FLAG_B();
	void DEFINE_FLAG_C();
	void DEFINE_FLAG_D();
	void DEFINE_FLAG_E();
	void DEFINE_FLAG_F();
	void DEFINE_FLAG_G();
	void DEFINE_FLAG_H();
	void DEFINE_FLAG_I();
	void DEFINE_FLAG_J();
	void DEFINE_FLAG_K();
	void DEFINE_FLAG_L();
	void DEFINE_FLAG_M();
	void DEFINE_FLAG_N();
	void DEFINE_FLAG_O();
	void DEFINE_FLAG_P();
	void DEFINE_FLAG_Q();
	void DEFINE_FLAG_R();
	void DEFINE_FLAG_S();
	void DEFINE_FLAG_T();
	void DEFINE_FLAG_U();
	void DEFINE_FLAG_V();
	void DEFINE_FLAG_W();
	void DEFINE_FLAG_X();
	void DEFINE_FLAG_Y();
	void DEFINE_FLAG_Z();
	void setVersion(const char*);
	//virtual bool PAUSE( const char* pattern );
	int parseCommand( int cmd_c, char* cmd_v[] );
	void RESET_TESTER();
	int DELAY_TIME( double );
	int sysDelay( double );
	int GetCurrentCpuNo();
	int GetSiteNo();
	const char* GetLibVersion();
	int GET_FC( int dut );
	void SET_FC(int fc);
	bool IS_MYDUT(unsigned int dut );
	int ALLSYNC();
//DEVICE CONFIG
public:
	void SET_DEVICE(int, ushort device_code=0);
	void SET_ADD_SIZE( uint, uint, uint, uint, uint );
	void SET_CE( uint, IO_AB, int, uint, CE_NUM );
	void SET_CE_COND_1DUT( int dutbit_ioa, int dutbit_iob, int cecnt_ioa, int cecnt_iob);
	int GET_CE_ORDER( int mcp );
	int GET_CE_OF_BLOCK( unsigned int blockaddr );
	int GET_CE_ADDR_MCP( unsigned int mcp );
	unsigned int GET_CHIP_INDEX( int mcp , int chip );
	int GET_CHIP_OF_BLOCK( unsigned int blockaddr );
	void SET_LIMIT( uint, uint, uint );
//BIB
public:
	void SET_BIB( int scan, int io_size, int bibx, int biby, bool f16mode, bool scan_invisible, bool cpux2 = true );
	void BIB(int, ...);
	void CLR_SCAN();
	void READ_SCAN(int, int );
	void SET_SCAN(unsigned int long long, ...);
	void SET_FCSCAN_BYDUT(unsigned int dut_index);
	void SET_FCSCAN_BYDUTINDEX(unsigned int dut, int index,	unsigned int long long value );
	void SET_CONCURRENT_MODE(bool concurrent=false);
	void SET_JUDGE(int rack );
	bool IS_SITE_FAULT();
	void SET_STROBE_DELAY( double );
	void SET_STROBE_DELAY( int, double );
	void SET_STROBE_DELAY( int, int, int );
	int index_to_xy(int index, unsigned int &x, unsigned int &y);
	int index_to_siab(int index, unsigned int &rScan, unsigned int &iogrp, unsigned int &ioab );
	int xy_to_index(unsigned int x, unsigned int y, unsigned int &index);
	int xy_to_siab(unsigned int x, unsigned int y, unsigned int &scan, unsigned int &io,unsigned int &ioab);
	int si_to_index(unsigned int scan, unsigned int io, unsigned int &index, bool bib_scan = false );
	int si_to_xy(unsigned int scan, unsigned int io, unsigned int &x, unsigned int &y);
	unsigned int BIB_X();
	unsigned int BIB_Y();
	unsigned int BIB_IO();
	unsigned int NCHIP();
	unsigned int BLOCK_LIMIT();
	unsigned int MCP();

//IO
public:
	void CLR_SCANMASK();	
	void clearScanMask();
	virtual void CLR_SCANMASK(unsigned int);
	void SET_SCANMASK_ALL();
	virtual void SET_SCANMASK(unsigned int dut);
	virtual void SET_SCANMASK();
	void setScanMask(bool,uint);
	void WRITE_SCANMASK(bool,uint);
	void SET_IOMASK_GLOBAL(int index=0, long long rising=0, long long falling = 0 );
	void RESET_DRIVER();
	void SET_PINOUT( bool, bool, bool, bool, bool, bool, bool, bool );
	void SET_PINOUT_DRV ( bool, uint, uint );
	void SET_PINOUT_IOA ( bool, uint, uint );
	void SET_PINOUT_IOB ( bool, uint, uint );
	void SET_PINOUT_UCLK( bool, uint, uint );
	void SET_PINOUT_MIO ( bool, uint, uint );
	void SET_PINOUT_SCANMASK_1ST( bool, uint, uint );
	void SET_PINOUT_SCANMASK_2ND( bool, uint, uint );
	void SET_PINOUT_SCAN_INV( bool, uint, uint );
	void SET_IODDRMODE_GLOBAL(bool);
	void SET_ADDRDDRMODE_GLOBAL(bool);
	void SET_MATCHRB_GLOBAL();
//FM
public:
	void FM_WRITE_GLOBAL(bool);
	void SET_FMMODE(int fc,unsigned int mode);
	void RESET_FMMODE(int fc,unsigned int mode);
	void SET_FMMODE_GLOBAL(unsigned int mode);
	void RESET_FMMODE_GLOBAL(unsigned int mode);
	void DBG_CLR_DFM_MEMORY( int count_2048 );
	void CLR_EXTFM_GLOBAL();
	void CLR_FM_GLOBAL();
	void GET_EXTFM_INIT(int limit=EXTFM_MAX_LIMIT , bool printf=false);
	virtual int GET_EXTFM_BLOCK(int limit=EXTFM_MAX_LIMIT , int count_limit =1, bool printf=false);
	virtual int GET_EXTFM_L1_RESULT         (int limit=EXTFM_MAX_LIMIT, bool fulladdr=true, int count_limit=1, bool all_address_processing=false, int chip_limit=0, bool noceaddr=false );
	virtual int GET_EXTFM_L1_RESULT_MAJORITY(int limit=EXTFM_MAX_LIMIT,	bool fulladdr=true, int count_limit=1, bool all_address_processing=false, int chip_limit=0, bool printf=false, int addrcnt=1, int failcountrange=1 );
	virtual int GET_EXTFM_L1_RESULT_SP      (int limit=EXTFM_MAX_LIMIT,	bool fulladdr=true, int count_limit=1, bool all_address_processing=false, int chip_limit=0, bool printf=false, bool special_add_ce_addr=false);
	virtual void GET_EXTFM_DATA(int limit=EXTFM_MAX_LIMIT , bool printf=false);
	virtual void GET_EXTFM_SCAN_ADDR(int limit=EXTFM_MAX_LIMIT, bool dupcheck = true, int escape=EXTFM_MAX_LIMIT , bool printf=false);
	void SET_EXTFM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos,	unsigned int address , unsigned int scan,bool dupcheck=true);
	void SET_EXTFM_FAIL(unsigned int board, unsigned int dut, unsigned int address, unsigned int scan, bool dupcheck=true);
	unsigned int GET_EXTFM_FCNT(unsigned int dut_index );
	unsigned int GET_EXTFM_FAIL(unsigned int board, unsigned int dut, unsigned int cnt );
	int READ_EXTFM_FCNT(int fc);
	unsigned int* GET_EXTFM_FAIL_POINTER(unsigned int board, unsigned int dut);
	bool IS_EXTFM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int column, unsigned int scan);
	bool IS_EXTFM_FAIL(unsigned int board, unsigned int dut, unsigned int column, unsigned int scan);
	void READ_EXTFM_DATA(int failth, unsigned int long long data[4]);
	void getExtFailData( int failth, unsigned int long long data[4] );		//for compile & match pgm
	void READ_LATCH(int board, int ddr, unsigned int long long data[6]);
	void SET_RT_MODE_GLOBAL( int mode );
	void SET_FAILLIMIT_GLOBAL(unsigned int limit);
	void SET_FAILMULTIIO_GLOBAL(unsigned int multiio);
	int GET_NBYTE_MATCH(double &result, char *base, unsigned int chip_index, unsigned int byte_index, unsigned int max_byte);
	void GET_EXTFM_ECC_FCNT(int limit=EXTFM_MAX_LIMIT ,bool printf=false , bool dont_clear=false);
	void CLR_CHUNK_FCNT();
	void GET_EXTFM_TMPFM(int limit=EXTFM_MAX_LIMIT , bool printf=false);
	void GET_EXTFM_DUTFM( int limit=EXTFM_MAX_LIMIT , bool printf=false);
	void SET_EXTFM_ECC_GLOBAL(int fail_limit=0, int multi_io=0x0, bool fail_count_save_mode=false);
	unsigned short GET_CHUNK_FCNT( unsigned int dut_index , int chunk );
	void SET_EXTFM_AUTOMASK_GLOBAL(int fail_limit=0, int multi_io=0x0);
	unsigned char TMP_FM_DATA( int board, unsigned int io, unsigned index );
	bool GET_IO_GRP( uint fc, uint ioab, uint iopin, uint& iogrp );
//OUT
public:
	void out_init_result();
	void out_fm_read_data();
//DRV
public:
	void SET_DRV_IOMODE( bool enb=false);
	int READ_DRV_FCNT(int fc);
	int READ_DRV_SCAN(int fc,int failorder);
	int READ_DRV_DATA(int fc, int failth);

//UBM
public:
	void SET_UDATA_0x00();
	void SET_UDATA_0xFF();
	void SET_UDATA32( unsigned int fc, int index, int index2, unsigned int data );
	void SET_UDATA( unsigned int fc, unsigned int ioab, unsigned int index, unsigned long long int data );
	void SET_UDATAOR( unsigned int fc, unsigned int ioab, int index, unsigned long long int data );
	unsigned int GET_UDATA32( unsigned int fc, int index, int index2 );
	unsigned long long GET_UDATA( unsigned int fc, unsigned int ioab, int index );
	void PRINTF_UBM( unsigned int, unsigned int size=UBM_DEPTH );
	virtual int SET_UBM( unsigned int dut );
	virtual void SET_UDATADUT( unsigned int	dut_index , int real_scan, unsigned int cnt, unsigned int data , int jump );
//VECTOR
public:
	void CLR_VECTOR();
	void SET_VECTOR_DATA( unsigned char* , unsigned int );
	void SET_VECTOR_DATA( unsigned short*, unsigned int );
	void SET_VECTOR     ( unsigned int*  , unsigned int );
	void PRINTF_VECTOR  ( unsigned int, unsigned int, unsigned int );
//DBM
public:
	void CLR_DBM();
	void SET_DBM_DATA( unsigned char*, unsigned int size, bool ddr = false );
	void SET_DBM_DATA( unsigned short*, unsigned int size, bool ddr = false );
	void SET_DBM( unsigned int, unsigned int, ... );
	void PRINTF_DBM( unsigned int, unsigned int, unsigned int );
	void WRITE_DBM(unsigned int, unsigned int );
	void setDbm( uint, uint );
	uint getDbm( uint );
//ECMD
public:
	void CLR_ECMD();
	void SET_ECMD(unsigned int index, unsigned long long ecmd_data );
	void SET_ECMD_DATA(unsigned long long *s, unsigned int size);
	void PRINTF_ECMD(unsigned int fc, unsigned int start, unsigned int count );
//SCRAMBLE
public:
	void SCRAMBLE (const char *);
	void SET_SCRAMBLE (const char *grp);
//SCAN MASK
public:
	void CLR_SCM();
	void SET_SCM_DATA(int dut, unsigned int realscan_blockaddr );
	void PRINTF_SCM(unsigned int fc, unsigned int start, unsigned int count );

//PG(PATTERN)
public:    
	bool SET_PATTERN();
	void SET_JNCFLAG(uint);
	bool MEAS_MPAT( const char* name, double wait=-1.0 );
	bool START_MPAT( const char* name, double wait= 0.0 );
	void WAIT_MPAT (                   double wait=-1.0 );
	void STOP_MPAT();
	void CONTINUE_MPAT();
	bool IS_PGPAUSE();
	uint GET_PGRUNTIME();
	void CLEAR_PGRUNTIME();
	virtual bool PAUSE( const char* pattern );
private:
	bool OUT_MPAT(uint);
//TG
public:
	void RATE( double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15 );
	void SET_RATE(int,double);
	void CLK ( TS_CLOCK_LIST, double r0 , double r1, double r2, double r3, double r4, double r5, double r6, double r7, double r8, double r9, double r10, double r11, double r12, double r13, double r14, double r15 );
	void SET_CLK( int, int, double );
	void SET_CLK_DELAY( int pin, double tdelay );

//CYP
public:
	void SET_CYP();
//MODULE(FMT)
public:
	void SET_MODULE(bool);
//TEMPERATURE
public:
	void SET_TEMP( const double sv, double wait, double soak, bool bprint = false );

//POWER(PPS/DAC)
public:
    void SET_VOUT_RANGE( uint, double, double );
	void SET_VOUT_RANGE( uint, uint, uint );		//Ablee api
	void SET_VS( uint unit, uint ch, double v, double ov, double uv, double oc );
	//void SET_VS_BUMP( uint unit, uint ch, double v, double ov, double uv, double oc );
	void SET_VS_ON( uint, int );
    void SET_VS_OFF( uint );
	double READ_VS( uint, uint );
	void SET_VIH(double);
	void SET_VIH_ON(double);
	void SET_VTH(double);
	void SET_MIOVOLT(double, double);	//VIH2, VOH2
	void SET_VIH2(double, double);					//VIH2, VOH2
public:
	void SET_MIO_VIH(double);						//For Debug
	void SET_MIO_VTH(double);						//For Debug
	void SET_COMP_IN(double);						//For Debug
	void SET_DAC_VOUT(int,double);					//For Debug
	bool MEAS_VIH(double&);							//For Debug
	bool MEAS_VTH(double&);							//For Debug
	bool MEAS_VIH2(double&,double&);				//For Debug
	bool MEAS_DRV_OUT(double&);						//For Debug
	double READ_PPS_VF  ( uint, uint );					//For Debug
	double READ_PPS_VOUT( uint, uint );					//For Debug
	double READ_PPS_IOUT( uint, uint );					//For Debug
	double READ_PPS_GND ( uint );						//For Debug
	//double GET_S1PS_ATT_VOLTAGE(double);			//For Debug
	void CLEAR_FAULTS(uint,int);					//For Debug
	int  READ_ALERT(uint);							//For Debug
	bool READ_STATUS_BYTE(uint,int,R_STATUS_BYTE&);	//For Debug
	bool READ_STATUS_WORD(uint,int,R_STATUS_WORD&);	//For Debug
	bool READ_STATUS_VOUT(uint,int,R_STATUS_VOUT&);	//For Debug
	bool READ_STATUS_IOUT(uint,int,R_STATUS_IOUT&);	//For Debug - 2022.12.14
	void SET_DGS_HIGH  ( uint, bool );				//For Debug
	void SET_SENSE_HIGH( uint, bool );				//For Debug
	int READ_SCALELOOP(uint,uint);					//For Debug - 2022.12.06
//TEMPERATURE
public:
	void SET_BITIME( int );
	void SET_AGING ( int );
//STEP
public:
	void SET_STEPCNT(int,int);

//FROM MAIN
public:
	std::string GET_PART_NAME();
	std::string GET_LOT_NAME();
	int GET_BLOCK_DATA(const char *step_name);
//TO MAIN
public:
	void SET_FM_DATA(unsigned int board, unsigned int dut, unsigned int chip_index,	unsigned int byte_index, unsigned char value);
	void SET_FM_DATA(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index,unsigned int byte_index, unsigned char value);
	unsigned char FM_DATA(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int byte_index);
	unsigned char FM_DATA(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int byte_index);
	int FM_READ_RESULT();
//FAIL CONFIG
public:
	void SET_FAILMODE( int mode );
	virtual int SET_PF_CE (unsigned int);
	virtual int SET_PF_REAL_CHIP (unsigned int mode);
	void SET_FAULTBIN(int);
	void SET_PRE_FAIL(unsigned int board, unsigned int dut, bool value);
	void SET_PRE_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value);
	bool IS_PRE_FAIL(unsigned int board, unsigned int dut);
	bool IS_PRE_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos);
	void SET_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block, bool value);
	void SET_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value);
	bool IS_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block);
	bool IS_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block);
	char IS_STEP_BLOCK_FAIL8(unsigned int board, unsigned int dut, unsigned int block);
	int STEP_BLOCK_FAIL_COUNT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int mcp,unsigned int minaddr , unsigned int	maxaddr);
	int STEP_BLOCK_FAIL_COUNT(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr , unsigned int maxaddr );
	int STEP_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int mcp, unsigned int minaddr, unsigned int maxaddr,bool cdwrite=false);
	int STEP_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int dut, unsigned int mcp,	unsigned int minaddr , unsigned int maxaddr,bool cdwrite=false);
	void SET_CUM_EXCL_BLOCK(bool exclusive_cum);
	bool IS_CUM_EXCL_BLOCK();
	void SET_CUM_BLOCK_FAIL (unsigned int board, unsigned int dut, unsigned int block, bool value);
	void SET_CUM_BLOCK_FAIL (unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value);
	bool IS_CUM_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block);
	bool IS_CUM_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block);
	bool IS_CUM_BLOCK_FAIL8(unsigned int board, unsigned int dut, unsigned int block);
	int	CUM_BLOCK_FAIL_COUNT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int mcp,unsigned int minaddr , unsigned int maxaddr );
	int CUM_BLOCK_FAIL_COUNT(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr , unsigned int maxaddr );
	//int CUM_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int x_pos, unsigned int	y_pos, unsigned int mcp, unsigned int minaddr, unsigned int maxaddr,bool cdwrite=false);
	int	CUM_BLOCK_FAIL_ADDRESS(unsigned int board, unsigned int dut, unsigned int mcp, unsigned int minaddr , unsigned int maxaddr,bool cdwrite=false);
	void CLR_CUM_BLOCK_FAIL();
	void SET_ADD_LIMIT_STEP ( int block_add_limit_step );
	void SET_INIT_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value);
	void SET_INIT_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block, bool value);
	bool IS_INIT_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block);
	bool IS_INIT_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block);
	char IS_INIT_BLOCK_FAIL8(unsigned int board, unsigned int dut, unsigned int block);
	void SET_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block, bool value);
	void SET_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block, bool value);
	bool IS_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int block);
	bool IS_GT_STEP_BLOCK_FAIL(unsigned int board, unsigned int dut, unsigned int block);
	void SET_STEP_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, bool value);
	void SET_STEP_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index, bool value);
	bool IS_STEP_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index);
	bool IS_STEP_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index);
	void SET_CUM_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, bool value);
	void SET_CUM_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index, bool value);
	bool IS_CUM_FAIL_CHIP(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index);
	bool IS_CUM_FAIL_CHIP(unsigned int board, unsigned int dut, unsigned int chip_index);
//DUT FAIL
public:
	void SET_STEP_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value);
	void SET_STEP_FAIL(unsigned int board, unsigned int dut, bool value);
	bool IS_STEP_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos);
	bool IS_STEP_FAIL(unsigned int board, unsigned int dut);
	void SET_CUM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value);
	void SET_CUM_FAIL(unsigned int board, unsigned int dut, bool value);
	bool IS_CUM_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos);
	bool IS_CUM_FAIL(unsigned int board, unsigned int dut);
	void SET_TEST_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos, bool value);
	void SET_TEST_FAIL(unsigned int board, unsigned int dut, bool value);
	bool IS_TEST_FAIL(unsigned int board, unsigned int x_pos, unsigned int y_pos);
	bool IS_TEST_FAIL(unsigned int board, unsigned int dut);
//CHIP ID
public:
	void INIT_READ_CHIPID();
	int RESULT_CHIPID_DATA(unsigned int mode);
	void SET_FM_READ_DATA_INDEX(unsigned int v);
	void SET_CHIPID_V(unsigned int board, unsigned int dut, unsigned int chip_index, char *value);
	void SET_CHIPID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, char* val);
	void SET_WAFERID_V(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int value);
	void SET_WAFERID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int value);
	void SET_XPOS_V(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int value);
	void SET_XPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int value);
	void SET_YPOS_V(unsigned int board, unsigned int dut, unsigned int chip_index, unsigned int value);
	void SET_YPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index, unsigned int value);
	char *CHIPID_V(unsigned int board, unsigned int dut, unsigned int chip_index);
	char *CHIPID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index);
	int WAFERID_V(unsigned int board, unsigned int dut, unsigned int chip_index);
	int WAFERID_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index);
	int XPOS_V(unsigned int board, unsigned int	dut, unsigned int chip_index);
	int XPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index);
	int YPOS_V(unsigned int board, unsigned int	dut, unsigned int chip_index);
	int YPOS_V(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int chip_index);
//NAND CHIP ID
public:
	void SET_READ_NAND_CHIPID();
	void READ_CHIP_DATA(unsigned short chip_count, unsigned short data_length, unsigned short chip_size, const char *data_type , int chipid_shfit);
	virtual int SET_FM_READ_DATA(unsigned int scan, unsigned int block_addr, unsigned int fm_data_read_index = 0);

//DRAM CHIP ID
public:
	void DRAM_READ_CHIP_DATA(unsigned short chip_count, const char *data_type);
	virtual int SET_DRAM_READ_CHIPID(unsigned int index = 0);
//EMMC CHIP ID
public:
	void EMMC_READ_CHIP_DATA(unsigned short chip_count, const char *data_type);
	virtual int SET_EMMC_READ_DATA(unsigned int index = 0);
	void SET_EMMC_CHIPID_UNIT_COUNT(unsigned short v) ;
	unsigned short EMMC_CHIPID_UNIT_COUNT();
	void SET_EMMC_CE(unsigned short v) ;
	unsigned short EMMC_CE() ;
//CATEGORY
public:
	void SET_FCAT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int fail_cate);
	void SET_FCAT(unsigned int board, unsigned int dut, unsigned int fail_cate);
	void SET_PCAT(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int pass_cate);
	void SET_PCAT(unsigned int board, unsigned int dut, unsigned int pass_cate);
	bool FCAT( int board, unsigned int x_pos, unsigned int y_pos, unsigned int fail_cate,unsigned int fail_cate_end);
	bool FCAT( int board, unsigned int dut, unsigned int fail_cate, unsigned int fail_cate_end);
	bool PCAT( int board, unsigned int x_pos, unsigned int y_pos, unsigned int pass_cate,unsigned int pass_cate_end);
	bool PCAT( int board, unsigned int dut, unsigned int pass_cate, unsigned int pass_cate_end);
	void SET_BIN(unsigned int board, unsigned int x_pos, unsigned int y_pos, unsigned int bin);
	void SET_BIN(unsigned int board, unsigned int dut, unsigned int bin);
//EMMC ONLY
public:
	void SET_EMMC_TEST_MODE( EMMC_TEST_MODE testmode = BISTX1 );
	unsigned int GET_EMMC_TEST_MODE();
	void SET_EMMC_BIT_VALUE(unsigned char v );
	void SET_EMMC_EXPECT(unsigned int bit, unsigned long long value = 0llu);
	unsigned int EMMC_EXPECT_BIT( );
	unsigned int long long EMMC_EXPECT_VALUE();
	void SET_EMMC_COMPARE(unsigned int bit);
	void SET_EMMC_COMPARE(unsigned int bit, unsigned long long value);
	unsigned int EMMC_COMPARE_BIT();
	unsigned int long long EMMC_COMPARE_VALUE();
	void SET_EMMC_COMPARE_STRING(const char* compare_string);
	unsigned short EMMC_COMPARE_STRING_SIZE();
	char* EMMC_COMPARE_STRING();
	void GET_EXTFM_EMMC_IO(int limit=EXTFM_MAX_LIMIT, int mode=0, int format=0, bool printf=false);
	void GET_DRV_EMMC_IO (int drvdepth, int mode=0, int format=0, bool printf=false);

//unknown virtual function
public:
	virtual int CID_X_CAL(int x);
	virtual int CID_Y_CAL(int y);
	virtual int CID_WAFER_CAL(int wafer);
	virtual std::string CID_LOTID_CAL(std::string lotid);
	virtual void SET_BLOCK(int addr, int s, int& address, int &scan );
	virtual void SET_COLUMN (int addr, int &address);
	virtual void SET_CHUNK (int addr, int scan , int &chunk );
	virtual bool RUNTIME_ERROR( const char*, const char *, ...);
	virtual int OPTION_CALL(int command, int wparam = 0, int lparam = 0);
	virtual int DO_EMMCDATA_FMT_USER( int format=0);
//DEBUG
public:
	//void PRINT  ( const char* szFmt, ... );	//2023.08.24 - remove
	void PRINTF ( const char* szFmt, ... );
	void EPRINT ( const char* szFmt, ... );
	void EPRINTF( const char* szFmt, ... );
	void SET_PRINTF_DUT( bool enb, int printf_dut );
	void SET_PRINTF_PAT( bool enb );
	void PRINTF_REG();
	void PRINTF_VOLTAGE();
	void DBG_DUMP_PATTERN();
	void DBG_DUMP_TIMESET();
	void PRINTF_REG_WITH_M(unsigned int fc =0 , int start=0 , int end=3);
	void PRINTF_PD(unsigned int fc =0 );
	void PRINTF_EXTFM(int fc , int limit = 1024 );	//limit check
	void PRINTF_LATCH(int fc);
//ETC
	int GET_IOA_DUTBIT();
	int GET_IOB_DUTBIT();
	int GET_IOA_DUTMASK();
	int GET_IOB_DUTMASK();
	int GET_DUTPERFC();
	int GET_DUTPERCPU();
	void sysResetLatch();
	void setFMClear();
//	void setScanMask( bool, int );
	void CLR_AUTOMASK_MEM_GLOBAL();
	void WRITE_AUTO_SCANMASK(bool,uint);

	//2023.06.13 - kwmoon(add fatric)
	void MULTIBIN_SET(int iBin10, int iBin11=-1, int iBin12=-1, int iBin13=-1, int iBin14=-1, int iBin15=-1);
	void SET_BIN_REF1(double dBin1     , double dBin2 =-1.0, double dBin3 =-1.0, double dBin4 =-1.0, double dBin5 =-1.0, double dBin6 =-1.0, double dBin7 =-1.0, double dBin8=-1.0,
					  double dBin9=-1.0, double dBin10=-1.0, double dBin11=-1.0, double dBin12=-1.0, double dBin13=-1.0, double dBin14=-1.0, double dBin15=-1.0 );

private:
	std::string m_sDeviceProgram;
	std::string m_sStepName;
	PROTOCOL::DEV_STATUS m_DevStat;
	uint m_nStepNo;
	uint m_nCurrentStepNo;
public:
	double m_dTmpTimeSum;
	int m_FCAdd;
	int m_fail_mode;
	int dut_per_cpu;
	int m_set_step_count;
	int m_set_merge_type;
	int m_scan;
	int m_bibScan;
	int m_bibIo;
	int m_bib_x;
	int m_bib_y;
	int m_scan_invisible;
	bool m_bib16mode;
	uint m_bibCpu;
	bool m_bReadNandChipId;
	uint m_nFmReadDataIndex;
	int m_nchip;
	int m_block_shift;
	int m_block_limit;
	int m_index_si_map[MAX_DUT];
	int _ceaddress[_IOAB][MAX_CE];
	int m_ioab_cnt[_IOAB][1024];
	int dut_per_fc;
	int _CurrentCpuNo;
	int m_scan_mask[USE_FC][SCAN_MEM_DEPTH][3];
	int m_use_mode;	//temp for compile
	unsigned char m_tmp_fm_data[MAX_NSLOT][MAX_IO][1024];		//temp for compile
	unsigned int m_mcp;
	bool m_ioabboth;	//for compile
	uint** m_ext_fm_fail;
	uint* m_ext_fm_fail_count;
	CPSClient m_PSClient;		//for compile & meas
	bool RUNMODE;
	bool MAKEITEM;
	bool JUMPMODE;				//20230222 - kwmoon
	bool TESTDONE;				//20230227 - kwmoon
	bool STOPMODE;
	bool TERMINATED;			//20230403 - kwmoon
	stTestItem m_stMakeItem;
	stTestItem m_stTestItem;
	stDevPgmVariables m_stPgmVariables;	//20230308 - kwmoon
	stUiInitInfo m_stUiInitInfo;		//Board Id
	stUiTestInfo m_stUiTestInfo;		//Auto/ChmbUse
public:
	stBiFind _bi_find[MAX_SLOT];		//for compile
	stChipData m_stChipData;
	stChipId*  m_pStChipId;
	stSetTemp  m_stSetTemp;
	int        m_nBiTime;
	int        m_nAgingTime;
	int        m_nFaultBin;
	char       m_szPc[MAX_TEXT_VALUE];
	
	//2023.04.21 - fatric
	int        m_iPpsStat;
	int        m_iPinOutDrStat;
	int        m_iPinOutUclkStat;

	//2023.05.03 - kwmoon
	char m_szProgramName[MAX_TEXT_VALUE];
public:
	//SET_LIMIT
	uint m_lmtBlkPerDut;
	uint m_lmtBlkPerCe;
	uint m_lmtBlkPerCeStep;

	uint m_lmtAddFail;			//PerCE

	//SET_ADD_SIZE
	uint m_addBlkShift;
	uint m_addPageShift;
	uint m_addColSize;
	uint m_addMaxCe;
	uint m_addMaxChip;

	uint m_nMaxDutPerCpu;
	uint m_nCePerDut;
	uint m_nChipPerCe;
	uint m_nBlockPerCe;
	uint m_nBlockPerChip;
	uint m_nBlockPerDut;
	uint m_nBlockPerDutBl;
	uint m_nChipPerDut;
	uint m_nChipPerDutBl;
	int m_iVarConf;
	int m_bVarAvail;
	int m_nBitCnt[256];
	int m_nBitPos[256];
	char* m_init_fail          ;
	char* m_init_fail_block    ;
	char* m_now_step_fail_block;
	char* m_cum_fail_block     ;
	char* m_cum_fail_chip      ;
	char* m_get_step_fail_block;
	char* m_step_fail_chip     ;
	char* m_test_fail          ;
	char* m_step_fail          ;
	char* m_cum_fail           ;
	uint  m_nCatePerDut        ;
	char* m_fail_cate          ;
	char* m_pass_cate          ;
	uint* m_bin                ;

	//2024.07.22 fatric
	uint* m_step_fail_cate     ;
	uint* m_step_pass_cate     ;

	//2023.06.13 - kwmoon(add fatric)
	bool  m_cum_exclusive      ;
	uint  m_bin_map[16]        ;	//BIN1 ~ BIN15
	uint  m_bin_limit[16]      ;	//BIN1 ~ BIN15
	uint* m_step_fail_block_cnt;	//2023.07.12 - fatric

public:
	int  m_debug_mode;
	bool m_bBlockFailAddr;
	bool m_bInitStep;
	bool m_bProcExtFmBlock;	//2023.05.12 - fatric

	void SET_DEBUG_MODE(int mode);
	void initTestEnd();
	void DISPLAY_PREFAIL_DUTMAP();
	void DISPLAY_STEPFAIL_DUTMAP();
	void DISPLAY_LATCH();
	void DISPLAY_FCAT();
	void DISPLAY_EXTFM_FCNT();
	void DISPLAY_BLOCK_FAIL_ADDRESS();
	void DISPLAY_BLOCK_FAIL_ADDRESS_CE();
	void DISPLAY_BLOCK_FAIL_ADDRESS_STACK();
	void DISPLAY_STEP_FAIL_CHIP();
	void DISPLAY_CUM_BLOCK_FAIL_ADDRESS_STACK();
	void DISPLAY_CHIP_INFO();
	void DISPLAY_CHUNKFAIL_CNT();	//2023.04.21 - fatric

private:
	int  UnitMajority(std::vector<uint>	&vBlkAddr    , uint uAddrCnt);
	bool IsSetBlk    (std::vector<int > &lstSettedBlk, int  iBlk    );

public:
	std::vector<std::string>				vecStepName;
	stCommonData							m_stCommonData;
	std::chrono::steady_clock::time_point	m_tpStepBegin;
	std::chrono::steady_clock::time_point	m_tpStepEnd;
	std::chrono::steady_clock::time_point	m_tpCycleBegin;		//2023.03.27 - KWMOON
	std::chrono::steady_clock::time_point	m_tpCycleEnd;		//2023.03.27 - KWMOON
	time_t									m_tStepBeginTime;
	time_t									m_tStepEndTime;
	std::vector<CStepData*>					m_vStepData;
	uint									m_iTsNo;

public:
	void SetInitStep(bool bInit=true);
	bool IsInitStep();
	void CYCLE_BEGIN();
	void CYCLE_END();
	bool ClearCommonData();
	bool ClearInitFailData();
	bool ClearChipData();
	bool ClearStepName();
	bool ClearTestFailData();
	bool ClearStepFailData();
	bool ClearStepFailBlockData();
	bool ClearStepFailChipData();
	bool ClearCumFailChipData();
	bool ClearCumFailBlockData();
	bool ClearStepTimeStamp();
	bool ClearSortBinData();
	bool ClearStepData();
	bool ClearProcGetExtFmBlock();	//2023.05.12 - fatric
	bool ClearTmpFiles();
	bool AppendStepFailData();
	bool AppendStepFailBlockData();
	bool AppendStepFailChipData();
	bool AppendStepTimeStamp();

	bool AppendFCateData();		//2024.07.22 fatric
	bool AppendPCateData();		//2024.07.22 fatric
	bool AppendStepFailMode();	//2024.07.22 fatric
	bool AppendStepUseMode();	//2024.08.18 fatric

	bool CreateInitFailData();
	bool CreateTestFailData();
	bool CreateCumFailChipData();
	bool CreateCumFailBlockData();
	bool CreateChipData();
	bool CreateCommonData();
	bool CreateStepName();
	bool CreateSortBinData();
	bool CreateBinmapData();	//2024.07.22 fatric
	bool CreateStepData();
	bool CreateCycleEnd(bool bEnd, int iCpu=-1);
	bool CreateCumFailCate();		//2024.06.24 - KWMOON
	bool AppendProcExtFmBlock();	//2023.05.12 - fatric
	void sendSortBinData();	//20240810 hsbae for test

	bool getStepName          (std::vector<std::string> &vStepName                              , int iCpu=0 );
	bool getChipData          (stChipId     *pstChipId                                          , int iCpu=0 );
	bool getCommonData        (stCommonData &stCommonData                                       , int iCpu=0 );
	bool getInitFailData      (char         *pInitFail                                          , int iCpu=0 );
	bool getTestFailData      (char         *pTestFail                                          , int iCpu=0 );
	bool getStepFailData      (int nStep, int nSize                  , char *pStepFail          , int iCpu=0 );
	bool getStepFailDataT     (int nStep, int nSize                  , char *pStepFail          , int iCpu=0 );
	bool getStepFailBlockData (int nStep, int nSize                  , char *pStepBlockFail     , int iCpu=0 );
	bool getStepFailBlockDataT(int nStep, int nSize                  , char *pStepBlockFail     , int iCpu=0 );
	bool getCumFailBlockData  (           int nSize                  , char *pCumBlockFail      , int iCpu=0 );
	bool getStepFailChipData  (int nStep, int nSize                  , char *pStepFailChip      , int iCpu=0 );
	bool getStepFailChipDataT (int nStep, int nSize                  , char *pStepFailChip      , int iCpu=0 );
	bool getCumFailChipData   (           int nSize                  , char *pCumFailChip       , int iCpu=0 );
	bool getStepTimeStampData (int nStep, std::vector<time_t> &vBegin, std::vector<time_t> &vEnd, int iCpu=0 );
	bool getSortBinData       (uint                    *pSortBin                                , int iCpu=0 );
	bool getStepData          (std::vector<CStepData*> &vStepData                               , int iCpu=0 );
	bool getCycleEnd          (bool &bEnd                                                       , int iCpu=0 );
	bool getProcExtFmBlock    (           int nSize                  , bool *pProcExtFmBlock    , int iCpu=0 ); 	//2023.05.12 - fatric
	bool IsChipFail    (char *pFailChip, int iDut, int iCe, int iChip, int iStep=0   );
	bool IsChipFail    (char *pFailChip, int iDut,                     int iStep=0   );
	bool IsPreFail     (char *pPreFail , int iDut                                    );
	bool IsTestFail    (char *pTestFail, int iDut                                    );
	bool IsStepFail    (char *pStepFail, int iDut,                     int iStep     );
	bool IsAllStepPass (char *pStepFail, int iDut,                     int nStep     );
	uint StepFailCnt   (char *pStepFail, char *pPreFail,               int iStep     );
	void getFailBlockInfo(std::vector<uint> &vBlkAddr, char *pFailBlock, int iDut, int iCe, int iChip, int iStep=0);
	bool IsChipFailT   (char *pFailChip, int iDut, int iCe, int iChip, int iStep=0   );
	bool IsChipFailT   (char *pFailChip, int iDut,                     int iStep=0   );
	bool IsPreFailT    (char *pPreFail , int iDut                                    );
	bool IsTestFailT   (char *pTestFail, int iDut                                    );
	bool IsStepFailT   (char *pStepFail, int iDut,                     int iStep     );
	bool IsAllStepPassT(char *pStepFail, int iDut,                     int nStep     );
	uint StepFailCntT  (char *pStepFail, char *pPreFail,               int iStep     );
	void getFailBlockInfoT(std::vector<uint> &vBlkAddr, char *pFailBlock, int iDut, int iCe, int iChip, int iStep=0);
	void mergeFailBlockInfo(std::vector<uint> &vCumBlkAddr, std::vector<uint> &vBlkAddr);
	bool IsIn(std::vector<uint> &vCumBlkAddr, uint iBlkAddr);
	bool GenCumFailBlock();
	bool GenFls();
	bool IsCycleEndOther(int iCpu);
	bool GenTot();
	bool GenCumFailBlockT();
	bool GenFlsT();
};

#endif // API_HPP
