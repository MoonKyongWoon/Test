#ifndef CIO_HPP
#define CIO_HPP

#include <iostream>
#include "io.hpp"
#include "bitchart.hpp"
#include "sysdef.hpp"

class CIO
{
public:
    CIO();
    ~CIO();
private:

public:
	bool OpenPcie(int);
	bool InitPcie();
	void ShowHelp();
	void DBMRamdomWrite(int, uint64_t, int, int fixdata=-1);
	void DBMRead       (int, uint64_t, int);
	void DFMRamdomWrite(int, uint64_t, int, int fixdata=-1);
	void DFMRead       (int, uint64_t, int);
	int parseArgument( int argc, char* argv[] );
private:
    CPcie* m_pPcieTPG0;
    CPcie* m_pPcieTPG1;
private:
	char* m_pDBmBuffer;
};

#endif // TESTLIB_HPP
