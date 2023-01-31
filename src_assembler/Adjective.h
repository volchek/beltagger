#pragma once

#include "POS.h"
#include "POS_Template.h"
#include "ParsePrefix.h"
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <memory>

namespace assembler {
	class Adjectives : public POS
	{
	public:
		Adjectives(wordsToLemsMap& db, std::shared_ptr<POS_Template> tm, std::shared_ptr<ParsePrefix> lk);
		~Adjectives();
	private:
		void parseExplicit(std::string& dataLine);
		void completePOS();
		std::shared_ptr<POS_Template> templ;
		std::shared_ptr<ParsePrefix> lookup;
	};
}

assembler::Adjectives::Adjectives(wordsToLemsMap& db, std::shared_ptr<POS_Template> tm, std::shared_ptr<ParsePrefix> lk) :
			POS(db), templ(tm), lookup(lk)
{ 
	completePOS();
	lookup->setCondition("Adj");
}

assembler::Adjectives::~Adjectives(){
	templ->clearParadigm("adj");
	templ->clearInflection();
	lookup->clearLookup();
	lookup->setCondition("");
}

void assembler::Adjectives::parseExplicit(std::string& dataLine)
{
	boost::replace_all(dataLine, "<", "");
	std::vector<std::string> wordforms;
	boost::split(wordforms, dataLine, boost::is_any_of("\t"));
	const std::string lemma = wordforms[0];
	int countWords = wordforms.size();

	std::vector<std::string> gram = templ->getParadigmAdjAndPart("1");
	for (int i = 1; i < countWords; ++i){
		std::string currentWordform = wordforms[i];
		std::vector<std::string> tags;
		boost::split(tags, gram[i-1], boost::is_any_of("|"));
		for (std::vector<std::string>::iterator it = tags.begin(); it != tags.end(); ++it){
			std::string currentTag = "Adj:";
			currentTag += *it;
			++database[currentWordform][lemma][currentTag];
			(lookup->getLookup(lemma)).push_back(currentWordform);
		}
	}
}

void assembler::Adjectives::completePOS()
{
	std::cout << "Working on adjectives and ordinals..." << std::endl;
	std::cout << "\tReading declension templates...";
	templ->parseTemplate("adj");
	std::cout << "\tDone" << std::endl;
	completeInnerPOS("adj_main", "primary source file");
}
