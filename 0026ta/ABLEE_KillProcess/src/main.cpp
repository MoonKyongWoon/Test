#include <iostream>
#include "kill.hpp"

int main( int argc, char* argv[] )
{
	CKill kill;
	return kill.parseArgument( argc, argv );
}
