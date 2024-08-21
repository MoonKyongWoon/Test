#ifndef ATSTRING_HPP
#define ATSTRING_HPP

#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdexcept>
#include <memory>
#include <string>
#include <list>
#include <vector>
#include <algorithm>

using namespace std;

class atstring : public string
{
public:
    atstring() {}
    atstring(char* s) : std::string(s) {}
    atstring(char* s, char* e) : std::string(s, e) {}
    atstring(const char* s) : std::string(s) {}
    atstring(const char* s, int n) : std::string(s, n) {}
    atstring(const char* s, int p, int n) : std::string(s, p, n) {}
    atstring(std::string s) : std::string(s) {}
    atstring(int n1, int n2): std::string(n1, n2) {}

public:
	operator const char*() { return c_str(); }
    operator const unsigned char*() { return (unsigned char*)c_str(); }
	operator char*() { return (char*)c_str(); }
	operator unsigned char*() { return (unsigned char*)c_str(); }
	operator void*() { return (void*)c_str(); }
	operator const char* () const throw() { return c_str(); }

	char & operator[](unsigned int i) { return (*(std::string *)this)[i]; }
	//char & operator[](int i) { return (*(std::string *)this)[i]; }

    atstring MakeUpper()
	{ 
		std::transform(this->begin(), this->end(), this->begin(), ::toupper);
		return *this;
	}

    atstring MakeLower()
	{ 
		std::transform(this->begin(), this->end(), this->begin(), ::tolower);
		return *this;
	}

    template<typename ... Args>
    void Format(const std::string& format, Args ... args )
	{
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        auto buf = std::make_unique<char[]>( size );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        *this = std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
	}

	void AppendFormat(const char* format, ...)
	{
		va_list args;
		va_start(args, format);

		#ifdef _MSC_VER
			int size = _vscprintf(format, args);
			cstring r;
			r.resize(++size, 0);
			vsnprintf_s((char*)r.data(), size, _TRUNCATE, format, args);
			this->insert(this->end()-1, r.begin(), r.end());
		#else
			/*size_t size = _snprintf( nullptr, 0, format, args) + 1; 
			unique_ptr<char[]> buf( new char[ size ] ); 
			_vsnprintf( buf.get(), size, format, args);
			this->insert(this->end()-1, buf.get(), buf.get() + size - 1 ); // remove '\0'*/
			size_t size = vsnprintf( nullptr, 0, format, args) + 1; 
			unique_ptr<char[]> buf( new char[ size ] ); 
			vsnprintf( buf.get(), size, format, args);
			*this = string(buf.get(), buf.get() + size - 1 ); // remove '\0'
		#endif
		va_end(args);
	}

	int GetLength()
	{
		return this->size();
	}

    atstring Mid(int n)
	{
		return this->substr(n);
	}

    atstring Mid(int n, int c)
	{
		return this->substr(n, c);
	}

    atstring Left(int c)
	{
		return this->substr(0, c);
	}

    atstring Right(int c)
	{
		return this->substr(GetLength() - c);
	}

    atstring TrimLeft()
	{
        atstring s = *this;
		size_t startpos = s.find_first_not_of(" \n\r\t");
		return (startpos == std::string::npos) ? "" : s.substr(startpos);
	}

    atstring TrimRight()
	{
        atstring s = *this;
		size_t endpos = s.find_last_not_of(" \n\r\t");
		return (endpos == std::string::npos) ? "" : s.substr(0, endpos+1);
	}

    atstring Trim()
	{
		return TrimRight().TrimLeft();
	}

    int Split(vector<atstring>&a, atstring delim)
	{
		int i = 0;
		for (size_t start = this->find_first_not_of(delim), end = start; start != string::npos; i++) {
			end = this->find_first_of(delim, start);
			a.push_back(this->substr(start, end-start));
			start = this->find_first_not_of(delim, end);
		}
		return i;
	}

	int Split(std::list<std::string>&a, atstring delim)
	{
		int i = 0;
		for (size_t start = this->find_first_not_of(delim), end = start; start != string::npos; i++) {
			end = this->find_first_of(delim, start);
			a.push_back(this->substr(start, end-start));
			start = this->find_first_not_of(delim, end);
		}
		return i;
	}

    int Replace(atstring search, atstring replace)
	{
		int n = 0;
		for (size_t pos = 0; (pos = this->find(search.c_str(), pos)) != string::npos; pos += replace.length(), n++)
			this->replace(pos, search.length(), replace.c_str());
		return n;
	}

    bool CompareNoCase(atstring s)
	{
		size_t sz = s.size();
		if (this->size() != sz)
			return false;
		for (size_t i = 0; i < sz; i++)
			if (tolower((*this)[i]) != tolower(s[i]))
				return false;
		return true;
	}

    atstring ExpandTab(int tab_sz)
	{
		char* p = *this;
		int col = 0;
        atstring s = "";

		while (*p) 
		{
			if (*p == '\t') 
			{
				for (int tab_exp = (col % tab_sz != 0) ? tab_sz - (col % tab_sz) : tab_sz; tab_exp != 0; tab_exp--) 
				{
					col++;
					s += " ";
				}
			}
			else
			{
				s += *p;
				col++;
			}
			p++;
		}
		*this = s;
		return *this;
	}

};

#endif
