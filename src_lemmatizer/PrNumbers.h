#ifndef PRNUMBERS_H
#define PRNUMBERS_H

#include "constants.h"
#include <map>
#include <vector>
#include <string>
#include <locale>
#include <boost/xpressive/xpressive.hpp>

namespace tagger {
	class PrNumbers
	{
	public:
		PrNumbers(std::locale&);
		PrNumbers();
		~PrNumbers();
		std::string checkNumber(const std::string&);
		std::pair<std::string, std::string> checkMixedNumber(const std::string&);
		std::string checkRomanNumber(std::string&);

	private:
		std::locale loc;
		bx::sregex_compiler compiler;
		const static int maxLenRNumber = 10;
		std::vector<std::pair<bx::sregex, std::string> > numTemplates;
		std::map<std::string, std::string> numSubst;
		void startPrNumbers();
		void completeNumTemplates();
		void completeNumSubst();
	};
}
#endif // PRNUMBERS_H
