#pragma once

#include "POS.h"
#include "POS_Template.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>

namespace assembler {
	class NounAddition : public POS
	{
	public:
		NounAddition(wordsToLemsMap& db) : POS(db)
			{ completeInnerPOS("noun_addition", "secondary source file"); }
		~NounAddition(){}
	private:
		void parseExplicit(std::string& dataLine);
	};
}

void assembler::NounAddition::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> data(4);	// stem - pos - inflection - paradigm
	boost::split(data, dataLine, boost::is_any_of("\t"));
	std::vector<std::string> infl;
	const std::string stem = data[0];
	const std::string pos = data[1];
	boost::split(infl, data[2], boost::is_any_of(" "));
	std::string lemma = stem;
	lemma += infl[0];
	std::vector<std::string> par;
	boost::split(par, data[3], boost::is_any_of(" "));
	int parSize = par.size();
	if (!parSize || (infl.size() != parSize)){
		std::cout << "FATAL: The inflectional template doesn't match the sequence of tags." << std::endl;
		return;
	}
	for (int i = 0; i < parSize; ++i){
		std::string wordform = stem;
		wordform += infl[i];
		const std::string tmp = normalizeTagset(par[i]); // TODO: Rename!
		std::vector<std::string> tags;
		boost::split(tags, tmp, boost::is_any_of("|"));
		for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it){
			std::string currentTag = pos;
			currentTag.append(":").append(*it);
			++database[wordform][lemma][currentTag];
		}
	}
}
