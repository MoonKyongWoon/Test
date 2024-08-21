#ifndef CLOG_HPP
#define CLOG_HPP

#include <map>
#include <vector>
#include "atstring.hpp"
#include "devsocket.hpp"
#include "sys.hpp"
#include "syslog.hpp"

class CLog
{
public:
	CLog();
private:
	void Init();
	std::string GetDate();
public:	
	void ClearError();
	void Printf  ( DEV_LOG_TYPE, const char* sfunc, const char*, ... );		//USER
	void EPrintf ( DEV_LOG_TYPE, const char* sfunc, const char*, ... );		//SYSTEM
	void Printf  ( const char*, ... );										//USER
	void Print   ( const char*, ... );										//USER
	int GetErrorCount();
	const char* GetError(int);
//2023.03.18 - kwmoon
public:
	void RuntimeError( DEV_LOG_TYPE, const char* sfunc, const char*, ... );			//USER
	void RuntimeError( const char*, ... );			//RUNTIME ERROR
private:
	std::map< DEV_LOG_TYPE, atstring> m_mapString;
	std::vector<atstring> m_vecLog;
};

#endif // CERROR_HPP
