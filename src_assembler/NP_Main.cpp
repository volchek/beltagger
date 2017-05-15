#include "NP_Main.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

const std::string assembler::NP_Main::properNameGroups[NP_MAIN_COUNT] = {"top", "org", "antr"};

assembler::NP_Main::NP_Main(wordsToLemsMap& db) :
	POS(db)
{
	completePOS("", "");
}

assembler::NP_Main::~NP_Main()
{
}

void assembler::NP_Main::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> data(4);	// stem - pos - inflections - paradigm
	boost::split(data, dataLine, boost::is_any_of("\t"));
	const std::string stem = data[0];
	const std::string pos = data[1];
	std::vector<std::string> infl;
	boost::split(infl, data[2], boost::is_any_of(" "));
	const std::string lemma = stem + infl[0];
	std::vector<std::string> paradigm;
	boost::split(paradigm, data[3], boost::is_any_of(" "));
	const int parSize = paradigm.size();
	if (parSize && (parSize == infl.size())){
		for (int i = 0; i < parSize; ++i){
			std::string wordform = stem;
			wordform += infl[i];
			std::vector<std::string> tags;
			std::string tmp = normalizeTagset(paradigm[i]); // TODO: Rename!
			boost::split(tags, tmp, boost::is_any_of("|"));
			for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it){
				std::string tag = pos;
				tag.append(":").append(*it);
				++database[wordform][lemma][tag];
			}
		}
	}
}

void assembler::NP_Main::completePOS(std::string posShort, std::string posFull)
{
	std::cout << "\tReading primary source files...";
	for (size_t i = 0; i < NP_MAIN_COUNT; ++i){
		std::string fileName = "np_" + properNameGroups[i] + "_main";
		readFile(fileName);
	}
	std::cout << "\tDone" << std::endl;
}
