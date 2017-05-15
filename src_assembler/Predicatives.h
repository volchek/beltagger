#pragma once

#include "AuxPOS.h"

namespace assembler {
	class Predicatives : public AuxPOS
	{
	public:
		Predicatives(wordsToLemsMap& db) : AuxPOS(db)
			{ completePOS("predic", "predicatives"); }
		virtual ~Predicatives(){ }
	};
}
