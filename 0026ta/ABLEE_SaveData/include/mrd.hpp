#ifndef CMRD_HPP
#define CMRD_HPP

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

class CMrd
{
public:
	explicit CMrd(COrganizedData*);

public:
	bool GenMrd();

private:
	bool GetFailBlockAddress( std::vector<uint>& v, char* p, uint slot, uint dut, uint ce, uint chip, uint step=0 );
	bool IsPreFail(uint,uint);

	bool           IsValidChipId(uint slot, uint x, uint y, uint iCe, uint iChip);
	char*          GetChipId    (uint slot, uint x, uint y, uint iCe, uint iChip);
	unsigned short GetWaferId   (uint slot, uint x, uint y, uint iCe, uint iChip);
	unsigned short GetPosX      (uint slot, uint x, uint y, uint iCe, uint iChip);
	unsigned short GetPosY      (uint slot, uint x, uint y, uint iCe, uint iChip);

	char*          GetFailBlkStepName(uint slot, uint dut, uint iBlock);

private:
	int  CreateMrdFile(T_LOT* pLot, uint slot, uint x, uint y, uint iCe, uint iChip);
	bool MakeHeader   (T_LOT* pLot, uint slot, uint x, uint y, uint iCe, uint iChip, int fd);
	bool AppendMrd    (             uint slot, uint x, uint y, uint iCe, uint iChip, int fd);

private:
	vector<string>  m_vecMrdFiles;
	string          m_strDir;
	string          m_strFiles;
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

#endif // CMRD_HPP
