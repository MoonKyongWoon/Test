#include <iostream>
#include "mio.hpp"

int main( int argc, char* argv[] )
{
	CIO io;
	return io.parseArgument( argc, argv );
}
