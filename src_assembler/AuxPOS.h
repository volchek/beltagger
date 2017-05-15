#ifndef AUXPOS_H
#define AUXPOS_H

#include "POS.h"
#include <string>

namespace assembler {
	class AuxPOS : public POS
	{
	public:
		AuxPOS(wordsToLemsMap& db);
		virtual ~AuxPOS();

	private:
		void parseExplicit(std::string& dataLine);
	};
}

#endif // AUXPOS_H
