#include "SymbolTweaker.h"
#include "functions.h"
#include "defines.h"
#include <iostream>
#include <fstream>


// Static fields for SymbolTweaker
const std::string tagger::SymbolTweaker::lettersOnlyCyr ="�����������������������������ء�������������������";
const std::string tagger::SymbolTweaker::lettersOnlyLat ="qwrtusdfghjklzvbnmQWRYUSDFGJLZVN";
const std::string tagger::SymbolTweaker::lettersNeutralCyr = "��������������������'-";
const std::string tagger::SymbolTweaker::lettersNeutralLat = "iIyexapocKEXBAPOCMTH'-";


tagger::SymbolTweaker::SymbolTweaker() :
	emptyLine(""),
	loc(std::locale(LOCNAME))
{
	startSymbolTweaker();
}


tagger::SymbolTweaker::SymbolTweaker(std::locale& inputLoc) : 
		emptyLine(""),
		loc(inputLoc)
{
	startSymbolTweaker();
}


tagger::SymbolTweaker::~SymbolTweaker()
{
}


void tagger::SymbolTweaker::startSymbolTweaker()
{
	compiler.imbue(loc);
	lettersOnlyCyrRegex = compiler.compile("[" + lettersOnlyCyr + "]+");
	lettersOnlyLatRegex = compiler.compile("[" + lettersOnlyLat + "]+");
	lettersBothAlphabetsDiagnostic = compiler.compile("([" + lettersOnlyCyr + "]|[" + lettersOnlyLat + "])+");
	lettersBothAlphabetsNeutral = compiler.compile("([" + lettersNeutralCyr + "]|[" + lettersNeutralLat + "])+");

	completeSet(lettersOnlyCyrSet, lettersOnlyCyr);
	completeSet(lettersOnlyLatSet, lettersOnlyLat);
	completeMap(lettersNeutralCyrMap, lettersNeutralCyr);
	completeMap(lettersNeutralLatMap, lettersNeutralLat);
}


void tagger::SymbolTweaker::completeMap(std::map<char, int>& lettersMap, const std::string& symbols)
{
	int i = 0;
	for (std::string::const_iterator it = symbols.begin(); it != symbols.end(); ++it, ++i){
		lettersMap[*it] = i;
	}
}


void tagger::SymbolTweaker::completeSet(std::set<char>& lettersSet, const std::string& symbols)
{
	for(std::string::const_iterator it = symbols.begin(); it != symbols.end(); ++it){
		lettersSet.insert(*it);
	}
}


bool tagger::SymbolTweaker::isLatin(const std::string& wordform)
{
	if ( (wordform.length() >= minLenLatinWord) &&  bx::regex_search(wordform, compiler.compile("^[a-z']+$", bx::regex_constants::icase))){
		return true;
	}
	return false;
}


bool tagger::SymbolTweaker::isRussian(const std::string& wordform)
{
	if (bx::regex_search(wordform, compiler.compile("�", bx::regex_constants::icase))){
		return false;
	}

	if (bx::regex_search(wordform, compiler.compile("[���]|��?��$", bx::regex_constants::icase)) ||
		bx::regex_search(wordform, compiler.compile("[������][����]", bx::regex_constants::icase)) ||
		bx::regex_search(wordform, compiler.compile("[���]�", bx::regex_constants::icase)) ||
		bx::regex_search(wordform, compiler.compile("�", bx::regex_constants::icase)) ||
		(bx::regex_search(wordform, compiler.compile("��", bx::regex_constants::icase)) && !bx::regex_search(wordform, compiler.compile("�|��|���|����", bx::regex_constants::icase))) ||
		(bx::regex_search(wordform, compiler.compile("��", bx::regex_constants::icase)) && !bx::regex_search(wordform, compiler.compile("��", bx::regex_constants::icase)))){
			return true;
	}

	if (wordform.length() > minLenSomeRusWord && (!wordform.find("-") != wordform.npos)) {
		if (bx::regex_search(wordform, compiler.compile("�[���������������]", bx::regex_constants::icase)) ||
			bx::regex_search(wordform, compiler.compile("��", bx::regex_constants::icase))){
			return true;
		}
	}

	int count = 0; // counts the occurrences of Cyrillic letters '�' and '�' 
	for(std::string::const_iterator it = wordform.begin(); it != wordform.end(); ++it){
		if (*it == '-' || *it == '�' || *it == '�'){
			return false;
		}
		if (*it == '�' || *it == '�'){
			++count;
		}
	}
	if (count > 2){
		return true;
	}
		
	return false;
}


std::string& tagger::SymbolTweaker::tweakNumeral(std::string& wordform)
{
	if(bx::regex_search(wordform, compiler.compile("���", bx::regex_constants::icase))){
		wordform = bx::regex_replace(wordform, compiler.compile("���(���|������|������)", bx::regex_constants::icase), "���$1");
		wordform = bx::regex_replace(wordform, compiler.compile("��������", bx::regex_constants::icase), "��������");
		wordform = bx::regex_replace(wordform, compiler.compile("��������", bx::regex_constants::icase), "��������");
	}

	if(bx::regex_search(wordform, compiler.compile("���[��]�[��]", bx::regex_constants::icase))){
		wordform = bx::regex_replace(wordform, compiler.compile("������([�����])", bx::regex_constants::icase), "������$1");
		wordform = bx::regex_replace(wordform, compiler.compile("�����(�[������]*|��)", bx::regex_constants::icase), "�����$1");	// possible prefixes: ���[��]|�����|�����|���|�����
		wordform = bx::regex_replace(wordform, compiler.compile("���([��])�����", bx::regex_constants::icase), "���$1�����");
		// TODO, possibly: "����������" => "����������" and the like
	}

	return wordform;
}


