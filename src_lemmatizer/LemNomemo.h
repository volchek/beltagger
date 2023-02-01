#ifndef LEMMATIZERNOMEMO_H
#define LEMMATIZERNOMEMO_H

#include "Lemmatizer.h"

namespace tagger {
	class LemNomemo : public Lemmatizer
	{
	public:
		LemNomemo(std::string&, std::string&, int, bool, std::string&, std::string);
		LemNomemo(std::string&, std::string&, int, bool, std::string&, std::string, bool);
		virtual ~LemNomemo();

	protected:
		virtual bool processWordform(std::string&, std::string&);
		std::string plainOutput(const wordInfo&, const std::string&, const std::string&);

	private:
		void processOne(const std::string&, const std::string&, const std::string&, const std::string&);
	};
}
#endif // LEMMATIZERNOMEMO_H
