#ifndef CCUMFAILBLOCK_HPP
#define CCUMFAILBLOCK_HPP

#include <iostream>
#include <errno.h>
#include <filesystem>
#include <stdio.h>
#include <unistd.h>
#include <vector>
#include "atstring.hpp"
#include "common.hpp"
#include "filesavelib.hpp"
#include "filemaplib.hpp"
#include "syslog.hpp"
#include "swcontrol.h"
#include "util.hpp"
#include "od.hpp"

class CCumFailBlock
{
public:
//	explicit CCumFailBlock(CFileMap*);
	explicit CCumFailBlock(COrganizedData*);
public:
	bool GenCumFailBlock();
private:
	bool GenFileName(T_LOT*);
//////////////////////////////////////

//	bool GenCumFileName(uint);
//	bool MakePreCumData(uint);
private:
	bool MakeHeader     (T_LOT*, bool&);
	bool MakeChipCumData(T_LOT*, bool&);
	bool MakeStepData   (T_LOT*, bool&);
//	bool MakeHeader  (uint, uint, bool&);
//	bool MakeChipCumData( uint, uint, bool& );
//	bool MakeStepData(uint,bool&);

public:
//	bool GenCumFailBlock(const char* szReportPath);
private:
	bool GetFailBlockAddress( std::vector<uint>& v, char* p, uint s, uint dut, uint ce, uint chip, uint step=0 );
	bool IsChipFail(char* p, uint s, uint d, uint step=0);
	bool IsChipFail(char* p, uint s, uint d, uint ce, uint chip, uint step=0);
	bool IsPreFail(uint, uint);
	void mergeFailBlockInfo(std::vector<uint> &vCumBlkAddr, std::vector<uint> &vBlkAddr);
	bool IsIn(std::vector<uint> &vCumBlkAddr, uint iBlkAddr);
//	bool GetFailBlockAddress( std::vector<uint>& v, char* p, uint dut, uint ce, uint chip, uint step=0 );
//	bool IsChipFail(char* p, uint d, uint step=0);
//	bool IsChipFail(char* p, uint d, uint ce, uint chip, uint step=0);
private:
	int m_fd;
	std::string m_strDir;
	std::string m_strFile;
	std::string m_sFile;
	COrganizedData* m_pOd;

//	int m_CumFd;
//	std::string m_CumFile;
//	CFileMap* m_pDataMgr;
//	char m_szReportPath[PATH_MAX];

//	std::vector<CStepData*> m_vecStepData[MAX_SLOT][MAX_SYS_CPU];
//	stInitTestHead m_initTestHead[MAX_SLOT][MAX_SYS_CPU];
//	stStepInInfo*  m_pStepInInfo [MAX_SLOT][MAX_SYS_CPU];
//	stStepOutInfo* m_pStepOutInfo[MAX_SLOT][MAX_SYS_CPU];
//	stChipId* m_pstChipId [MAX_SLOT];
//	char* m_pInitFail     [MAX_SLOT];
//	char* m_pCumFailChip  [MAX_SLOT];
//	char* m_pStepFailChip [MAX_SLOT];
//	char* m_pCumFailBlock [MAX_SLOT];
//	char* m_pStepFailBlock[MAX_SLOT];
////	std::string m_sCumHead;
//private:
//	bool m_bCpu2X;
//	uint m_nMaxCpu;
//	uint m_nMaxDut;
//	uint m_nMaxDutPerCpu;
//	uint m_nMaxChipPerDut;
//	uint m_nCePerDut;
//	uint m_nChipPerCe;
//	uint m_nChipPerDut;
//	uint m_nChipPerDutBl;
//	uint m_nBlockPerDut;
//	uint m_nBlockPerDutBl;
//	uint m_nBlockPerCe;
//	uint m_nBlockPerChip;
//	uint m_nBibX;
//	uint m_nBibY;
//	uint m_nMaxStep;

public:
	int YYYY( time_t tTime );
	int MM  ( time_t tTime );
	int DD  ( time_t tTime );
	int hh  ( time_t tTime );
	int mm  ( time_t tTime );
	int ss  ( time_t tTime );
};

#endif // CCUMFAILBLOCK_HPP
