#pragma once

#include "AuxPOS.h"

namespace assembler {
	class NP_Abbr : virtual public AuxPOS
	{
	public:
		NP_Abbr(wordsToLemsMap& db) : AuxPOS(db)
			{ completePOS("np_abbr", "abbreviations"); }
		virtual ~NP_Abbr(){ }
		void completePOS(std::string posShort, std::string posFull){
			std::cout << "\tReading " << posFull << "...";
			readFile(posShort);
			std::cout << "\tDone" << std::endl;
		}
	};
}
