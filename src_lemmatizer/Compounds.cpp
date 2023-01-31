#include "Compounds.h"
#include "functions.h"
#include "defines.h"
#include "Lemmatizer.h"
#include <iostream>
#include <algorithm>
#include <string>
#include <boost/algorithm/string.hpp>


tagger::Compounds::Compounds() :
	loc(std::locale(LOCNAME))
{
	startCompounds();
}

tagger::Compounds::Compounds(std::locale& inputLocale) :
	loc(inputLocale)
{
	startCompounds();
}


tagger::Compounds::~Compounds()
{
}

void tagger::Compounds::startCompounds()
{
	compiler.imbue(loc);
	completeAdjSubst();
}

void tagger::Compounds::completeAdjSubst()
{
	adjSubst["ага"] = "Adj:msG|Adj:msA|Adj:nsG|Adj:msA";
	adjSubst["ае"] = "Adj:nsN|Adj:nsA|Adj:fsG";
	adjSubst["ай"] = "Adj:fsG|Adj:fsD|Adj:fsI|Adj:fsL";
	adjSubst["аму"] = "Adj:msD|Adj:nsD";
	adjSubst["аю"] = "Adj:fsI";
	adjSubst["а€"] = "Adj:fsN";
	adjSubst["ую"] = "Adj:fsA";
	adjSubst["ы"] = "Adj:msN|Adj:msA";
	adjSubst["ым"] = "Adj:msI|Adj:msL|Adj:nsI|Adj:nsL|Adj:0pD";
	adjSubst["ым≥"] = "Adj:0pI";
	adjSubst["ых"] = "Adj:0pG|Adj:0pA|Adj:0pL";
	adjSubst["ы€"] = "Adj:0pN|Adj:0pA";
}

// Main functions

bool tagger::Compounds::processSimpleCases(std::string& wordform, std::vector<std::string>& res)
{
	if (processEqualParts(cPartsInfo, res)){
		return true;
	}
	if (processNumbAndAdj(cWordformParts, cPartsInfo, res)){
		return true;
	}
	return false;
}

bool tagger::Compounds::processEqualParts(compInfo& partsInfo, std::vector<std::string>& res)
{
	if (!checkEqTags(partsInfo)){
		return false;
	}

	std::string lemma = "";
	for (compInfo::const_iterator it = partsInfo.begin(); it != partsInfo.end(); ++it){
		lemma += it->first;
		lemma += '-';
	}
	lemma.pop_back();
	res.push_back(lemma);
	res.push_back(partsInfo[0].second);
	res.push_back(compEqPostfix);
	return true;
}

bool tagger::Compounds::processNumbAndAdj(std::vector<std::string>& wordformParts, compInfo& partsInfo, std::vector<std::string>& res)	// words like "8-вЄсельны"
{
	if (wordformParts.size() != 2){
		return false;
	}

	if (bx::regex_match(wordformParts[0], compiler.compile("\\d+"))){
		std::string lemma = wordformParts[0];
		bx::smatch m;
		wordInfo splitPartsInfo;
		Lemmatizer::pushData(splitPartsInfo, partsInfo[1].first, partsInfo[1].second);
		wordInfo adjInfo = Lemmatizer::grepTag(splitPartsInfo, compiler.compile("Adj"));
		if (adjInfo.size() != 0){
			std::pair<std::string, std::string> finishAdjInfo = conjoinParts(adjInfo);
			lemma.append("-").append(finishAdjInfo.first);
			res.push_back(lemma);
			res.push_back(finishAdjInfo.second);
			res.push_back(compAdjPostfix);
			return true;
		}
		else if (bx::regex_match(wordformParts[1], m, compiler.compile("(гадов|к≥ламетров|пав€рхов|томн|тонн)(ы(х|м≥?|€)?|а(га|му|€|й)|ую)"))){
			lemma.append("-").append(m[1]).append("ы");
			res.push_back(lemma);
			res.push_back(adjSubst[m[2]]);
			res.push_back(compAdjPostfix);
			return true;
		}
	}
	return false;
}

bool tagger::Compounds::processAdj(wordInfo& res)
{
	if (cPartsInfo.size() < 2) {
		return false;
	}
	
	wordInfo lastPartInfo = cSplitPartsInfo[cSplitPartsInfo.size() - 1];
	Lemmatizer::pushData(lastPartInfo, cPartsInfo[cPartsInfo.size() - 1].first, cPartsInfo[cPartsInfo.size() - 1].second);
	std::string lemma = "";
	for (size_t i = 0; i < cWordformParts.size() - 1; ++i){
		if (cPartsInfo[i].second == "Adv" || checkAdj(cWordformParts[i])){
			lemma += cWordformParts[i];
			lemma += "-";
		}
		else { return false; }
	}
	wordInfo adjectives = Lemmatizer::grepTag(lastPartInfo, compiler.compile("^Adj:"));
	if (adjectives.size() != 0){
		for (wordInfo::iterator it = adjectives.begin(); it != adjectives.end(); ++it){
			std::string fullLemma = lemma;
			fullLemma += it->first;
			res[fullLemma] = it->second;
		}
		return true;
	}
	return false;
}

