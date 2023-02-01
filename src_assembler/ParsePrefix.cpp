#include "ParsePrefix.h"


void assembler::ParsePrefix::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> allData(2);	// lemma - prefix
	boost::split(allData, dataLine, boost::is_any_of("\t"));
	std::string lemma = allData[0];
	std::string prefix = allData[1];

	if (lookup.find(lemma) != lookup.end()){
		std::string prefixedLemma = prefix + lemma;
		for (vecStrIter it = lookup[lemma].begin(); it != lookup[lemma].end(); ++it){
			std::string wordform = *it;
			std::string prefixedWordform = prefix + wordform;
			for (gramsToIntMap::iterator iter = database[wordform][lemma].begin(); iter != database[wordform][lemma].end(); ++iter){
				std::string tag = iter->first;
				if (tag.find(grepCondition) != tag.npos){
					++database[prefixedWordform][prefixedLemma][tag];
				}
			}
		}
	}
	else {
		std::cout << "WARNING: Non-attested lemma " << lemma << std::endl;
	}
}

void assembler::ParsePrefix::clearLookup()
{
	if (lookup.size()){
		lookup.clear();
	}
}

std::vector<std::string>& assembler::ParsePrefix::getLookup(std::string par)
{
	return lookup[par];
}

void assembler::ParsePrefix::completePOS()
{
	if (grepCondition == "Adj"){
		completeInnerPOS("adj_prefixed", "the list of prefixed adjectives");
	}
	else if (grepCondition == "N"){
		completeInnerPOS("noun_prefixed", "the list of prefixed nouns");
	}
}
