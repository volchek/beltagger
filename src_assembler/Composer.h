#pragma once

#include "DBTypedefs.h"
#include "ParsePrefix.h"
#include "POS_Template.h"
#include <string>
#include <fstream>
#include <boost/shared_ptr.hpp>

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
	boost::shared_ptr<assembler::POS_Template> objTemplate;
	boost::shared_ptr<assembler::ParsePrefix> objParsePrefix;

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

