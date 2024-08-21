#ifndef CITEMCUM_HPP
#define CITEMCUM_HPP

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

class CItemCum
{
public:
	explicit CItemCum(COrganizedData*);

public:
	bool GenItemCum();

private:
	bool GetFailBlockAddress( std::vector<uint>& v, char* p, uint slot, uint dut, uint ce, uint chip,                uint step=0 );
	bool IsBlockFail        (                       char* p, uint slot, uint dut, uint ce, uint chip, uint iBlkAddr, uint step=0 );
	bool IsStepFail         (                       char* p, uint slot, uint dut, uint step );
	bool IsPreFail(uint,uint);
	bool IsChipFail     (char* p, uint slot, uint dut, uint ce, uint chip, uint step=0);
	int  GetStepFailMode( int* p, uint step );

private:
	bool CreateItemCumFile(T_LOT* pLot);
	bool MakeHeader       (T_LOT* pLot, int fd);
	bool AppendItemCum    (T_LOT* pLot, int fd);

private:
	std::string m_strDir;
	std::string m_strFile;
	std::string m_sFile;
	COrganizedData* m_pOd;

public:
	int YYYY( time_t tTime );
	int MM  ( time_t tTime );
	int DD  ( time_t tTime );
	int hh  ( time_t tTime );
	int mm  ( time_t tTime );
	int ss  ( time_t tTime );
};

#endif // CITEMCUM_HPP
