#ifndef GETOPTIONS_H
#define GETOPTIONS_H

#include "taggerTypes.h"

namespace tagger {
	
	class GetOptions
	{
	public:
		GetOptions();
		~GetOptions();
		void getOpt(int, char**);
	
	private:
		optionValues opts;
		void createLem();
	};
}
#endif // GETOPTIONS_H