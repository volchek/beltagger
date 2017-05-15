#pragma once

#include "DBTypedefs.h"


namespace assembler {
	class POS
	{
	public:
		POS(wordsToLemsMap& db);
		virtual ~POS();
		static void setPathPrefix(std::string);
	protected:
		wordsToLemsMap& database;
		void readFile(std::string fileName);
		std::string unfold(std::string& tag, const std::string& posLabel);
		std::string normalizeTagset(std::string& inputString);
		virtual void completePOS(std::string posShort, std::string posFull);
		virtual void completeInnerPOS(std::string posShort, std::string posFull);
	private:
		virtual void parseExplicit(std::string& dataLine) = 0;

		static std::string pathPrefix;
		static const std::string defaultPathPrefix;
		static const std::string pathPostfix;
	};
}
