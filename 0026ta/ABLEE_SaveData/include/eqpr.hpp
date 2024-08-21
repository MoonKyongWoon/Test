#ifndef CEQPR_HPP
#define CEQPR_HPP

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

class CEqpr
{
public:
	explicit CEqpr(COrganizedData*);

private:
	uint  GetPara(T_LOT* pLot);
	const char* LotProc();

	bool IsPreFail(uint,uint);

private:
	bool CreateEqprFile(T_LOT* pLot);
	bool MakeHeader    (T_LOT* pLot, int fd);
	bool AppendEqpr    (T_LOT* pLot, int fd);

public:
	bool GenEqpr();

private:
	string          m_strDir;
	string          m_strFile;
	string          m_sFile;
	COrganizedData* m_pOd;

public:
	int YYYY( time_t tTime );
	int MM  ( time_t tTime );
	int DD  ( time_t tTime );
	int hh  ( time_t tTime );
	int mm  ( time_t tTime );
	int ss  ( time_t tTime );
};

#endif // CEQPR_HPP
