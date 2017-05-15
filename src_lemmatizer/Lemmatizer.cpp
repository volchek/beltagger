#include "Lemmatizer.h"
#include "functions.h"
#include "defines.h"

#include <iostream>
#include <cstdio>
#include <ctime>
#include <algorithm>
#include <boost/algorithm/string.hpp>


tagger::database tagger::Lemmatizer::lFullDB;	// static field for the database

tagger::Lemmatizer::Lemmatizer(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar, std::locale& inputLocale) : 
		lSTweaker(new tagger::SymbolTweaker),
		lPrNumbers(new tagger::PrNumbers),
		lPrTar(tar == "sh" ? new tagger::TarShallow : new tagger::TarDeep),
		lComp(new tagger::Compounds),
		loc(inputLocale)
{
	initFields(inputFile, outputFile, lastIndex, logUnknowns, unkFile);
}



tagger::Lemmatizer::Lemmatizer(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar) : 
		lSTweaker(new tagger::SymbolTweaker),
		lPrNumbers(new tagger::PrNumbers),
		lPrTar(tar == "sh" ? new tagger::TarShallow : new tagger::TarDeep),
		lComp(new tagger::Compounds),
		loc(std::locale(LOCNAME))
{
	initFields(inputFile, outputFile, lastIndex, logUnknowns, unkFile);
}

tagger::Lemmatizer::~Lemmatizer()
{
}

void tagger::Lemmatizer::initFields(const std::string& inputFile, const std::string& outputFile, int lastIndex, bool logUnknowns, const std::string& unkFile)
{
	lLemStat.total = 0;
	lLemStat.level2 = 0;
	lLemStat.level3 = 0;
	lLemStat.level4 = 0;
	lLemStat.covered = 0;
	lLemStat.noncovered = 0;
	lLemOptions.inputFile = inputFile;
	lLemOptions.outputFile = outputFile;
	lLemOptions.unknownsFile = unkFile;
	lLemOptions.lastColumnIndex = lastIndex;
	lLemOptions.logUnk = logUnknowns;
	compiler.imbue(loc);
	belUpLetters = compiler.compile("^[÷” ≈Õ√ÿ«’‘€¬¿œ–ŒÀƒ∆›ﬂ◊—Ã≤“¡ﬁ]$");
}


/***********************
       Start
***********************/

void tagger::Lemmatizer::startLem()
{
	const std::clock_t startTime = clock();
	readDB();
	const std::clock_t intermediateTime = clock();
	std::printf("Complete in %.2f seconds.\n", ((float)(intermediateTime - startTime))/CLOCKS_PER_SEC);

	processInputFile();
	printLogInfo();

	const std::clock_t endTime = clock();
	std::printf("Complete in %.2f seconds.\n", ((float)(endTime - intermediateTime))/CLOCKS_PER_SEC);
	std::printf("%d tokens in total, %d of them recognized (%.4f).\n", (int)lLemStat.total, (int)lLemStat.covered, (lLemStat.covered/(float)lLemStat.total));
	std::printf("Recognized at once: %d (%.4f).\n", (int)(lLemStat.total - lLemStat.level2), ((lLemStat.total - lLemStat.level2)/(float)lLemStat.total));
	std::printf("2nd level gain: %d (%.4f).\n", (int)(lLemStat.level2 - lLemStat.level3), ((lLemStat.level2 - lLemStat.level3)/(float)lLemStat.total));
	std::printf("3nd level gain: %d (%.4f).\n", (int)(lLemStat.level3 - lLemStat.level4), ((lLemStat.level3 - lLemStat.level4)/(float)lLemStat.total));
	std::printf("4nd level gain: %d (%.4f).\n", (int)(lLemStat.level4 - lLemStat.noncovered), ((lLemStat.level4 - lLemStat.noncovered)/(float)lLemStat.total));
}

void tagger::Lemmatizer::printLogInfo()
{
	if (lLemOptions.logUnk == true){
		logUnknowns();
	}
}

void tagger::Lemmatizer::readDB()
{
	std::cout << "Reading database..." << std::endl;

	std::FILE* fp;
	fp = fopen("db.txt", "r");
	if (!fp){
		std::cerr << accessError("grammatical database") << std::endl;
		exit(1);
	};

	char buf[bufSize];
	char* nextToken;
	std::string wordform, lemma, tag;
	while (std::fgets(buf, sizeof(buf), fp)){
		char* trimmed = strtok_r(buf, "\n", &nextToken);
		wordform = strtok_r(trimmed, "\t", &nextToken);
		lemma = strtok_r(NULL, "\t", &nextToken);
		tag = strtok_r(NULL, "\t", &nextToken);
		lFullDB[wordform] = std::make_pair(lemma, tag);
	}
	std::fclose(fp);
}

