#pragma once

#include "AuxPOS.h"

namespace assembler {
	class CommonAbbr : public AuxPOS
	{
	public:
		CommonAbbr(wordsToLemsMap& db) : AuxPOS(db)
			{ completePOS("abbr", "common abbreviations"); }
		virtual ~CommonAbbr(){ }
	};
}
