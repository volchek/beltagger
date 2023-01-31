#include "GetOptions.h"
#include "defines.h"

#include <locale>
#include <string>
#include <memory>

int main(int argc, char* argv[])
{
	setlocale(LC_ALL, LOCNAME);

	std::shared_ptr<tagger::GetOptions> go(new tagger::GetOptions);
	go->getOpt(argc, argv);

	return 0;
}
