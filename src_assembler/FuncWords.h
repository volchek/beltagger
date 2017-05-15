#pragma once

#include "AuxPOS.h"
#include <iostream>
#include <string>

namespace assembler {
	const size_t FW_COUNT = 6;

	class FuncWords : public AuxPOS
	{
	public:
		FuncWords(wordsToLemsMap& db) : AuxPOS(db)
			{ completePOS(); }
		virtual ~FuncWords(){ }
	private:
		void completePOS();
		static const std::string functionWords[FW_COUNT];
	};

	const std::string FuncWords::functionWords[FW_COUNT] = {"conjunctions", "prepositions", 
		"particles", "modals", "interjections", "punctuation"};

	void FuncWords::completePOS()
	{
		std::cout << "Working on function words..." << std::endl;
		for(size_t i = 0; i < FW_COUNT; ++i){
			std::cout << "\tReading the " << functionWords[i] << " source file...";
			readFile("fw_" + functionWords[i]);
			std::cout << "\tDone" << std::endl;
		}
	}
}
