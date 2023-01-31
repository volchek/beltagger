#pragma once

#include "POS.h"
#include "POS_Template.h"
#include "ParsePrefix.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <memory>

namespace assembler {
	class Noun : public POS
	{
	public:
		Noun(wordsToLemsMap& db, std::shared_ptr<POS_Template> tm, std::shared_ptr<ParsePrefix> lk);
		virtual ~Noun();
		void completePOS();
	private:
		void parseExplicit(std::string& dataLine);
		std::shared_ptr<POS_Template> templ;
		std::shared_ptr<ParsePrefix> lookup;
	};
}

assembler::Noun::Noun(wordsToLemsMap& db, std::shared_ptr<POS_Template> tm, std::shared_ptr<ParsePrefix> lk) :
			POS(db), templ(tm), lookup(lk)
{ 
	completePOS();
	lookup->setCondition("N");
}

assembler::Noun::~Noun(){
	templ->clearParadigm("noun");
	templ->clearInflection();
	lookup->clearLookup();
	lookup->setCondition("");
}

void assembler::Noun::parseExplicit(std::string& dataLine)
{
	boost::replace_all(dataLine, "<", "");
	std::vector<std::string> data;
	boost::split(data, dataLine, boost::is_any_of("\t"));
	const std::string len = data[0];
	const std::string subclass = data[1];
	const std::string gramClass = len + subclass;
	std::vector<std::string> gram = templ->getParadigm(gramClass);
	if (!gram.size()) { return; }
	const std::string features = data[2];	// the feature block is temporarily unused
	const std::string lemma = data[3];

	const int dataSize = data.size();
	for (int i = 3; i < dataSize; ++i){
		const std::string currentWordform = data[i];
		std::vector<std::string> currentTags;
		boost::split(currentTags, gram[i-3], boost::is_any_of("|"));
		for (std::vector<std::string>::iterator it = currentTags.begin(); it != currentTags.end(); ++it){
			const std::string currentTag = *it;
			++database[currentWordform][lemma][currentTag];
			(lookup->getLookup(lemma)).push_back(currentWordform);
		}
	}
}

void assembler::Noun::completePOS()
{
	std::cout << "Working on common nouns..." << std::endl;
	std::cout << "\tReading declension templates...";
	templ->parseTemplate("noun");
	std::cout << "\tDone" << std::endl;
	completeInnerPOS("noun_main", "primary source file");
}
