#ifndef CSAVEMAP_HPP
#define CSAVEMAP_HPP

#include "od.hpp"
#include "common.hpp"
#include "util.hpp"

class CSaveMap
{
public:
	CSaveMap();
	~CSaveMap();

public:
	int parseArgument( int argc, char* argv[] );

private:
	void ShowUsage();

public:
	bool SendMap    ();
	bool GenSaveMap (uint iFileType);
	bool GenMap     (uint iFileType, T_LOT* pLot);
	bool AddSendList(                T_LOT* pLot);

private:
	string m_sHeadSet;
	string m_sHeadPre;

private:
	bool MakeHead(uint iFileType, T_LOT* pLot);
	bool MakeBody(uint iFileType, T_LOT* pLot);

	bool SaveMapCnt(char* pszPath, uint iSetCnt, uint iPreCnt);

private:
	string	m_strDir;
	string	m_strFiles;

	time_t m_tStart;
	time_t m_tEnd  ;
	string m_strHead;
	string m_strBody;

	uint m_iCnt;

public:
	int YYYY( time_t tTime );
	int MM  ( time_t tTime );
	int DD  ( time_t tTime );
	int hh  ( time_t tTime );
	int mm  ( time_t tTime );
	int ss  ( time_t tTime );

private:
	bool WriteFile(int, const char*, uint );

private:
	COrganizedData m_OD;
};

#endif // CSAVEMAP_HPP
