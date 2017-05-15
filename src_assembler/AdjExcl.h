#pragma once

#include "AuxPOS.h"

namespace assembler {
	class AdjExcl :	public AuxPOS
	{
	public:
		AdjExcl(wordsToLemsMap& db) : AuxPOS(db)
			{ completeInnerPOS("adj_exclusions", "non-inflected comparatives"); }
		virtual ~AdjExcl(){}
	};
}
