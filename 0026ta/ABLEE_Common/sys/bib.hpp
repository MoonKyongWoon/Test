/////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * BIB/SCAN system api
 */
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef CBIB_HPP
#define CBIB_HPP

#include "testlib_global.h"
#include "basestruct.hpp"
#include "dutmap.hpp"
#include "sysdef.hpp"
#include "swcontrol.h"
#include <string>
#include <map>
#include <unordered_map>
#include <vector>

#define MAX_BIB_SEL			(0x4)
enum BIB_SEL
{
	SEL0    = (0x1<<0),
	SEL1    = (0x1<<1),
	SEL2    = (0x1<<2),
	SEL3    = (0x1<<3),
};

class TESTLIBSHARED_EXPORT CDut
{
public:
	explicit CDut(int, int, int);
	int  GetSi();
	uint GetDutNo();
	uint GetSiScanNo();
	uint GetSiIoGroup();
	uint GetSiCpuNo();
	uint GetSiFcNo();
	uint GetSiIoAb();
	uint GetX();
	uint GetY();
public:
	void SetSiInfo(int,uint,uint,uint,uint,uint);
	void SetIoSize(uint);
private:
	int  m_nSi;
	uint m_X;
	uint m_Y;
	uint m_DutNo;
	uint m_nSiScanNo;
	uint m_nSiIoGroup;
	uint m_nSiCpuNo;
	uint m_nSiFcNo;
	uint m_nSiIoAb;
private:
	uint m_uIoSize;
};

class TESTLIBSHARED_EXPORT CBib
{
public:
    CBib();
	~CBib();
public:
	bool GetBibId( std::string&, std::string&, int&  );
	int  GetBibId();
	int  GetBibIf();
	void SetBibIf(int,int);
public:
	uint GetBlockLimit();		//BLOCK SIZE
	uint GetCeBlockLimit();		//CE BLOCK SIZE
	uint GetChipBlockLimit();	//CHIP BLOCK SIZE
	uint GetFailBlockLimit();	//FAIL BLOCK LIMIT
	uint GetFailAddressLimit();	//FAIL ADDR  LIMIT
	uint GetBlockShift();
	uint GetPageShift();
	uint GetColumnSize();
	uint GetCeCount();
	uint GetChipCount();
	int  GetBibX();
	int  GetBibY();
	int  GetBibIo();
	bool IsBibF16Mode();
	bool IsBibScanInvisible();
	bool IsBib2Cpu();
	bool GetFc(int dut, int& fc);
	bool IsMyDut( BIB_CPU cpu, int dut, bool& mydut );
	bool GetCeOrder( uint mcp, uint& ce_order );
	bool GetCeOfBlock( uint blockaddr, int& ce );
	bool GetCeAddrMcp( uint mcp, int& ce );
	bool GetXYOfDut( int dut, uint& x, uint& y );
	bool GetSiabOfDut( int dut, uint& scan, uint& iogrp, uint& ioab );
	bool GetXyToIndex( uint x, uint y, uint& index );
	bool GetXyToSiab( uint x, uint y, uint& scan, uint& iogrp, uint& ioab );
	bool GetSiToIndex( uint scan, uint iogrp, uint& index, bool bib_scan=false );
	bool GetSiToXy( uint scan, uint iogrp, uint& x, uint& y );
	bool GetIoGrp( uint cpu, uint fc, uint ioab, uint iopin, uint& iogrp );
	int  GetSi(int);
	uint GetDutPerFc();
	uint GetDutPerCpu();
	uint GetDutPerIoAb();
//BIB ID / BIB IF
private:
	void SetBibSel(BIB_SEL);
	bool ValidBibParity(int);
	void SetScanOr();
	void SetScanAnd();
//SET_SCAN
public:
	void ClearScan();
	void SetScan( int, unsigned int long long );
	void GetScan( int, int );
	bool WriteScan();
    void ClearBibSet();
    void SetBib( int, int, int, int, bool, bool, bool );
//BIB
public:
	bool Bib( int index, int si );
	void ClearBib();
//SET_ADD_SIZE
public:
	void SetAddSize( uint, uint, uint, uint, uint );
//SET_LIMIT
public:
	void SetLimit( uint block, uint faillimit, uint blocklimit );
//SET_CE
public:
	void ClearSetCe();
	bool SetCe( uint, uint, uint, uint, uint );
//SET_CE_COND_1DUT
public:
	bool SetCeCond1Dut( uint, uint, uint, uint );
//SCAN MASK
public:
	void ClrScanMask();
	void SetScanEnableAll();
	void SetScanMaskAll();
	void WriteScanMask( uint, uint, uint, int );
private:
	int m_nMemOffset;
	int m_nBibId;
	unsigned long long int m_ScanMem[SCAN_MEM_DEPTH];
	unsigned long long int m_ScanOr;
	unsigned long long int m_ScanAnd;
	stBibSet m_stBibSet;
	stAddSize m_stAddSize;
	stSetLimit m_stSetLimit;
	stSetCeCond1Dut m_stSetCeCond1Dut;
	std::string m_sBibId;
	std::vector<CDut*> m_vecDuts;
	std::unordered_map<uint,CDut*> m_mapSiIndex;		//2023.07.09 - KWMOON
	std::unordered_map<uint,CDut*> m_mapSiIndex0;		//2023.07.09 - KWMOON
	std::unordered_map<uint,CDut*> m_mapSiIndex1;		//2023.07.09 - KWMOON
	stSetCe m_stSetCe[MAX_CE_NUM*2];

};

#endif // BIB_HPP
