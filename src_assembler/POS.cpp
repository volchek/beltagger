#include "POS.h"
#include <fstream>
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <stdio.h>
#include <stdlib.h>

// Initializing class-wide constants

const std::string assembler::POS::defaultPathPrefix = "./src/src_";
const std::string assembler::POS::pathPostfix = ".txt";
std::string assembler::POS::pathPrefix = assembler::POS::defaultPathPrefix;

const std::regex assembler::POS::gluedCasesTemplate = std::regex("N[GA]|G[ADL]|D[AL]|IL");
const std::vector<std::pair<std::regex, std::string>> assembler::POS::normalizationPairs = {
    std::make_pair(std::regex("^(.+)NGDAIL$"), "$1N|$1G|$1D|$1A|$1I|$1L"),
    std::make_pair(std::regex("^(.+)GDIL$"), "$1G|$1D|$1I|$1L"),
    std::make_pair(std::regex("^(.+)([NGDAIL])([NGDAIL])([NGDAIL])$"), "$1$2|$1$3|$1$4"),
    std::make_pair(std::regex("^(.+)([NGDAIL])([NGDAIL])$"), "$1$2|$1$3")
};

assembler::POS::POS(wordsToLemsMap& db) :
	database(db)
{
}

assembler::POS::~POS()
{
}

void assembler::POS::setPathPrefix(std::string newPath){
	if (!newPath.empty()){
		if (newPath[newPath.length() - 1] == '/' || newPath[newPath.length() - 1] == '\\'){
			pathPrefix = newPath + "src_";
		}
		else {
			pathPrefix = newPath + "/src_";
		}
	}
	else {
		pathPrefix = defaultPathPrefix;
	}
}

void assembler::POS::readFile(std::string fileName)
{
	std::string filePath = "";
	filePath.append(pathPrefix).append(fileName).append(pathPostfix);
	std::ifstream filein;
	filein.open(filePath);
	if (!filein.is_open()){
		std::cerr << "Can't find " << filePath;
		return;
	}
	
	std::string inputString;
	while(std::getline(filein, inputString)){
		if (!inputString.empty()){
			boost::trim(inputString);
			if(inputString.length() > 0 && inputString[0] != '#'){
				parseExplicit(inputString);
			}
		}
	}
	filein.close();
}


/**********************
   Tag normalization
***********************/

std::string assembler::POS::normalizeTagset(std::string& inputStr)
{
	if (std::regex_search(inputStr, gluedCasesTemplate))
	{
		for (auto & pair : normalizationPairs) {
			inputStr = std::regex_replace(inputStr, pair.first, pair.second);
		}
	}
	return inputStr;
}

std::string assembler::POS::unfold(std::string& tag, const std::string& posLabel)
{
	std::vector<std::string> allTags;
	const std::string tmp = normalizeTagset(tag); // TODO: Rename!
	boost::split(allTags, tmp, boost::is_any_of("|"));
	std::string resultTag = "";
	for(vecStrIter it = allTags.begin(); it != allTags.end(); ++it)
	{
		std::string oneAugmentedTag = "";
		oneAugmentedTag.append(posLabel).append(":").append(*it).append("|");
		resultTag.append(oneAugmentedTag);
	}
	resultTag.pop_back();
	return resultTag;
}

void assembler::POS::completePOS(std::string posShort, std::string posFull)
{
	std::cout << "Working on " << posFull << "..." << std::endl;
	std::cout << "\tReading the source file...";
	readFile(posShort);
	std::cout << "\tDone" << std::endl;
}

void assembler::POS::completeInnerPOS(std::string posShort, std::string fileName)
{
	std::cout << "\tReading " << fileName << "...";
	readFile(posShort);
	std::cout << "\tDone" << std::endl;
}
