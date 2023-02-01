#include "Composer.h"
#include "Adverb.h"
#include "Predicatives.h"
#include "CommonAbbr.h"
#include "FuncWords.h"
#include "Pronoun.h"
#include "Numeral.h"
#include "NP_Abbr.h"
#include "NP_Main.h"
#include "NP_Excl.h"
#include "Adjective.h"
#include "AdjAddition.h"
#include "AdjComp.h"
#include "AdjExcl.h"
#include "Noun.h"
#include "NounAddition.h"
#include "NounExcl.h"
#include "Verb.h"
#include "POS.h"

#include <boost/program_options.hpp>
#include <iostream>

Composer::Composer(int argc, char** argv) :
	outputFormat("detailed")
	, objTemplate(new assembler::POS_Template(database))
	, objParsePrefix(new assembler::ParsePrefix(database))
{
	getOutputOption(argc, argv);
	assembler::POS::setPathPrefix(inputPath);
	completeDB();
}

Composer::~Composer()
{
}


/**********************
 Command line options
***********************/

void Composer::getOutputOption(int argc, char** argv){
	namespace po = boost::program_options;
	po::options_description desc("Options");
	desc.add_options()
		("format,f", po::value<std::string>(), "output format")
		("input-dir,i", po::value<std::string>(), "input directory")
		("output-file,o", po::value<std::string>(), "output file");
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		// -f: Output format
		if (vm.count("format")){
			std::string format = vm["format"].as<std::string>();
			if (format == "detailed" || format == "simple" || format == "dummy"){
				outputFormat = format;
			}
			else {
				std::cerr << "Invalid -f flag. Using the default output format.\n" << std::endl;
			}
		}

		// -i: Input directory
		if (vm.count("input-dir")){
			inputPath = vm["input-dir"].as<std::string>();
		}

		// -o: Output file ("tagged_" + input file name by default)
		if (vm.count("output-file")){
			outputFile = vm["output-file"].as<std::string>();
		}
		else {
			outputFile = "db.txt";
		}

		po::notify(vm);
	}
	catch (std::exception&){
		std::cerr << "No -f value supplied. Using the default output format.\n" << std::endl;
	}
}

void Composer::completeDB()
{
	using namespace assembler;
	Adverb adv(database);
	Predicatives pred(database);
	CommonAbbr abbr(database);
	FuncWords fw(database);
	Pronoun pron(database);
	Numeral num(database);
	completeProperNouns();
	completeAdjectives();
	completeCommonNouns();
	Verb verb(database, objTemplate);
}

void Composer::completeProperNouns()
{
	using namespace assembler;
	std::cout << "Working on proper nouns..." << std::endl;
	NP_Main np_main(database);
	NP_Excl np_excl(database);
	NP_Abbr np_abbr(database);
}


void Composer::completeAdjectives()
{
	using namespace assembler;
	Adjectives adj(database, objTemplate, objParsePrefix);
	AdjAddition addAdj(database, objTemplate);
	AdjComp compAdj(database, objTemplate);
	AdjExcl exclCompAdj(database);
	objParsePrefix->completePOS();
}

void Composer::completeCommonNouns()
{
	using namespace assembler;
	Noun noun(database, objTemplate, objParsePrefix);
	NounAddition addNouns(database);
	NounExcl exclNouns(database);
	objParsePrefix->completePOS();
}

/**********************
		Output
***********************/

void Composer::outputDB() const
{
	std::cout << "Writing output...";
	std::ofstream fileout(outputFile);
	if (!fileout.is_open()){
		std::cout << "Unable to write into db.txt" << std::endl;
		return;
	}

	if (outputFormat == "detailed"){
		outputDetailed(fileout);
	}
	else if (outputFormat == "simple"){
		outputSimple(fileout);
	}
	else // outputFormat == "dummy"
	{
		outputDummy(fileout);
	}
	fileout.close();
	std::cout << "\tDone" << std::endl;
}

void Composer::outputDetailed(std::ofstream& fileout) const
{
	using namespace assembler;
	wordsToLemsMap::const_iterator it;
	for (it = database.begin(); it != database.end(); ++it){
		std::string lemmata, grams;
		lemToGramsMap::const_iterator iter;
		for (iter = it->second.begin(); iter != it->second.end(); ++iter){
			lemmata += iter->first;
			gramsToIntMap::const_iterator inIter;
			for (inIter = iter->second.begin(); inIter != iter->second.end(); ++inIter){
				grams += inIter->first;
				grams += "|";
			}
			grams += "|";
			lemmata += "|";
		}
		lemmata.pop_back();
		grams.pop_back(); grams.pop_back(); // two trailing pipes need to be erased
		fileout << it->first << "\t" << lemmata << "\t" << grams << "\n";
	}
}

void Composer::outputSimple(std::ofstream& fileout) const
{
	using namespace assembler;
	wordsToLemsMap::const_iterator it;
	for (it = database.begin(); it != database.end(); ++it){
		const std::string wordform = it->first;
		std::string lemmata;
		std::set<std::string> grams;
		std::string strGrams;
		lemToGramsMap::const_iterator iter;
		for (iter = it->second.begin(); iter != it->second.end(); ++iter){
			lemmata += iter->first;
			gramsToIntMap::const_iterator inIter;
			for (inIter = iter->second.begin(); inIter != iter->second.end(); ++inIter){
				grams.insert(inIter->first);
			}
			lemmata += "|";
		}
		for(std::set<std::string>::const_iterator setIter = grams.begin(); setIter != grams.end(); ++setIter){
			strGrams += *setIter;
			strGrams += "|";
		}
		lemmata.pop_back();
		strGrams.pop_back();
		fileout << wordform << "\t" << lemmata << "\t" << strGrams << "\n";
	}
}

void Composer::outputDummy(std::ofstream& fileout) const
{
	using namespace assembler;
	wordsToLemsMap::const_iterator it;
	for (it = database.begin(); it != database.end(); ++it){
		const std::string wordform = it->first;
		lemToGramsMap::const_iterator iter;
		for (iter = it->second.begin(); iter != it->second.end(); ++iter){
			const std::string lemma = iter->first;
			gramsToIntMap::const_iterator inIter;
			for (inIter = iter->second.begin(); inIter != iter->second.end(); ++inIter){
				const std::string gram = inIter->first;
				fileout << wordform << "\t" << lemma << "\t" << gram << "\n";
			}
		}
	}
}
