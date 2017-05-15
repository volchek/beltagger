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
	simpleTar["���"] = "���";
	simpleTar["�"] = "�";
	simpleTar["��"] = "��";
}


std::string tagger::TarShallow::tweakTar(const std::string& wordform)
{
	if(simpleTar.find(wordform) != simpleTar.end()){
		return simpleTar[wordform];
	}
	std::string cleanWordform = wordform;
	if (bx::regex_search(cleanWordform, compiler.compile("��"))){
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([������])([�����])", bx::regex_constants::icase), "�$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("�����([�����])", bx::regex_constants::icase), "���$1");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("����([�����])", bx::regex_constants::icase), "���$1");	
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([���])", bx::regex_constants::icase), "�'$1");
	}
	if (bx::regex_search(cleanWordform, compiler.compile("��"))){
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("����", bx::regex_constants::icase), "���");	// BEWARE! "��������" and the like get incorrect representations
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([�������])([�����])", bx::regex_constants::icase), "�$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("����", bx::regex_constants::icase), "�����");	// correct the mistakes
	}
	if (bx::regex_search(cleanWordform, compiler.compile("[���]�"))){
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([�])([�����])", bx::regex_constants::icase), "�$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([��])([�����])", bx::regex_constants::icase), "�$1$2");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("����([�����])", bx::regex_constants::icase), "���$1");
		cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("��([��])([�����])", bx::regex_constants::icase), "�$1$2");	// BEWARE! "����������" and the like get incorrect representations
	}
	return cleanWordform;
}
