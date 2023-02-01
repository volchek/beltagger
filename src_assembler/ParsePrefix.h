#pragma once

#include "POS.h"
#include <iostream>
#include <boost/algorithm/string.hpp>

namespace assembler {
	class ParsePrefix : public POS
	{
	public:
		ParsePrefix(wordsToLemsMap& db) : POS(db) { }
		virtual ~ParsePrefix() { }
		std::vector<std::string>& getLookup(std::string par);
		void setCondition(std::string cond) { grepCondition = cond; }
		void completePOS();
		void clearLookup();
	private:
		virtual void parseExplicit(std::string& dataLine);
		std::string grepCondition;
		std::map<std::string, std::vector<std::string> > lookup;
	};
}
