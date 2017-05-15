#include "GetOptions.h"
#include "defines.h"

#include <locale>
#include <string>
#include <boost/shared_ptr.hpp>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, LOCNAME);

	boost::shared_ptr<tagger::GetOptions> go(new tagger::GetOptions);
	go->getOpt(argc, argv);

	return 0;
}