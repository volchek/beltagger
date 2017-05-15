#ifndef TARDEEP_H
#define TARDEEP_H

#include "TarShallow.h"
#include "constants.h"
#include <locale>

namespace tagger {
	class TarDeep : public TarShallow
	{
	public:
		TarDeep();
		TarDeep(std::locale&);
		~TarDeep();
		std::string tweakTar(const std::string&);

	private:
		std::string& replaceLj(std::string&);
		std::string& replaceCons(std::string&);
	};
}
#endif // TARDEEP_H