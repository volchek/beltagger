#pragma once

#include "NumAndPron.h"

namespace assembler {
	class Numeral : public NumAndPron
	{
	public:
		Numeral(wordsToLemsMap& db) : NumAndPron(db)
			{ completePOS("numcardcoll", "cardinal and collective numerals"); }
		virtual ~Numeral(){ }
	};
}