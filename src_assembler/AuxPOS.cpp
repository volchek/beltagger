#include "AuxPOS.h"
#include <boost/algorithm/string.hpp>


assembler::AuxPOS::AuxPOS(wordsToLemsMap& db) : POS(db)
{
}

assembler::AuxPOS::~AuxPOS()
{
}

void assembler::AuxPOS::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> allData(3);	// wordform - lemma - tag
	boost::split(allData, dataLine, boost::is_any_of("\t"));
	++database[allData[0]][allData[1]][allData[2]];
}

