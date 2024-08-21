#ifndef CFLS_HPP
#define CFLS_HPP

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

class CFls
{
public:
//	explicit CFls(CFileMap*);
	explicit CFls(COrganizedData*);
public:
//	bool GenFls(const char* szReportPath);
	bool GenFls();
private:
	bool GenFileName (uint, T_LOT*);
	bool MakeSortingHead(uint, T_LOT*);
	bool MakeHistoryHead(uint, T_LOT*);

//	bool GenFlsFileName(uint,int);
//	bool MakeSortingHead(uint,uint,uint);
//	bool MakeHistoryHead(uint,uint,uint);
//	bool MakePreFlsData(uint,uint);
private:
//	bool MakeSortingDataSummary  (uint,uint,bool&);
//	bool MakeSortingDataBoardMap (uint,uint,bool&);
//	bool MakeTestHistoryReport   (uint,uint,uint,bool&);
//	bool MakeTestBlockCountReport(uint,uint,bool&);
//	bool MakeFailSocketReport    (uint,uint,uint,bool&);
	bool MakeSortingDataSummary  (uint, T_LOT*, bool&);
	bool MakeSortingDataBoardMap (uint, T_LOT*, bool&);
	bool MakeTestHistoryReport   (uint, T_LOT*, bool&);
	bool MakeTestBlockCountReport(uint, T_LOT*, bool&);
	bool MakeFailSocketReport    (uint, T_LOT*, bool&);
private:
	uint StepFailCount(int,uint);
	uint StepFailCount(T_LOT*, int, uint);
	bool IsStepFail(int,uint,uint);
	bool IsAllStepPass(int,uint,uint);
private:
	std::string GetSortHead();
	std::string GetHistoryHead();
//	std::string GetHostname();
//	std::string GetChambername();

private:
//	int m_FlsFd;
//	CFileMap* m_pDataMgr;
//	std::string m_FlsFile;
//	char m_szReportPath[PATH_MAX];
	int m_fd;
	COrganizedData* m_pOd;
	std::string     m_strDir;
	std::string     m_strFiles;
	std::string     m_sFile;
private:
	std::string m_sHeadSorting;
	std::string m_sHeadHistory;

	std::string m_sTestHistoryReport;
	std::string m_sTestBlockCountReport;
	std::string m_sFailSocketReport;

//	uint* m_pSortBin [MAX_SLOT];
//	char* m_pInitFail[MAX_SLOT];
//	char* m_pStepFail[MAX_SLOT];
//	stInitTestHead m_initTestHead[MAX_SLOT][MAX_SYS_CPU];
//	stStepInInfo*  m_pStepInInfo [MAX_SLOT][MAX_SYS_CPU];
//	stStepOutInfo* m_pStepOutInfo[MAX_SLOT][MAX_SYS_CPU];
//	std::vector<CStepData*> m_vecStepData[MAX_SLOT][MAX_SYS_CPU];
//	uint m_nSortBinCnt [MAX_SLOT][17];
//	uint m_nSortBinHCnt[MAX_SLOT][17];
//	uint m_Binmap      [MAX_SLOT][16];
//
//private:
//	bool m_bCpu2X;
//	uint m_nMaxCpu;
//	uint m_nMaxDut;
//	uint m_nMaxDutPerCpu;
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

#endif // CFLS_HPP