bool tagger::Compounds::processNouns(std::vector<std::string>& res)
{
	std::vector<wordInfo> nouns;
	for(std::vector<wordInfo>::iterator it = cSplitPartsInfo.begin(); it != cSplitPartsInfo.end(); ++it){
		wordInfo currentNoun = Lemmatizer::grepTag(*it, compiler.compile(":[mfnc0]:[sp][NGDAIL]$"));
		if (currentNoun.empty()){
			return false;
		}
		nouns.push_back(currentNoun);
	}
		
	std::vector<wordInfo> tags;
	if (!intersectMaps(nouns, tags)){
		return false;
	}
	
	res = createWordformInfo(tags);
	if (!res.empty()){
		return true;
	}

	return false;
}

// Auxiliary functions

bool tagger::Compounds::splitWordform(std::string& wordform)
{
	if (!cPartsInfo.empty()){
		clearFields();
	}
	if (wordform.find("-") != wordform.npos || wordform.find("Ц") != wordform.npos){
		std::string tmp = wordform;
		boost::split(cWordformParts, tmp, boost::is_any_of("-Ц"));
		cPartsInfo = findInfo(cWordformParts);
		splitInfo(cPartsInfo, cSplitPartsInfo);
		return true;
	}
	return false;
}

void tagger::Compounds::clearFields()
{
		cPartsInfo.clear();
		cWordformParts.clear();
		cSplitPartsInfo.clear();
}

tagger::Compounds::compInfo tagger::Compounds::findInfo(const std::vector<std::string>& wordformParts)
{
	compInfo res;
	size_t i = 0;
	for (std::vector<std::string>::const_iterator it = wordformParts.begin(); it != wordformParts.end(); ++it, ++i){
		const databaseIter elemIter = Lemmatizer::lFullDB.find(*it);
		std::string lcPart = toLowerStr(*it);
		const databaseIter lcElemIter = Lemmatizer::lFullDB.find(lcPart);
		if (elemIter != Lemmatizer::lFullDB.end()){
			res.push_back(elemIter->second);
		}
		else if (lcElemIter != Lemmatizer::lFullDB.end()){
			res.push_back(lcElemIter->second);
			cWordformParts[i] = toLowerStr(cWordformParts[i]);
		}
		else {
			res.push_back(std::make_pair("", ""));
		}
	}
	return res;
}

void tagger::Compounds::splitInfo(compInfo& partsInfo, std::vector<tagger::wordInfo>& splitInfo)
{
	size_t i = 0;
	splitInfo.resize(partsInfo.size());
	for(compInfo::iterator it = partsInfo.begin();  it != partsInfo.end(); ++it){
		Lemmatizer::pushData(splitInfo[i], it->first, it->second);
		++i;
	}
}

bool tagger::Compounds::checkEqTags(compInfo& partsInfo) const
{
	std::string firstGram = partsInfo[0].second;
	if (firstGram != ""){
		for (size_t i = 1; i < partsInfo.size(); ++i){
			if (partsInfo[i].second == firstGram){
				continue;
			}
			else { return false; }
		}
		return true;
	}
	else { return false; }
}

bool tagger::Compounds::checkExistTags(compInfo& partsInfo) const
{
	for(compInfo::const_iterator it = partsInfo.begin(); it != partsInfo.end(); ++it){
		if (it->second == ""){
			return false;
		}
	}
	return true;
}

bool tagger::Compounds::checkAdj(const std::string& wordform)
{
	char lastSymb = wordform[wordform.size() - 1];
	std::string stem = wordform.substr(0, wordform.size() - 1);
	std::vector<std::string> possibleAdj; // size of this vector is guaranteed to be 0 or 1
	if (lastSymb == 'е' || lastSymb == '€'){
		stem += "≥";
		possibleAdj.push_back(stem);
	}
	else if (lastSymb == 'а'){
		char lastStemSymb = stem[stem.size() - 1];
		if (lastStemSymb == 'г' || lastStemSymb == 'к' || lastStemSymb == 'х' ){
			stem += "≥";
			possibleAdj.push_back(stem);
		}
		else { 
			stem += "ы";
			possibleAdj.push_back(stem);
		}
	}
	if (possibleAdj.size() == 0){
		return false;
	}
	compInfo possibleAdjInfo = findInfo(possibleAdj); // now size of possibleAdjInfo is guaranteed to be 1
	if (possibleAdjInfo[0].second.find("Adj:") != possibleAdjInfo[0].second.npos){
		return true;
	}
	return false; 
}

