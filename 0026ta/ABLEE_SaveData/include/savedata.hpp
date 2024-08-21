#ifndef CDATASAVE_HPP
#define CDATASAVE_HPP

#include "od.hpp"
#include "fls.hpp"
#include "cumfailblock.hpp"
#include "mrd.hpp"
#include "chipid.hpp"
#include "eqpr.hpp"
#include "chdr.hpp"
#include "itemcum.hpp"
#include "sysini.hpp"
#include "util.hpp"

class CSaveData
{
public:
	CSaveData();
	~CSaveData();
public:
	int parseArgument( int argc, char* argv[] );
	void ShowHelp();
public:
	bool SaveData();
	bool GenFls();
	bool GenCumFailBlock();
	bool GenMrd();
	bool GenChipId();
	bool GenEqpr();
	bool GenChdr();
	bool GenItemCum();
private:
	bool LoadUiAutoMode();	//2024.06.13 - kwmoon
	bool InitDataDir();
	bool StbyReportDir();
	bool ClearDirectory();
	bool WriteFile(int, const char*, uint );
private:
	COrganizedData m_OD;
	CFls         * m_pFls;
	CCumFailBlock* m_pCumFailBlock;
	CMrd         * m_pMrd;
	CChipId      * m_pChipId;
	CEqpr	     * m_pEqpr;
	CChdr        * m_pChdr;
	CItemCum     * m_pItemCum;
private:
	char m_szDataRoot[PATH_MAX];
	char m_szDataDir[PATH_MAX];
	char m_szReportDir[PATH_MAX];
	char m_szLotNo[MAX_TEXT_VALUE];
	char m_szPartNo[MAX_TEXT_VALUE];
	char m_szOprId[MAX_TEXT_VALUE];
	int  m_iSublot;
	bool m_bUiMode;
	CFileMap* m_pDataMgr;
	int m_nSlot;
	int m_nCpu;
};

#endif // CDATASAVE_HPP