/***********************
     Main methods
***********************/

void tagger::Lemmatizer::processInputFile()
{
	std::cout << "Processing tokenized corpus" << std::endl;
	std::ifstream filein(lLemOptions.inputFile);
	if (!filein.is_open()){
		std::cerr << "Invalid input file" << std::endl;
		exit(1);
	}

	lProcessed.open(lLemOptions.outputFile);
	if (!lProcessed.is_open()){
		std::cerr << "Invalid output file" << std::endl;
		exit(1);
	}

	// Test: get count of columns
	std::string tmp;
	for(size_t i = 0; i < 5; ++i){
		std::vector<std::string> data;
		std::getline(filein, tmp);
		boost::split(data, tmp, boost::is_any_of("\t"));
		size_t vecSize = data.size();
		if ( vecSize != lLemOptions.lastColumnIndex + 1){
			std::cout << "Error: The input file cannot be parsed into " << lLemOptions.lastColumnIndex + 1 << " columns" << std::endl;
			exit(1);
		}
	}
	filein.seekg(0, filein.beg);
	
	lLemStat.total = 0;
	std::string dataLine, wordform;
	std::vector<std::string> data(lLemOptions.lastColumnIndex + 1);

	while (std::getline(filein, dataLine)){

		if (lLemStat.total % reportingStep == 0) {
			std::cout << lLemStat.total << " tokens processed\r";
			std::cout.flush();
		}

		++lLemStat.total;
		boost::split(data, dataLine, boost::is_any_of("\t"));
		wordform = data[lLemOptions.lastColumnIndex];
		
		if (processWordform(dataLine, wordform)){
			continue;
		}
		else {
			markAsUnknown(wordform, dataLine, std::string(""));
		}
	}

	filein.close();

	lLemStat.noncovered = sumMapValues(lUnknown);
	lLemStat.covered = lLemStat.total - lLemStat.noncovered;
}

bool tagger::Lemmatizer::mainLoop(const std::string& inputLine, std::string& wordform)
{
	// (Regular tokens = R)
	// R1. "As is"
	if (processBothCases(inputLine, wordform, "")){
		return true;
	}
	
	// R2. Minor orthographic tweaks
	++lLemStat.level2;
	if (processWithTweaks(inputLine, wordform)){
		return true;
	}	

	// R3. Tarashkevitsa
	++lLemStat.level3;
	if (processTar(inputLine, wordform)){
		return true;
	}

	// R4.
	++lLemStat.level4;
	if (processHyph(inputLine, wordform)){
		return true;
	}

	if (processComp(inputLine, wordform)){
		return true;
	}

	return false;
}

bool tagger::Lemmatizer::processBothCases(const std::string& inputLine, std::string& wordform, const std::string linePostfix, const std::string& originalWordform)
{
	const std::string lcWordform = toLowerStr(wordform);
	const databaseIter itWordform = lFullDB.find(wordform);
	const databaseIter itLcWordform = lFullDB.find(lcWordform);
	if (wordform != lcWordform && itWordform != lFullDB.end() && itLcWordform != lFullDB.end()){
		processBoth(wordform, lcWordform, inputLine, linePostfix);
		return true;
	}
	else if (itWordform != lFullDB.end()){
		processOne(wordform, inputLine, linePostfix, originalWordform);
		return true;
	}
	else if (itLcWordform != lFullDB.end()){
		processOne(lcWordform, inputLine, linePostfix, originalWordform);
		return true;
	}
	else {
		return false;
	}
}