std::set<std::string>& tagger::Compounds::populateTagsSet(const tagger::wordInfo& info, std::set<std::string>& tagsSet)
{
	for(wordInfo::const_iterator it = info.begin(); it != info.end(); ++it){
		for (std::set<std::string>::const_iterator inIt = it->second.begin(); inIt != it->second.end(); ++inIt){
			std::string oneTags = inIt->substr(inIt->size() - 2, 2);
			tagsSet.insert(oneTags);
		}
	}
	return tagsSet;
}

bool tagger::Compounds::intersectMaps(std::vector<wordInfo>& vec, std::vector<wordInfo>& res)
{
	size_t lastIndex = vec.size() - 1;
	std::set<std::string> candidates;
	candidates = populateTagsSet(vec[lastIndex], candidates);
	if (candidates.empty()){ return false; } // think of a situation when there are some lemmas but no tags are attached to them

	for(size_t i = 0; i < lastIndex; ++i){
		std::set<std::string> currentPartTags;
		currentPartTags = populateTagsSet(vec[i], currentPartTags);
		if (currentPartTags.empty()){ return false; }

		// Solution borrowed from stackoverflow.com/questions/2874441
		std::set<std::string>::iterator candIt = candidates.begin();
		while (candIt != candidates.end()){
			std::set<std::string>::iterator currentIt = candIt++;
			std::set<std::string>::const_iterator tagIt = currentPartTags.find(*currentIt);
			if (tagIt == currentPartTags.end()){
				candidates.erase(currentIt);
			}
		}
		if (candidates.empty()){ return false; }
	}

	std::string regex = createRegex(candidates);
	for (std::vector<wordInfo>::iterator it = vec.begin(); it != vec.end(); ++it){
		res.push_back(Lemmatizer::grepTag(*it, compiler.compile(regex)));
	}
	
	return true;
}

std::string tagger::Compounds::createRegex(std::set<std::string>& tags)
{
	std::string regex;
	if (tags.size() == 1){
		std::set<std::string>::iterator it = tags.begin();
		regex = *it;
	}
	else {
		regex = "(";
		for (std::set<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it){
			regex += *it;
			regex += "|";
		}
		regex.pop_back();
		regex += ")$";
	}
	return regex;
}

std::string tagger::Compounds::createLemma(std::vector<wordInfo>& info)
{
	std::vector<std::vector<std::string> > allLemmas;
	for(std::vector<wordInfo>::const_iterator it = info.begin(); it != info.end(); ++it){
		std::vector<std::string> currentLemmas;
		for(wordInfo::const_iterator inIt = it->begin(); inIt != it->end(); ++inIt){
			currentLemmas.push_back(inIt->first);
		}
		allLemmas.push_back(currentLemmas);
	}
	
	std::vector<std::vector<std::string> > lemmas = getCartesianProduct(allLemmas);
	std::string res = "";
	if (lemmas.empty()){
		return res;
	}
	
	for(std::vector<std::vector<std::string> >::const_iterator it = lemmas.begin(); it != lemmas.end(); ++it){
		for(std::vector<std::string>::const_iterator inIt = it->begin(); inIt != it->end(); ++inIt){
			res += *inIt;
			res += "-";
		}
		res.pop_back();
		res += "|";
	}
	res.pop_back();
	return res;
}

std::vector<std::string> tagger::Compounds::createWordformInfo(std::vector<wordInfo>& info)
{
	std::set<std::string> gender;
	std::set<std::string> tags;
	for(std::vector<wordInfo>::const_iterator it = info.begin(); it != info.end(); ++it){
		for(wordInfo::const_iterator inIt = it->begin(); inIt != it->end(); ++inIt){
			for (std::set<std::string>::const_iterator setIt = inIt->second.begin(); setIt != inIt->second.end(); ++setIt){
				gender.insert(setIt->substr(setIt->size() - genderIndex, 1));
				tags.insert(*setIt);
			}
		}
	}

	std::string postfix = compPartCountPostfix;
	std::string partCount = std::to_string(cWordformParts.size());
	if (gender.size() > 1){
		partCount += "n";
	}
	postfix.replace(postfix.find("#"), 1, partCount);

	std::string lemma = createLemma(info);
	std::vector<std::string> res;
	if (lemma != ""){
		res.push_back(lemma);
		res.push_back(createTag(tags));
		res.push_back(postfix);
	}
	return res;
}

std::string tagger::Compounds::createTag(std::set<std::string>& tags)
{
	std::string tag = "";
	for(std::set<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it){
		tag += *it;
		tag += "|";
	}
	tag.pop_back();
	return tag;
}
