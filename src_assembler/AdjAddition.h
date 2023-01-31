#pragma once

#include "POS.h"
#include "POS_Template.h"
#include <boost/algorithm/string.hpp>
#include <memory>

namespace assembler {
	class AdjAddition : public POS
	{
	public:
		AdjAddition(wordsToLemsMap& db, std::shared_ptr<POS_Template> tm) : POS(db),
			templ(tm)
			{ completeInnerPOS("adj_addition", "secondary source file"); }
		~AdjAddition(){}
	private:
		void parseExplicit(std::string& dataLine);
		std::shared_ptr<POS_Template> templ;
	};
}

void assembler::AdjAddition::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> data(3);	// stem - pos - class
	boost::split(data, dataLine, boost::is_any_of("\t"));
	const std::string stem = data[0];
	const std::string pos = data[1];
	const std::string adjClass = data[2];
	std::vector<std::string> infl = templ->getInflection(adjClass);
	std::string lemma = stem;
	lemma += infl[0];
	std::vector<std::string> par = templ->getParadigmAdjAndPart(adjClass);
	int inflSize = infl.size();
	if ( !inflSize || (inflSize != par.size()) ){
		return;
	}
	for (int i = 0; i < inflSize; ++i){
		std::string wordform = stem;
		wordform += infl[i];
		std::vector<std::string> tags;
		boost::split(tags, par[i], boost::is_any_of("|"));
		for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it){
			std::string currentTag = pos;
			currentTag.append(":").append(*it);
			++database[wordform][lemma][currentTag];
		}
	}
}
