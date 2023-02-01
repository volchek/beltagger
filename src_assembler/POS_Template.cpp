#include "POS_Template.h"
#include <iostream>
#include <boost/algorithm/string.hpp>
#include <regex>

assembler::POS_Template::POS_Template(wordsToLemsMap& db)
	: POS(db)
{
}

assembler::POS_Template::~POS_Template()
{
}

void assembler::POS_Template::parseNounTemplatePlugin(std::string& dataLine)
{
	std::vector<std::string> allData(2);
	boost::split(allData, dataLine, boost::is_any_of("\t"));
	std::vector<std::string> grams;
	boost::split(grams, allData[1], boost::is_any_of(" "));
	for (std::vector<std::string>::iterator it = grams.begin(); it != grams.end(); ++it){
		*it = unfold(*it, "N");
	}
	paradigm[allData[0]] = grams;
}

void assembler::POS_Template::parseVerbTemplatePlugin(std::string& dataLine)
{
	std::vector<std::string> allData(2);
	boost::split(allData, dataLine, boost::is_any_of("\t"));
	std::vector<std::string> grams;
	boost::split(grams, allData[1], boost::is_any_of(" "));
	paradigm[allData[0]] = grams;
}

void assembler::POS_Template::parseAdjAndPartTemplatePlugin(std::string& dataLine)
{
	std::vector<std::string> allData(3);	// class - inflection - paradigm
	boost::split(allData, dataLine, boost::is_any_of("\t"));
	std::vector<std::string> infl;
	boost::split(infl, allData[1], boost::is_any_of(" "));
	inflection[allData[0]] = infl;
	std::vector<std::string> grams;
	boost::split(grams, allData[2], boost::is_any_of(" "));
	for (std::vector<std::string>::iterator it = grams.begin(); it != grams.end(); ++it){
		*it = normalizeTagset(*it);
	}
	paradigmAdjAndPart[allData[0]] = grams;
}

void assembler::POS_Template::parseTemplate(std::string pos)
{
	if (!std::regex_search(pos, std::regex("^noun|adj|verb|part$"))){
		std::cout << "Unable to continue: Incorrect part-of-speech label supplied to the template reader.\n";
		return;
	}
	templatePOS = pos;
	std::string fileName = pos + "_template";
	readFile(fileName);
}

void assembler::POS_Template::parseExplicit(std::string& dataline){
	if (templatePOS == "noun") { parseNounTemplatePlugin(dataline); }
	else if (templatePOS == "verb") { parseVerbTemplatePlugin(dataline); }
	else { parseAdjAndPartTemplatePlugin(dataline); }
}

std::vector<std::string>& assembler::POS_Template::getParadigmAdjAndPart(std::string par)
{
	return paradigmAdjAndPart[par];
}

std::vector<std::string>& assembler::POS_Template::getParadigm(std::string par)
{
	return paradigm[par];
}


std::vector<std::string>& assembler::POS_Template::getInflection(std::string par)
{
	return inflection[par];
}

/**********************
	Clear data
***********************/

void assembler::POS_Template::clearParadigm(std::string pos)
{
	if (pos == "adj"){
		if (paradigmAdjAndPart.size()){
			paradigmAdjAndPart.clear();
		}
	}
	else {
		if (paradigm.size()){
			paradigm.clear();
		}
	}
}

void assembler::POS_Template::clearInflection()
{
	if (inflection.size()){
		inflection.clear();
	}
}
