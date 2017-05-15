#include "NP_Excl.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

const std::string assembler::NP_Excl::properNameGroups[NP_EXCL_COUNT] = {"antr", "top"};

assembler::NP_Excl::NP_Excl(wordsToLemsMap& db) :
	POS(db)
{
	completePOS("", "exceptional wordforms");
}


assembler::NP_Excl::~NP_Excl()
{
}

void assembler::NP_Excl::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> data(3);	// wordform - lemma - paradigm
	boost::split(data, dataLine, boost::is_any_of("\t"));
	const std::string wordform = data[0];
	const std::string lemma = data[1];
	const std::string allTags = normalizeTagset(data[2]);
	std::vector<std::string> tags;
	boost::split(tags, allTags, boost::is_any_of("|"));
	for(std::vector<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it){
		++database[wordform][lemma][*it];
	}
}

void assembler::NP_Excl::completePOS(std::string posShort, std::string posFull)
{
	std::cout << "\tReading " << posFull << "...";
	for (size_t i = 0; i < NP_EXCL_COUNT; ++i){
		std::string fileName = "np_" + properNameGroups[i] + "_exclusions";
		readFile(fileName);
	}
	std::cout << "\tDone" << std::endl;
}