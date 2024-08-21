#ifndef CFILESAVELIB_HPP
#define CFILESAVELIB_HPP

#include <iostream>
#include <errno.h>
#include <filesystem>
#include <stdio.h>
#include <unistd.h>

#include "atstring.hpp"
#include "filemaplib.hpp"
#include "syslog.hpp"
#include "swcontrol.h"
#include "sysini.hpp"
#include "util.hpp"

class CFileSaveLib
{
public:
	CFileSaveLib();
};

#endif // CFILESAVELIB_HPP
