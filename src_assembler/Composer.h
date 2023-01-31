#pragma once

#include "DBTypedefs.h"
#include "ParsePrefix.h"
#include "POS_Template.h"
#include <string>
#include <fstream>
#include <memory>

class Composer
{
public:
	Composer(int avgc, char** argv);
	~Composer();

	void completeDB();
	void outputDB() const;

private:
	std::string outputFormat;
	assembler::wordsToLemsMap database;
	std::map<std::string, int> debugCountWords;	
	std::shared_ptr<assembler::POS_Template> objTemplate;
	std::shared_ptr<assembler::ParsePrefix> objParsePrefix;

	std::string inputPath;
	std::string outputFile;

	void completeProperNouns();
	void completeAdjectives();
	void completeCommonNouns();

	void getOutputOption(int, char**);
	void outputDetailed(std::ofstream&) const;
	void outputSimple(std::ofstream&) const;
	void outputDummy(std::ofstream&) const;
};

