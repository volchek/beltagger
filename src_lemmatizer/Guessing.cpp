#include "Guessing.h"
#include "functions.h"

#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ctime>

tagger::Guessing::Guessing()
{
}

tagger::Guessing::Guessing(std::string currentSufTable, std::string currentLogFile = "") :
	suffixTable(currentSufTable), logFile(currentLogFile)
{
}

tagger::Guessing::~Guessing()
{
}

void tagger::Guessing::completeThresholds()
{
	thresholds[0.585] = 1;	// ~log2(1.5)
	thresholds[1.322] = 2;	// ~log2(2.5)
	thresholds[1.808] = 3;	// ~log2(3.5)
	thresholds[2.17] = 4;	// ~log2(4.5)
	thresholds[2.46] = 5;	// ~log2(5.5)
}

void tagger::Guessing::startGuessing()
{
	const std::clock_t startTime = clock();
	completeThresholds();
	loadSuffixTable(suffixTable);
	const std::clock_t endTime = clock();
	std::printf("Complete in %.2f seconds.\n", ((float)(endTime - startTime))/CLOCKS_PER_SEC);

}

int tagger::Guessing::limitGuessing(double currentEntropy)
{
	for(std::map<double, size_t>::const_iterator it = thresholds.begin(); it != thresholds.end(); ++it){
		if(currentEntropy <= it->first){
			return it->second - 1;
		}
	}
	return -1;
}

bool tagger::Guessing::loadSuffixTable(std::string& fileName)
{
	std::cout << "Reading suffix table for lemma and tag guessing..." << std::endl;
	std::ifstream filein(fileName);
	if (!filein.is_open()){
		std::cerr << accessError("suffix table for guessing") << std::endl;
		return false;
	}

	std::string currentLine;
	std::string suffix, oneCandidate;
	int frequency;
	double entr;
	while (std::getline(filein, currentLine)){
		std::istringstream ss(currentLine);
		ss >> suffix >> frequency >> entr;
		std::vector<std::pair<std::string, std::string> > data;
		while (ss >> oneCandidate){
			std::string lemmaSuffix, tag;
			std::string::const_iterator it = oneCandidate.begin();
			while (*it != '{'){
				lemmaSuffix += *it;
				++it;
			}
			++it;
			while (*it != '}'){
				tag += *it;
				++it;
			}
			data.push_back(std::make_pair(lemmaSuffix, tag));
		}
		guess[suffix] = data;
		entropy[suffix] = entr;
	}
	
	filein.close();
	return true;
}

bool tagger::Guessing::guessTag(std::string& wordform, tagger::wordInfo& info)
{
	std::map<double, std::set<std::string> > suffixes;
	size_t wLen = wordform.length(); 
	for (size_t i = 0; i < wLen; ++i){
		std::string currentSuf = wordform.substr(i, wLen - i);
		std::map<std::string, double>::const_iterator it = entropy.find(currentSuf);
		if (it != entropy.end()){
			suffixes[it->second / currentSuf.length()].insert(currentSuf);
		}
	}
	
	if (suffixes.empty()){
		return false;
	}

	// Find a suffix with the least tag entropy
	std::list<std::string> best;
	std::map<double, std::set<std::string> >::const_iterator it = suffixes.begin();
	for (std::set<std::string>::const_iterator inIt = it->second.begin(); inIt != it->second.end(); ++inIt){
		best.push_back(*inIt);
	}

	best.sort(sortLenCondition);
	if (best.size() == 0){
		return false;
	}
	std::string winner = best.front();

	int lastIndex = limitGuessing(entropy[winner]);
	if (lastIndex < 0){	// if the least entropy overcomes the highest threshold, give up
		return false;
	}

	// Return an entropy-proportional subset of the list of possible tags
	std::string stem = wordform.substr(0, wLen - winner.length());
	std::vector<std::pair<std::string, std::string> >::iterator vecIt = guess[winner].begin();
	for (int i = 0; i <= lastIndex; ++i){
		std::vector<std::pair<std::string, std::string> >::iterator curIt = vecIt + i;
		std::string currentLemma = stem;
		if (curIt->first != ""){
			currentLemma += curIt->first;	
		}
		std::string currentTag = curIt->second;
		info[currentLemma].insert(currentTag);
	}
	return true;
}
