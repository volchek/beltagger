#pragma once

#include "POS.h"

namespace assembler {
	class POS_Template : public POS
	{
	public:
		POS_Template(wordsToLemsMap& db);
		~POS_Template();
		std::vector<std::string>& getParadigmAdjAndPart(std::string par);
		std::vector<std::string>& getParadigm(std::string par);
		std::vector<std::string>& getInflection(std::string par);
		void parseTemplate(std::string pos);
		void clearParadigm(std::string pos);
		void clearInflection();

	private:
		void parseExplicit(std::string& pos);
		void parseAdjAndPartTemplatePlugin(std::string& dataLine);
		void parseVerbTemplatePlugin(std::string& dataLine);
		void parseNounTemplatePlugin(std::string& dataLine);

		std::string templatePOS;
		std::map<std::string, std::vector<std::string> > paradigm;
		std::map<std::string, std::vector<std::string> > paradigmAdjAndPart;
		std::map<std::string, std::vector<std::string> > inflection;
	};
}
