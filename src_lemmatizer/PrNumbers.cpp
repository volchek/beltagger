#include "PrNumbers.h"
#include "defines.h"


tagger::PrNumbers::PrNumbers() :
	loc(std::locale(LOCNAME))
{
	startPrNumbers();
}


tagger::PrNumbers::PrNumbers(std::locale& inputLocale) :
	loc(inputLocale)
{
	startPrNumbers();
}


tagger::PrNumbers::~PrNumbers()
{
}

void tagger::PrNumbers::startPrNumbers()
{
	compiler.imbue(loc);
	completeNumTemplates();
	completeNumSubst();
}


void tagger::PrNumbers::completeNumTemplates()
{
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d+((\\.|,)\\d+)?$"), "Number"));
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d+:\\d+$"), "Score|Time"));
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d{2}:\\d{2}:\\d{2}$"), "Time"));
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d{2}\\.\\d{2}\\.\\d{2}(\\d{2})?$"), "Date"));
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d{1,3}[-Ц]\\d{1,3}$"), "Span"));
	numTemplates.push_back(std::make_pair(compiler.compile("^[12]\\d{3}[-Ц]([12]\\d)?\\d{2}$"), "Timespan"));
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d{1,4}\\.$"), "ListItem"));
	numTemplates.push_back(std::make_pair(compiler.compile("^(\\d{2,3}|\\d{1}-\\d{3}-\\d{3}|\\d{1}-\\d{4}-\\d{2})(-\\d{2}){2}$"), "Phone"));
	numTemplates.push_back(std::make_pair(compiler.compile("^є\\.\\d+$"), "IssueID"));
	numTemplates.push_back(std::make_pair(compiler.compile("^є\\d+$"), "OrdNumber"));
	numTemplates.push_back(std::make_pair(compiler.compile("^\\d{1,4}x\\d{1,4}$"), "Size"));
	numTemplates.push_back(std::make_pair(compiler.compile("^1:\\d{2,10}$"), "Scope"));
}

void tagger::PrNumbers::completeNumSubst()
{
	numSubst["е"] = "NumOrd:nsN|NumOrd:nsA";
	numSubst["й"] = "NumOrd:fsG|NumOrd:fsD|NumOrd:fsI|NumOrd:fsL";
	numSubst["м"] = "NumOrd:msI|NumOrd:msL|NumOrd:nsI|NumOrd:nsL|NumOrd:0pD";
	numSubst["х"] = "NumOrd:0pG|NumOrd:0pA|NumOrd:0pL";
	numSubst["ы"] = "NumOrd:msN|NumOrd:msA";
	numSubst["ю"] = "NumOrd:fsA";
	numSubst["€"] = "NumOrd:0pN|NumOrd:0pA|NumOrd:fsN";
	numSubst["га"] = "NumOrd:msG|NumOrd:nsG";
	numSubst["му"] = "NumOrd:msD|NumOrd:nsD";
	numSubst["ым"] = "NumOrd:msI|NumOrd:nsI|NumOrd:0pD";
	numSubst["≥м"] = "NumOrd:msI|NumOrd:nsI|NumOrd:0pD";
	numSubst["ай"] = "NumOrd:fsG|NumOrd:fsD|NumOrd:fsI|NumOrd:fsL";
}


std::string tagger::PrNumbers::checkNumber(const std::string& wordform)
{
	for(std::vector<std::pair<bx::sregex, std::string> >::iterator it = numTemplates.begin(); it != numTemplates.end(); ++it){
		if(bx::regex_search(wordform, it->first)){
			return it->second;
		}
	}
	return "";
}


std::pair<std::string, std::string> tagger::PrNumbers::checkMixedNumber(const std::string& wordform)
{
	bx::smatch m;
	if (bx::regex_search(wordform, m, compiler.compile("^(\\d{1,4})\\-([еймхыю€]|га|му|ым|ай)$"))){
		std::pair<std::string, std::string> res = std::make_pair(m[1] + "-ы", numSubst[m[2]]);
		return res;
	}
	else {
		return std::make_pair("", "");
	}
}


std::string tagger::PrNumbers::checkRomanNumber(std::string& wordform)
{
	if (wordform.length() > maxLenRNumber){
		return "";
	}

	if(bx::regex_search(wordform, compiler.compile("^[IVXLC]+(-[IVXLC]+)*$"))){
		if (wordform.find("-") != wordform.npos){
			return "RTimespan";
		}
		else {
			return "RNumber";
		}
	}
	return "";
}