bool tagger::Lemmatizer::processWithTweaks(const std::string& inputLine, std::string& wordform)
{
	const std::string linePostfix = "";
	// Ambiguous "i"
	if (wordform == "I"){
		processSimple(inputLine, linePostfix, std::string("I|i"), std::string("RNumber|Latin|Conj:coord"));
		return true;
	}

	// Truncated name
	if (wordform.size() == 1){
		if (bx::regex_search(wordform, belUpLetters)){
			processSimple(inputLine, linePostfix, wordform, std::string("NP_initial"));
			return true;
		}
	}
	
	// Roman number
	std::pair<std::string, std::string> romanNumber = lSTweaker->checkRomanNumber(wordform);
	if (romanNumber.first != ""){
		processSimple(inputLine, linePostfix, romanNumber.first, romanNumber.second);
		return true;
	}
	
	// Minor orthographic tweaks
	std::string cleanI = lSTweaker->tweakLatinI(wordform);
	if (wordform != cleanI){
		processOne(cleanI, inputLine, linePostfix);
		return true;
	}

	std::string cleanWithUshort = lSTweaker->tweakSymbol(wordform);
	std::string clean = lSTweaker->tweakUshort(wordform);
	if (clean != ""){
		if (processBothCases(inputLine, clean, "")){
			return true;
		}
	}

	// Mixed symbols
	std::pair<std::string, std::string> cyrAndLatWords = lSTweaker->tweakMixedSymbols(wordform);
	if (cyrAndLatWords.first != ""){
		if(processBothCases(inputLine, cyrAndLatWords.first, "")){
			return true;
		}
		else {
			std::string twoLemmas = cyrAndLatWords.first;
			twoLemmas.append("|").append(cyrAndLatWords.second);
			processSimple(inputLine, "", twoLemmas, "UNK|Latin");
			return true;
		}
	}

	// Normalizing numeral roots such as "‰ÁÂ‚ˇÚ" => "‰Áˇ‚ˇÚ"
	std::string cleanWithNumerals = lSTweaker->tweakNumeral(wordform);	// clean
	if (processBothCases(inputLine, cleanWithNumerals, orthPostfix)){
		return true;
	}

	// (Tokens with numbers = N)
	// N1, see details in PrNumber::numTemplates
	std::string numberTag = lPrNumbers->checkNumber(wordform);
	if (numberTag != ""){
		processSimple(inputLine, linePostfix, wordform, numberTag);
		return true;
	}

	// N2, see details in PrNumber::numSubst
	std::pair<std::string, std::string> lemmaAndTag = lPrNumbers->checkMixedNumber(wordform);
	if (lemmaAndTag.second != ""){
		processSimple(inputLine, linePostfix, lemmaAndTag.first, lemmaAndTag.second);
		return true;
	}

	// (Other regex-detected classes = O)
	// O1. Word is Latin script
	if (lSTweaker->isLatin(wordform)){
		processSimple(inputLine, linePostfix, wordform, std::string("Latin"));
		return true;
	}
	// O2. Word is Russian script
	if (lSTweaker->isRussian(wordform)){
		processSimple(inputLine, linePostfix, wordform, std::string("Russian"));
		return true;
	}

	return false;
}

bool tagger::Lemmatizer::processTar(const std::string& inputLine, std::string& wordform)
{
	std::string tarRemoved = lPrTar->tweakTar(wordform);
	if (processBothCases(inputLine, tarRemoved, tarPostfix, wordform)){
		return true;
	}
	return false;
}

bool tagger::Lemmatizer::processHyph(const std::string& inputLine, std::string& wordform)
{
	std::string hyphRemoved = lSTweaker->tweakHyphen(wordform);
	if (processBothCases(inputLine, hyphRemoved, hyphenPostfix, wordform)){
		return true;
	}
	return false;
}

bool tagger::Lemmatizer::processComp(const std::string& inputLine, std::string& wordform)
{
	if (!lComp->splitWordform(wordform)){
		return false;
	}
	
	std::vector<std::string> res;
	if (lComp->processSimpleCases(wordform, res)){
		processSimple(inputLine, res[2], res[0], res[1]);	// res[2]: [COMPeq], [COMPadj]
		return true;
	}
	
	wordInfo adjectives;
	if(lComp->processAdj(adjectives)){
		printLine(plainOutput(adjectives, inputLine, compAdjPostfix));
		return true;
	};

	
	if(lComp->processNouns(res)){
		processSimple(inputLine, res[2], res[0], res[1]);
		return true;
	}
	
	return false;
}

/***********************
  Plain output methods
***********************/

std::string tagger::Lemmatizer::plainOutputPlugin(const wordInfo& wordformData, const std::string& linePostfix) const
{
	std::string lemmas, tags;
	for (wordInfo::const_iterator it = wordformData.begin(); it != wordformData.end(); ++it){
		lemmas += it->first;
		lemmas += '|';
		for (std::set<std::string>::const_iterator inIt = it->second.begin(); inIt != it->second.end(); ++inIt){
			tags += *inIt;
			tags += '|';
		}
		tags += '|';
	}
	lemmas.pop_back();
	tags.pop_back(); tags.pop_back();
	std::string outputStr = lemmas;
	return outputStr.append("\t").append(tags).append("\t").append(linePostfix);
}

/***********************
  Processing methods
***********************/

void tagger::Lemmatizer::processSimple(const std::string& linePrefix, const std::string& linePostfix, const std::string& lemma, const std::string& pos)
{
	std::string dataLine = linePrefix;
	dataLine.append("\t").append(lemma).append("\t").append(pos).append("\t").append(linePostfix);
	printLine(dataLine);
}

