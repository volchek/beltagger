#ifndef COMPOUNDS_H
#define COMPOUNDS_H

#include "taggerTypes.h"
#include "constants.h"
#include <string>
#include <vector>
#include <map>
#include <locale>
#include <boost/xpressive/xpressive.hpp>

namespace tagger {

	class Compounds
	{
		typedef std::vector<std::pair<std::string, std::string> > compInfo;
	public:
		Compounds();
		Compounds(std::locale& loc);
		~Compounds();
		bool splitWordform(std::string&);
		bool processSimpleCases(std::string&, std::vector<std::string>&);
		bool processAdj(tagger::wordInfo&);	// words like "прыгожа-гнуткі"
		bool processNouns(std::vector<std::string>&);	// words like "палац-прыгажун", "школа-інтэрнат"

	private:
		std::locale loc;
		bx::sregex_compiler compiler;
		std::map<std::string, std::string> adjSubst;
		std::vector<std::string> cWordformParts;
		compInfo cPartsInfo;
		std::vector<wordInfo> cSplitPartsInfo;
		const static size_t genderIndex = 4;

		void startCompounds();
		void completeAdjSubst();
		compInfo findInfo(const std::vector<std::string>&);
		bool checkEqTags(compInfo&) const;
		bool checkExistTags(compInfo&) const;
		bool checkAdj(const std::string&);
		bool processEqualParts(compInfo&, std::vector<std::string>&);	// words like "рэжысёр-пастаноўшчык"
		bool processNumbAndAdj(std::vector<std::string>&, compInfo&, std::vector<std::string>&);	// words like "8-вёсельны"
		void splitInfo(compInfo&, std::vector<wordInfo>&);
		void clearFields();
		bool intersectMaps(std::vector<wordInfo>&, std::vector<wordInfo>&);
		std::set<std::string>& populateTagsSet(const wordInfo&, std::set<std::string>&);
		std::string createRegex(std::set<std::string>&);
		std::vector<std::string> createWordformInfo(std::vector<wordInfo>&);
		std::string createLemma(std::vector<wordInfo>&);
		std::string createTag(std::set<std::string>&);
	};
}

#endif // COMPOUNDS_H
