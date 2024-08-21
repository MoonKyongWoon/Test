#include <iostream>
#include "savemap.hpp"

int main( int argc, char* argv[] )
{
	CSaveMap savemap;
	return savemap.parseArgument( argc, argv );
}

