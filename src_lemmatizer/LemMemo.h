#ifndef LEMMEMO_H
#define LEMMEMO_H

#include "Lemmatizer.h"

namespace tagger {

	class LemMemo : public Lemmatizer
	{
	public:
		LemMemo(std::string&, std::string&, int, bool, std::string&, std::string);
		LemMemo(std::string&, std::string&, int, bool, std::string&, std::string, bool);
		virtual ~LemMemo();

	protected:
		std::string plainOutput(const wordInfo&, const std::string&, const std::string&);
		bool processWordform(std::string&, std::string&);

	private:
		std::unordered_map<std::string, std::string> lSeen;
		void processOne(const std::string&, const std::string&, const std::string&, const std::string&);
		bool processMemo(std::string&, std::string&);
	};
}

#endif // LEMMEMO_H
