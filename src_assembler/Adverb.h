#pragma once

#include "AuxPOS.h"

namespace assembler {
	class Adverb : public AuxPOS
	{
	public:
		Adverb(wordsToLemsMap& db) : AuxPOS(db)
			{ completePOS("adv", "adverbs"); }
		virtual ~Adverb(){ }
	};
}
