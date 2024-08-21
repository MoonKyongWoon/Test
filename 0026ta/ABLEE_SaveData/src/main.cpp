#include <iostream>
#include "savedata.hpp"

int main( int argc, char* argv[] )
{
	CSaveData savedata;
	return savedata.parseArgument( argc, argv );
}