std::pair<std::string, std::string> tagger::SymbolTweaker::checkRomanNumber(std::string& wordform)
{
	if (wordform.length() <= maxLenRNumber){
		if(bx::regex_search(wordform, compiler.compile("^[I�VX�LC�]+(-[I�VX�LC�]+)*$"))){
			wordform = bx::regex_replace(wordform, compiler.compile("�"), "I");
			wordform = bx::regex_replace(wordform, compiler.compile("�"), "X");
			wordform = bx::regex_replace(wordform, compiler.compile("�"), "C");
			if (wordform.find("-") != wordform.npos){
				return std::make_pair(wordform, "RTimespan");
			}
			else {
				return std::make_pair(wordform, "RNumber");
			}
		}
		else {
			return std::make_pair("", "");
		}
	}
	else {
		return std::make_pair("", "");
	}
	return std::make_pair("", "");
}


std::string& tagger::SymbolTweaker::tweakUshort(std::string& wordform)
{
	if (wordform[0] == '�'){
		wordform[0] = '�';
	}
	else if (wordform[0] == '�'){
		wordform[0] = '�';
	}
	return wordform;
}


std::string& tagger::SymbolTweaker::tweakLatinI(std::string& wordform)
{
	if (wordform == "i"){
		wordform = "�";
	}
	return wordform;
}


std::string tagger::SymbolTweaker::tweakHyphen(std::string wordform)
{
	int countHyphen = 0;
	for (std::string::const_iterator it = wordform.begin(); it != wordform.end(); ++it){
		if (*it == '-'){ 
			++countHyphen;
		}
	}

	if (countHyphen == 1){
		wordform = bx::regex_replace(wordform, compiler.compile("(\\w)\\-(\\w)", bx::regex_constants::icase), "$1$2");
	}
	return wordform;
}


std::string& tagger::SymbolTweaker::clearMixedWordform(std::string& wordform)
{
	int countCyr = 0;
	int countLat = 0;
	std::vector<std::pair<int, int> > neutralLatIndices, neutralCyrIndices;
	int strLen = wordform.length();

	for (int i = 0; i < strLen; ++i){
		if (lettersOnlyCyrSet.find(wordform[i]) != lettersOnlyCyrSet.end()){
			++countCyr;
		}
		else if(lettersOnlyLatSet.find(wordform[i]) != lettersOnlyLatSet.end()){
			++countLat;
		}
		else if(lettersNeutralCyrMap.find(wordform[i]) != lettersNeutralCyrMap.end()){
			neutralCyrIndices.push_back(std::make_pair(i, lettersNeutralCyrMap[wordform[i]]));
		}
		else if (lettersNeutralLatMap.find(wordform[i]) != lettersNeutralLatMap.end()){
			neutralLatIndices.push_back(std::make_pair(i, lettersNeutralLatMap[wordform[i]]));
		}
	}

	if (countLat <= minCountLatinSymbols && countCyr > countLat){
		for(std::vector<std::pair<int, int> >::const_iterator it = neutralLatIndices.begin(); it != neutralLatIndices.end(); ++it){
			wordform[it->first] = lettersNeutralCyr[it->second];
		}
	}
	else if (countCyr <= minCountCyrSymbols && countLat > countCyr){
		for(std::vector<std::pair<int, int> >::const_iterator it = neutralCyrIndices.begin(); it != neutralCyrIndices.end(); ++it){
			wordform[it->first] = lettersNeutralLat[it->second];
		}
	}
	return wordform;
}


const std::string& tagger::SymbolTweaker::tweakSymbol(std::string& wordform)
{
	if (bx::regex_match(wordform, lettersOnlyCyrRegex) ||
		bx::regex_match(wordform, lettersOnlyLatRegex) ||
		bx::regex_match(wordform, lettersBothAlphabetsDiagnostic)){
			return wordform;
	}
	else if (!bx::regex_match(wordform, lettersBothAlphabetsNeutral)){
		return clearMixedWordform(wordform);
	}
	else {
		return emptyLine;
	}
}


std::pair<std::string, std::string> tagger::SymbolTweaker::tweakMixedSymbols(std::string& wordform)
{
	std::string cyrWordform = "";
	std::string latWordform = "";
	for (std::string::const_iterator it = wordform.begin(); it != wordform.end(); ++it){
		if(lettersNeutralCyrMap.find(*it) != lettersNeutralCyrMap.end()){
			cyrWordform += *it;
			latWordform += lettersNeutralLat[lettersNeutralCyrMap[*it]];
		}
		else if (lettersNeutralLatMap.find(*it) != lettersNeutralLatMap.end()){
			latWordform += *it;
			cyrWordform += lettersNeutralCyr[lettersNeutralLatMap[*it]];
		}
		else {
			return std::make_pair("", "");
		}
	}
	return std::make_pair(cyrWordform, latWordform);
}
