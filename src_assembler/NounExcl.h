#pragma once

#include "AuxPOS.h"
#include <boost/algorithm/string.hpp>

namespace assembler {
	class NounExcl :	public POS
	{
	public:
		NounExcl(wordsToLemsMap& db) : POS(db)
			{ completeInnerPOS("noun_exclusions", "exceptional wordforms"); }
		virtual ~NounExcl(){}
	private:
		void parseExplicit(std::string& dataline);
	};
}

void assembler::NounExcl::parseExplicit(std::string& dataLine)
{
	std::vector<std::string> data(3);	// wordform - lemma - paradigm
	boost::split(data, dataLine, boost::is_any_of("\t"));
	std::vector<std::string> tags;	// wordform - lemma - paradigm
	const std::string allTags = normalizeTagset(data[2]); // TODO: Check name validity
	boost::split(tags, allTags, boost::is_any_of("|"));
	for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it){
		++database[data[0]][data[1]][*it];
	}
}
