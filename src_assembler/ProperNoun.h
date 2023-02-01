#pragma once

#include "NP_Main.h"
#include "NP_Excl.h"
#include "NP_Abbr.h"

#include <iostream>

namespace assembler {
	class ProperNoun : virtual public NP_Main,
		virtual public NP_Excl, virtual public NP_Abbr
	{
	public:
		ProperNoun(wordsToLemsMap& db) :
		AuxPOS(db), NP_Main(db), NP_Excl(db), NP_Abbr(db)
		{ }
		~ProperNoun() {}
		void completePOS(std::string posShort, std::string posFull);
	};
}

void assembler::ProperNoun::completePOS(std::string posShort, std::string posFull)
{
	std::cout << "Working on proper nouns..." << std::endl;
}
