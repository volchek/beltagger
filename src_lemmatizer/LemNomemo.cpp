#include "LemNomemo.h"

tagger::LemNomemo::LemNomemo(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar) :
	Lemmatizer(inputFile, outputFile, lastIndex, logUnknowns, unkFile, tar)
{
	startLem();
}

tagger::LemNomemo::LemNomemo(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar, bool extension) :
	Lemmatizer(inputFile, outputFile, lastIndex, logUnknowns, unkFile, tar)
{
}


tagger::LemNomemo::~LemNomemo()
{
}

/***********************
Plain output methods
***********************/

std::string tagger::LemNomemo::plainOutput(const wordInfo& data, const std::string& wordform, const std::string& linePostfix)
{
	std::string outputStr = wordform;
	return outputStr.append("\t").append(plainOutputPlugin(data, linePostfix));
}

/***********************
Processing methods
***********************/

void tagger::LemNomemo::processOne(const std::string& wordform, const std::string& linePrefix, const std::string& linePostfix, const std::string&)
{
	std::string res = linePrefix;
	res.append("\t").append(processOnePlugin(wordform, linePostfix));
	printLine(res);
}

bool tagger::LemNomemo::processWordform(std::string& inputLine, std::string& wordform)
{
	return mainLoop(inputLine, wordform);
}
