#pragma once

#include "POS.h"

namespace assembler {
	const size_t NP_MAIN_COUNT = 3;

	class NP_Main :	public POS
	{
	public:
		NP_Main(wordsToLemsMap& db);
		virtual ~NP_Main();
		void completePOS(std::string posShort, std::string posFull);
	protected:
		void parseExplicit(std::string& dataLine);
	private:
		static const std::string properNameGroups[NP_MAIN_COUNT];
	};
}
