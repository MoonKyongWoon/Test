#ifndef CKILL_HPP
#define CKILL_HPP

#include <iostream>

class CKill
{
public:
	CKill();
	~CKill();
public:
	int parseArgument( int argc, char* argv[] );
	int KillProcess(std::string);
private:
	void ShowHelp();
private:
	std::string m_sApp;
};

#endif
