#include "TarShallow.h"
#include "defines.h"

tagger::TarShallow::TarShallow() :
	loc(std::locale(LOCNAME))
{
	compiler.imbue(loc);
	completeSimpleTar();
}


tagger::TarShallow::TarShallow(std::locale& inputLocale) :
	loc(inputLocale)
{
	compiler.imbue(loc);
	completeSimpleTar();
}


tagger::TarShallow::~TarShallow()
{
}


void tagger::TarShallow::completeSimpleTar()
{
	simpleTar["бяз"] = "без";
	simpleTar["й"] = "і";
	simpleTar["ня"] = "не";
}


std::string tagger::TarShallow::tweakTar(const std::string& wordform)
{
	if(simpleTar.find(wordform) != simpleTar.end()){
		return simpleTar[wordform];
	}
	std::string cleanWordform = wordform;
	if (bx::regex_search(cleanWordform, compiler.compile("зь"))){
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("зь([влмнбз])([ьяеёюі])", bx::regex_constants::icase), "з$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("дзьдз([ьяеёюі])", bx::regex_constants::icase), "ддз$1");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("зьдз([ьяеёюі])", bx::regex_constants::icase), "здз$1");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("зь([яею])", bx::regex_constants::icase), "з'$1");
	}
	if (bx::regex_search(cleanWordform, compiler.compile("сь"))){
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("сься", bx::regex_constants::icase), "шся");	// BEWARE! "калосься" and the like get incorrect representations
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("сь([влмнпсц])([ьяеёюі])", bx::regex_constants::icase), "с$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("пісме", bx::regex_constants::icase), "пісьме");	// correct the mistakes
	}
	if (bx::regex_search(cleanWordform, compiler.compile("[лнц]ь"))){
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("ль([л])([ьяеёюі])", bx::regex_constants::icase), "л$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("нь([нс])([ьяеёюі])", bx::regex_constants::icase), "н$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("ньдз([ьяеёюі])", bx::regex_constants::icase), "ндз$1");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("ць([вц])([ьяеёюі])", bx::regex_constants::icase), "ц$1$2");	// BEWARE! "заахвоцьце" and the like get incorrect representations
	}
	return cleanWordform;
}
