#include "NumAndPron.h"
#include <boost/algorithm/string.hpp>
#include <vector>


assembler::NumAndPron::NumAndPron(wordsToLemsMap& db) :
	POS(db)
{
}

assembler::NumAndPron::~NumAndPron()
{
}

void assembler::NumAndPron::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> allData(4);	// lemma - pos - wordforms - tags
	boost::split(allData, dataLine, boost::is_any_of("\t"));
	std::string lemma = allData[0];
	std::string pos = allData[1];

	std::vector<std::string> wordforms;
	boost::split(wordforms, allData[2], boost::is_any_of(" "));

	std::vector<std::string> tags;
	boost::split(tags, allData[3], boost::is_any_of(" "));
	for (vecStrIter it = tags.begin(); it != tags.end(); ++it){
		*it = unfold(*it, pos);
	}

	const size_t vecSize = wordforms.size();
	if (tags.size() != vecSize) { return; }
	for (size_t i = 0; i < vecSize; ++i){
		std::string currentWord = wordforms[i];
		std::vector<std::string> currentTags;
		boost::split(currentTags, tags[i], boost::is_any_of("|"));
		for (vecStrIter inIter = currentTags.begin(); inIter != currentTags.end(); ++inIter){
			++database[currentWord][lemma][*inIter];
		}
	}
}
