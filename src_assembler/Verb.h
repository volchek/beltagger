#pragma once

#include "POS.h"
#include "POS_Template.h"
#include <boost/algorithm/string.hpp>
#include <boost/regex.hpp>
#include <boost/shared_ptr.hpp>
#include <algorithm>

namespace assembler {
	class Verb : public POS
	{
	public:
		Verb(wordsToLemsMap& db, boost::shared_ptr<POS_Template> tm);
		~Verb();
	private:
		std::string reshapeVerbTag(const std::string& tag, const std::string& featureBlock);
		void parseExplicit(std::string& dataLine);
		void completePOS();
		boost::shared_ptr<POS_Template> templ;
	};
}

assembler::Verb::Verb(wordsToLemsMap& db, boost::shared_ptr<POS_Template> tm) : 
			POS(db), templ(tm)
{ 
	completePOS();
}

assembler::Verb::~Verb(){
	templ->clearParadigm("verb");
	templ->clearParadigm("abj");
	templ->clearInflection();
}

std::string assembler::Verb::reshapeVerbTag(const std::string& tag, const std::string& featureBlock)
{
	std::string resultTag = "V:";
	boost::smatch res;
	if (boost::regex_search(tag, res, boost::regex("^Part:(act|pass)$"))){
		resultTag.append("Part:").append(featureBlock).append(":").append(res[1]);
	}
	else if(tag == "Adv"){
		resultTag.append("Adv:").append(featureBlock);
	}
	else { resultTag.append(featureBlock).append(":").append(tag); }
	return resultTag;
}

void assembler::Verb::parseExplicit(std::string& dataLine)
{
	dataLine = boost::regex_replace(dataLine, boost::regex("<"), "");
	std::vector<std::string> data;
	boost::split(data, dataLine, boost::is_any_of("\t"));
	const std::string len = data[0];
	const std::string subclass = data[1];
	const std::string gramClass = len + subclass;
	std::vector<std::string> gram = templ->getParadigm(gramClass);
	if (!gram.size()) { return; }
	const std::string features = data[2];
	const std::string lemma = data[3];

	const int dataSize = data.size();
	for (int i = 3; i < dataSize; ++i){
		std::string currentWordform = data[i];
		std::vector<std::string> currentTags;
		boost::split(currentTags, gram[i-3], boost::is_any_of("|"));
		for (std::vector<std::string>::iterator it = currentTags.begin(); it != currentTags.end(); ++it){
			std::string currentTag = reshapeVerbTag(*it, features);
			// Case 1: infinitive, gerund, finite verb
			if (currentTag.find("Part") == currentTag.npos){
				++database[currentWordform][lemma][currentTag];
			}
			// Case 2: participle 
			{
				std::string stem;
				std::string partClass;
				if (currentTag.find(":pass") != currentTag.npos){	// passive participle might not be reflexive
					stem = currentWordform.substr(0, currentWordform.length() - 1);
					partClass = '1';
					if (stem[stem.length()-1] == 'м'){
						currentTag += ":p";	// present tense marker	
					}
					else {
						currentTag += ":a";
					}
				}
				else if(currentTag.find(":act") != currentTag.npos){
					if (currentWordform[currentWordform.length() - 1] == 'ы'){	// irreflexive
						stem = currentWordform.substr(0, currentWordform.length() - 1);
						partClass = '2';
					}
					else if(currentWordform.substr(currentWordform.length()-3 , currentWordform.length()-1) == "ыся" ){	// reflexive
						stem = currentWordform.substr(0, currentWordform.length() - 3);
						partClass = '3';
					}
					if (stem[stem.length()-1] == 'ч'){
						currentTag += ":p";	// present tense marker
					}
					else {
						currentTag += ":a";	// past tense marker
					}
				}
				std::vector<std::string> currentPartInfl = templ->getInflection(partClass);
				std::vector<std::string> currentPartParadigm = templ->getParadigmAdjAndPart(partClass);
				if (currentPartInfl.size() != currentPartParadigm.size()){
					std::cout << "ERROR" << std::endl;
				}
				else {
					for (size_t i = 0; i < currentPartInfl.size(); ++i){
						std::string currentPart = stem;
						currentPart.append(currentPartInfl[i]);
						std::vector<std::string> currentPartTags;
						boost::split(currentPartTags, currentPartParadigm[i], boost::is_any_of("|"));
						for (std::vector<std::string>::iterator it = currentPartTags.begin(); it != currentPartTags.end(); ++it){
							std::string finTag = currentTag;
							finTag.append(":").append(*it);
							++database[currentPart][lemma][finTag];
						}
					}
				}
			}
		}
	}
}

void assembler::Verb::completePOS()
{
	std::cout << "Working on verbs, participles, and gerunds..." << std::endl;
	std::cout << "\tReading verbal conjugation template...";
	templ->parseTemplate("verb");
	std::cout << "\tDone" << std::endl;

	std::cout << "\tReading participial declension templates...";
	templ->parseTemplate("part");
	std::cout << "\tDone" << std::endl;
	
	completeInnerPOS("verb_main", "the source file");
}
