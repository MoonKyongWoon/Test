#ifndef CCHDR_HPP
#define CCHDR_HPP

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

class CChdr
{
public:
	explicit CChdr(COrganizedData*);

public:
	bool GenChdr();

private:
	uint  GetPara(T_LOT* pLot);
	const char* LotProc();

	bool IsPreFail(uint,uint);
	bool IsChipFail     (char* p, uint slot, uint dut, uint ce, uint chip, uint step=0);
	int  GetFailBlockCnt(char* p, uint slot, uint dut, uint ce, uint chip, uint step=0);
	bool GetFCate       (uint* p, uint slot, uint dut, std::vector<uint>& vFCate, bool bWithStep=true);

private:
	bool CreateChdrFile(T_LOT* pLot, uint iChip, uint nTotChip);
	bool MakeHeader    (T_LOT* pLot, uint iChip, uint nTotChip, int fd);
	bool AppendChdr    (T_LOT* pLot, uint iChip, uint nTotChip, int fd);

private:
	vector<string>  m_veCChipIdFiles;
	std::string     m_strDir;
	std::string     m_strFiles;
	std::string     m_sFile;
	COrganizedData* m_pOd;

public:
	int YYYY( time_t tTime );
	int MM  ( time_t tTime );
	int DD  ( time_t tTime );
	int hh  ( time_t tTime );
	int mm  ( time_t tTime );
	int ss  ( time_t tTime );
};

#endif // CCHDR_HPP
