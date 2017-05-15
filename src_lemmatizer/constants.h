#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <string>
#include <boost/xpressive/xpressive.hpp>

namespace tagger {
	namespace bx = boost::xpressive;

	// Postfixes
	const std::string compEqPostfix = "[COMPeq]";
	const std::string compAdjPostfix = "[COMPadj]";
	const std::string compPartCountPostfix = "[COMP#eq]";
	const std::string orthPostfix = "[ORTH]";
	const std::string tarPostfix = "[TAR]";
	const std::string hyphenPostfix = "[HYPH]";
	const std::string guessPostfix = "[GUESS]";

	// Lemmatizer
	const size_t bufSize = 1024;
	const size_t reportingStep = 10000;

}

#endif // CONSTANTS_H