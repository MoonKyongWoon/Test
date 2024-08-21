#ifndef CCHIPID_HPP
#define CCHIPID_HPP

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

class CChipId
{
public:
	explicit CChipId(COrganizedData*);

public:
	bool GenChipId();

private:
	bool IsPreFail(uint,uint);
	bool IsChipFail     (char* p, uint slot, uint dut, uint ce, uint chip, uint step=0);
	int  GetFailBlockCnt(char* p, uint slot, uint dut, uint ce, uint chip, uint step=0);

	bool GetFCate       (uint* p, uint slot, uint dut, std::vector<uint>& vFCate, bool bWithStep=true);
	bool GetChipFailStep(char* p, uint slot, uint dut, uint ce, uint chip, std::vector<uint>& vStep );

private:
	bool CreateChipIdFile(uint iFileType, T_LOT* pLot, uint iChip, uint nTotChip);
	bool MakeHeader      (uint iFileType, T_LOT* pLot, uint iChip, uint nTotChip, int fd);
	bool AppendChipId    (uint iFileType, T_LOT* pLot, uint iChip, uint nTotChip, int fd);

private:
	vector<string>  m_veCChipIdFiles;
	string          m_strDir;
	string          m_strFiles;
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

#endif // CCHIPID_HPP
