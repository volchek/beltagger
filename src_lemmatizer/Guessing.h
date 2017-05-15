#ifndef GUESSING_H
#define GUESSING_H

#include "taggerTypes.h"

#include <string>
#include <vector>
#include <map>
#include <unordered_map>

namespace tagger {
	class Guessing
	{
		typedef std::unordered_map<std::string, std::vector<std::pair<std::string, std::string> > > sufInfo;
	public:
		Guessing();
		explicit Guessing(std::string, std::string);
		virtual ~Guessing();
		void startGuessing();
		bool guessTag(std::string&, wordInfo&);	// a straightforward function for tag guessing

	private:
		std::string suffixTable;
		std::string logFile;
		std::map<double, size_t> thresholds;	// threshold system used in hypothesizing lemmas and tags
		std::map<std::string, double> entropy;
		sufInfo guess;

		void completeThresholds();
		bool loadSuffixTable(std::string&);	// loading suffix table for guessing, if needed
		int limitGuessing(double);
	};
}

#endif // GUESSING_H