#include <map>
#include <set>
#include <vector>
#include <string>

namespace assembler {
	typedef std::map<std::string, int> gramsToIntMap;
	typedef std::map<std::string, gramsToIntMap> lemToGramsMap;
	typedef std::map<std::string, lemToGramsMap> wordsToLemsMap;

	typedef std::vector<std::string> vecStr;
	typedef std::vector<std::string>::iterator vecStrIter;
}