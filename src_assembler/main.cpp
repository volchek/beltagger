#include "Composer.h"
#include "defines.h"

#include <locale>
#include <iostream>
#include <ctime>
#include <boost/shared_ptr.hpp>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, LOCNAME);
	std::clock_t start = clock();
	
	Composer comp(argc, argv);
	comp.outputDB();
	
	std::clock_t end = clock();
	std::printf("Complete in %f seconds.\n", ((float)(end - start))/CLOCKS_PER_SEC);

	return 0;
}