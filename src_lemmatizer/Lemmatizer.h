#ifndef LEMMATIZER_H
#define LEMMATIZER_H

#include "taggerTypes.h"
#include "SymbolTweaker.h"
#include "PrNumbers.h"
#include "TarShallow.h"
#include "TarDeep.h"
#include "Compounds.h"
#include "constants.h"

#include <fstream>
#include <locale>
#include <boost/shared_ptr.hpp>

namespace tagger {

	class Lemmatizer
	{
		friend class Compounds;

	public:
		Lemmatizer();
		Lemmatizer(std::string&, std::string&, int, bool, std::string&, std::string);
		Lemmatizer(std::string&, std::string&, int, bool, std::string&, std::string, std::locale&);
		virtual ~Lemmatizer();

	protected:
		void initFields(const std::string&, const std::string&, int, bool, const std::string&);
		void startLem();
		std::string plainOutputPlugin(const wordInfo&, const std::string&) const;
		std::string processOnePlugin(const std::string&, const std::string&) const;
		bool processBothCases(const std::string&, std::string&, const std::string, const std::string&);
		inline bool processBothCases(const std::string& inputLine, std::string& wordform, const std::string postfix)
			{ return processBothCases(inputLine, wordform, postfix, wordform); }
		void printLine(const std::string&);
		bool mainLoop(const std::string&, std::string&);
		virtual void printLogInfo();

	private:
		static database lFullDB;
		std::map<std::string, int> lUnknown;
		std::ofstream lProcessed;
		std::locale loc;
		bx::sregex_compiler compiler;
		bx::sregex belUpLetters;
		optionValues lLemOptions;
		statistics lLemStat;
		boost::shared_ptr<tagger::PrNumbers> lPrNumbers;
		boost::shared_ptr<tagger::SymbolTweaker> lSTweaker;
		boost::shared_ptr<tagger::TarShallow> lPrTar;
		boost::shared_ptr<tagger::Compounds> lComp;

		static void readDB();
		void processInputFile();
		virtual bool processWordform(std::string&, std::string&) = 0;
		bool processWithTweaks(const std::string&, std::string&);
		bool processTar(const std::string&, std::string&);
		bool processHyph(const std::string&, std::string&);
		bool processComp(const std::string&, std::string&);

		virtual std::string plainOutput(const wordInfo&, const std::string&, const std::string&) = 0;
		virtual void processOne(const std::string&, const std::string&, const std::string&, const std::string& originalWordform = std::string("")) = 0;
		void processSimple(const std::string&, const std::string&, const std::string&, const std::string&);
		void processBoth(const std::string&, const std::string&, const std::string&, const std::string&);

		static void pushData(std::map<std::string, std::set<std::string> >&, std::string&, std::string&);
		static wordInfo grepTag(wordInfo&, bx::sregex);

		void markAsUnknown(const std::string&, const std::string&, const std::string);
		void logUnknowns();
	};
}
#endif // LEMMATIZER_H