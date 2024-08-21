#ifndef CIPCHANGE_HPP
#define CIPCHANGE_HPP

#include <iostream>
#include <fstream>
#include "io.hpp"

class CIpchange
{
public:
    CIpchange();
    ~CIpchange();
public:
	int Run();
	int Run(int id);
	void Test();
private:
	bool OpenPcie();
    uint GetSlotId();
    bool ChangeIp(std::string);
	void SaveIp(void);
	void SaveText(std::string, std::string);
	bool Ipchange(int);
	bool LinkUp(int);
private:
	CPcie* m_pcie;
};

#endif //IPCHANGE_HPP
