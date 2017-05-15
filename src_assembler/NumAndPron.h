#pragma once
#include "POS.h"

namespace assembler {
	class NumAndPron : public POS
	{
	public:
		NumAndPron(wordsToLemsMap& db);
		virtual ~NumAndPron();
	private:
		void parseExplicit(std::string& dataLine);
	};
}