std::string tagger::Lemmatizer::processOnePlugin(const std::string& wordform, const std::string& linePostfix) const
{
	const std::pair<std::string, std::string> wordformData = lFullDB[wordform];
	std::string dataLine = wordformData.first;
	dataLine.append("\t").append(wordformData.second).append("\t").append(linePostfix);
	return dataLine;
}

void tagger::Lemmatizer::processBoth(const std::string& wordform, const std::string& modifiedWordform, const std::string& linePrefix, const std::string& linePostfix)
{
	wordInfo merged;
	std::pair<std::string, std::string> wordformData = lFullDB[wordform];
	pushData(merged, wordformData.first, wordformData.second);
	std::pair<std::string, std::string> modifiedWordformData = lFullDB[modifiedWordform];
	pushData(merged, modifiedWordformData.first, modifiedWordformData.second);
	printLine(plainOutput(merged, linePrefix, linePostfix));
}

/***********************
     Other methods
***********************/

void tagger::Lemmatizer::pushData(wordInfo& data, std::string& lemmaInfo, std::string& tagInfo)
{
	if(lemmaInfo.find("|") != lemmaInfo.npos){
		std::vector<std::string> lemmas, tags;
		boost::split(lemmas, lemmaInfo, boost::is_any_of("|"));

		std::size_t start = 0, end = 0;
		while ((end = tagInfo.find("||", start)) != std::string::npos) {
			tags.push_back(tagInfo.substr(start, end - start));
			start = end + 2;
		}
		tags.push_back(tagInfo.substr(start));
		
		const int lemmasSize = lemmas.size();
		for (int i = 0; i < lemmasSize; ++i){
			if (tags[i].find('|') != tags[i].npos){
				std::vector<std::string> currentTags;
				boost::split(currentTags, tags[i], boost::is_any_of("|"));
				for (std::vector<std::string>::const_iterator it = currentTags.begin(); it != currentTags.end(); ++it){
					data[lemmas[i]].insert(*it);
				}
			}
			else {
				data[lemmas[i]].insert(tags[i]);
			}
		}
	}
	else if (tagInfo.find('|') != tagInfo.npos) {
		std::vector<std::string> tags;
		boost::split(tags, tagInfo, boost::is_any_of("|"));
		for (std::vector<std::string>::const_iterator it = tags.begin(); it != tags.end(); ++it){
			data[lemmaInfo].insert(*it);
		}
	}
	else {
		data[lemmaInfo].insert(tagInfo);
	}
}

tagger::wordInfo tagger::Lemmatizer::grepTag(wordInfo& data, bx::sregex regex)
{
//	const boost::regex matchCondition(regex);
	wordInfo output;
	for (wordInfo::const_iterator it = data.begin(); it != data.end(); ++it){
		const std::string lemma = it->first;
		for (std::set<std::string>::const_iterator inIter = it->second.begin(); inIter != it->second.end(); ++inIter){
			const std::string currentTag = *inIter;
			if (bx::regex_search(currentTag, regex)){
				output[lemma].insert(currentTag);
			}
		}
	}
	return output;
}


/**************************
 Processing unknown tokens
**************************/

void tagger::Lemmatizer::markAsUnknown(const std::string& wordform, const std::string& linePrefix, const std::string linePostfix)
{
	++lUnknown[wordform];
	std::string dataLine = linePrefix;
	dataLine.append("\t").append(wordform).append("\tUNK\t").append(linePostfix).append("\n");
	lProcessed << dataLine;
}

void tagger::Lemmatizer::logUnknowns()
{
	std::ofstream fileout(lLemOptions.unknownsFile);
	if (!fileout.is_open()){
		std::cerr << "Invalid file for unknown tokens" << std::endl;
		return;
	}

	// TODO: Comment why in the following code unary minus is required
	// (short explanation: to simplify porting to GCC, we mustn't use
	// custom comparator in std::sort, and the output ordering should be
	// by decreasing frequency, then by increasing lexicographic order
	// of tokens).
	std::vector<std::pair<int, std::string> > mapCopy;
	for (std::map<std::string, int>::const_reverse_iterator citer = lUnknown.rbegin(); citer != lUnknown.rend(); ++citer){
		mapCopy.push_back(std::make_pair(-(citer->second), citer->first));
	}
	std::sort(mapCopy.begin(), mapCopy.end());
	for (std::vector<std::pair<int, std::string> >::const_iterator it = mapCopy.begin(); it != mapCopy.end(); ++it){
		fileout << it->second << "\t" << -(it->first) << "\n";
	}
	fileout.close();
}


/***********************
       Logging
***********************/

void tagger::Lemmatizer::printLine(const std::string& dataLine)
{
	lProcessed << dataLine << "\n";
}