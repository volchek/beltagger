#ifndef TAGGERTYPES_H
#define TAGGERTYPES_H

#include <string>
#include <map>
#include <unordered_map>
#include <set>

namespace tagger {

	typedef std::unordered_map<std::string, std::pair<std::string, std::string> > database;
	typedef database::iterator databaseIter;
	typedef std::map<std::string, std::set<std::string> > wordInfo;

	struct optionValues {
		std::string inputFile, outputFile, unknownsFile, guessedFile;
		int lastColumnIndex;
		bool logUnk, logGuessed;
		bool memo, guessing;
		std::string tar;
		std::string suffixTable;
	};

	struct statistics {
		long int total, level2, level3, level4;
		long int covered, noncovered;
	};
}

#endif // TAGGERTYPES_H