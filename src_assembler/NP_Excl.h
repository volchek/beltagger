#pragma once
#include "POS.h"

namespace assembler {
	const size_t NP_EXCL_COUNT = 2;

	class NP_Excl : public POS
	{
	public:
		NP_Excl(wordsToLemsMap& db);
		virtual ~NP_Excl();
		void completePOS(std::string posShort, std::string posFull);
	protected:
		void parseExplicit(std::string& dataLine);
	private:
		static const std::string properNameGroups[NP_EXCL_COUNT];
	};
}

