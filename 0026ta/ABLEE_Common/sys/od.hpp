#ifndef CORGANIZEDDATA_HPP
#define CORGANIZEDDATA_HPP

#include <QObject>
#include <list>
#include "filemaplib.hpp"

class T_SLOT
{
private:
	bool m_bValid;


public:
	char BIB_ID[64];
	int  BIB_X;
	int  BIB_Y;
	char MLB_CODE[32];
	char IOC     [32];
	char BOC     [32];

	bool *sdut;
	uint nInQty;
	uint nSortBinCnt [17];
	uint nSortBinHCnt[17];

public:
	bool *ldut;	//Loaing DUT
	bool *mdut;	//Masked DUT

public:
	T_SLOT();
	~T_SLOT();
	void Init();

	void SET_VALID(bool bValid);
	bool IS_VALID();

};


class T_LOT
{
public:
	char LOT_NO     [64];
	char PART_NO    [32];
	char TEST_PGM_NO[64];
	char DEVICE_NO  [64];
	char OPER       [32];
	char FAB        [32];
	char GRADE      [32];
	char OWNER      [32];
	char FACTORY    [32];
	int  RWK_COUNT;
	char RWK_CODE   [32];
	char MPG_FLAG       ;
	char OP_ID      [32];

	int  SNO;

	time_t START_TIME;
	time_t END_TIME  ;


	T_SLOT*	RS;		//Reference Slot
	T_SLOT S[32];	//32Slot <- 16Slot Available

	char m_szReportDir[PATH_MAX];

//private:
	uint  m_uRS;				//Reference Slot No: 0 ~ 15
	uint  m_uRC;				//Reference CPU  No: 0 ~  1

public:
	time_t m_tInitStart; //for SaveMap
	time_t m_tInitEnd  ; //for SaveMap

public:
	T_LOT();
	T_LOT(string sLotNo);
	void Init();

};

typedef std::list<T_LOT*> LOT_LIST;

class CFtpConf
{
public:
	char IP  [PATH_MAX];
	char USER[PATH_MAX];
	char PASS[PATH_MAX];
	char DIR [PATH_MAX];

	CFtpConf();
	void Init();
	bool Get(const char* pszSvr);
};

class CFileMap;
class COrganizedData : public QObject
{
public:
	COrganizedData();
	~COrganizedData();

private:
	bool      m_bDebugMode;
	QString   m_sDataRoot;

public:
	CFileMap* m_pDataMgr;
	char      m_szReportRoot[PATH_MAX];

public:
	LOT_LIST m_Lot;

private:
	void ClrLotList();
	void GetDataRootPath();
	bool GetReportRootPath();
	bool GetRefSlotCpu();
	bool GetDevPgmVar();
	bool GetMaxStep();

public:
	void SET_DEBUGMODE( bool bDebugMode );
	void SET_ERRMSG( const char* str, ...);
	bool ParseRCmd          (bool  bSavemap=false);
	bool ParseEventIni      (bool* pbValidBib);
	bool ParseStartBiCmd    (bool* pbValidBib);
	bool ParseBoardMaintInfo(bool* pbValidBib, bool bSavemap=false);
	bool ParseTgetBiOpenLot (bool  bSavemap=false);
	bool ParseVidIni        (bool  bSavemap=false);
	void DispLOT();

	bool MkReportDir        (bool  bSavemap=false);
	bool GatherFoundationData();
	bool GatherInitFailData  (uint iFileType); //for SaveMap only, iFileType==0: set map, iFileType==1: pre map

	bool ReadSetupTime(stSetupTimePair &tSetup);
public:
//	bool IsStepFail(int iSlot, uint iDut, uint iStep)

public:
///////////////////////////////////////////////////////////////////////
//Foundation Data
	stInitTestHead			m_initTestHead [MAX_SLOT][MAX_SYS_CPU];

	uint*					m_pSortBin     [MAX_SLOT];
	char*					m_pInitFail    [MAX_SLOT];
	char*					m_pStepFail    [MAX_SLOT];

	stStepInInfo*			m_pStepInInfo  [MAX_SLOT][MAX_SYS_CPU];
	stStepOutInfo*			m_pStepOutInfo [MAX_SLOT][MAX_SYS_CPU];
	std::vector<CStepData*>	m_vecStepData  [MAX_SLOT][MAX_SYS_CPU];
	uint                    m_nSortBinCnt  [MAX_SLOT][17];
	uint                    m_nSortBinHCnt [MAX_SLOT][17];
	uint                    m_Binmap       [MAX_SLOT][16];

	stChipId*               m_pstChipId     [MAX_SLOT];
	char*                   m_pCumFailChip  [MAX_SLOT];
	char*                   m_pStepFailChip [MAX_SLOT];
	char*                   m_pCumFailBlock [MAX_SLOT];
	char*                   m_pStepFailBlock[MAX_SLOT];

	uint*                   m_pStepFCate    [MAX_SLOT];
	uint*                   m_pStepPCate    [MAX_SLOT];
	int*                    m_pStepFailMode [MAX_SLOT];
	int*                    m_pStepUseMode  [MAX_SLOT];

/////////////////////////////////////////////////////////////////
	stDevPgmVariables		m_stDevPgmVar [MAX_SLOT][MAX_SYS_CPU];

	char m_szVersion     [MAX_SLOT][MAX_TEXT_VALUE];
	char m_szDevice      [MAX_SLOT][MAX_TEXT_VALUE];
	uint m_nMaxCpu       [MAX_SLOT];
	uint m_nMaxDut       [MAX_SLOT];
	uint m_nMaxDutPerCpu [MAX_SLOT];
	uint m_nBibX         [MAX_SLOT];
	uint m_nBibY         [MAX_SLOT];
	uint m_nMaxStep      [MAX_SLOT];

	uint m_nCePerDut     [MAX_SLOT];
	uint m_nChipPerCe    [MAX_SLOT];
	uint m_nBlockPerCe   [MAX_SLOT];
	uint m_nBlockPerChip [MAX_SLOT];
	uint m_nChipPerDut   [MAX_SLOT];
	uint m_nChipPerDutBl [MAX_SLOT];
	uint m_nBlockPerDutBl[MAX_SLOT];

	bool m_bValidSlot    [MAX_SLOT];
///////////////////////////////////////////////////////////////////////
};

#endif // CORGANIZEDDATA_HPP
