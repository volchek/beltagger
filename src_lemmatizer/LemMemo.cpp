#include "LemMemo.h"

tagger::LemMemo::LemMemo(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar):
	Lemmatizer(inputFile, outputFile, lastIndex, logUnknowns, unkFile, tar)
{
	startLem();
}

tagger::LemMemo::LemMemo(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar, bool extension):
	Lemmatizer(inputFile, outputFile, lastIndex, logUnknowns, unkFile, tar)
{
}

tagger::LemMemo::~LemMemo()
{
}

/***********************
  Plain output methods
***********************/

std::string tagger::LemMemo::plainOutput(const wordInfo& data, const std::string& wordform, const std::string& linePostfix)
{
	const std::string wordformInfo = plainOutputPlugin(data, linePostfix);
	lSeen[wordform] = wordformInfo;	// memoization
	std::string result = wordform;
	return result.append("\t").append(wordformInfo);
}

/***********************
  Processing methods
***********************/

void tagger::LemMemo::processOne(const std::string& wordform, const std::string& linePrefix, const std::string& linePostfix, const std::string& originalWordform)
{
	const std::string dataLine = processOnePlugin(wordform, linePostfix);
	lSeen[originalWordform] = dataLine;	// memoization
	std::string res = linePrefix;
	res.append("\t").append(dataLine);
	printLine(res);
}

bool tagger::LemMemo::processWordform(std::string& inputLine, std::string& wordform)
{
	// (Regular tokens = R)
	// R0. Cached output
	if (processMemo(inputLine, wordform)){
		return true;
	}

	return mainLoop(inputLine, wordform);
}

bool tagger::LemMemo::processMemo(std::string& inputLine, std::string& wordform)
{
	std::unordered_map<std::string, std::string>::iterator it = lSeen.find(wordform);
	if(it != lSeen.end()){
		std::string dataLine = inputLine;
		dataLine.append("\t").append(it->second);
		printLine(dataLine);
		return true;
	}
	return false;
}