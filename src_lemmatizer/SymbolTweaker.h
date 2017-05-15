#ifndef SYMBOLTWEAKER_H
#define SYMBOLTWEAKER_H

#include "constants.h"
#include <map>
#include <set>
#include <string>
#include <locale>
#include <boost/xpressive/xpressive.hpp>

namespace tagger{

	class SymbolTweaker
	{
	public:
		SymbolTweaker();
		SymbolTweaker(std::locale&);
		~SymbolTweaker();
		bool isLatin(const std::string&);
		bool isRussian(const std::string&);
		std::string& tweakNumeral(std::string&);
		std::string& tweakUshort(std::string&);
		std::string& tweakLatinI(std::string&);
		const std::string& tweakSymbol(std::string&);
		std::string tweakHyphen(std::string);
		std::pair<std::string, std::string> checkRomanNumber(std::string&);
		std::pair<std::string, std::string> tweakMixedSymbols(std::string&);

	private:
		static const std::string lettersOnlyCyr;
		static const std::string lettersOnlyLat;
		static const std::string lettersNeutralCyr;
		static const std::string lettersNeutralLat;
		std::map<char, int> lettersNeutralCyrMap, lettersNeutralLatMap;
		std::set<char> lettersOnlyCyrSet, lettersOnlyLatSet;

		const static int minLenLatinWord = 3;
		const static int minLenSomeRusWord = 5;
		const static int maxLenRNumber = 10;
		const static int minCountLatinSymbols = 2;
		const static int minCountCyrSymbols = 2;
		std::string emptyLine;

		std::locale loc;
		bx::sregex_compiler compiler;
		bx::sregex lettersOnlyCyrRegex;
		bx::sregex lettersOnlyLatRegex;
		bx::sregex lettersBothAlphabetsDiagnostic;
		bx::sregex lettersBothAlphabetsNeutral;

		void completeSet(std::set<char>&, const std::string&);
		void completeMap(std::map<char, int>&, const std::string&);
		void startSymbolTweaker();
		std::string& clearMixedWordform(std::string&);

	};
}
#endif // SYMBOLTWEAKER_H

