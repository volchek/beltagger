#pragma once

#include "NumAndPron.h"

namespace assembler {
	class Pronoun : public NumAndPron
	{
	public:
		Pronoun(wordsToLemsMap& db) : NumAndPron(db)
			{ completePOS("pron", "pronouns"); }
		virtual ~Pronoun(){ }
	};
}