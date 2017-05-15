#ifndef TARSHALLOW_H
#define TARSHALLOW_H

#include "constants.h"
#include <locale>
#include <string>
#include <map>

namespace tagger {
	class TarShallow
	{
	public:
		TarShallow();
		TarShallow(std::locale&);
		virtual ~TarShallow();
		virtual std::string tweakTar(const std::string&);
	protected:
		bx::sregex_compiler compiler;
	private:
		std::locale loc;
		std::map<std::string, std::string> simpleTar;
		void completeSimpleTar();
	};
}
#endif // TARSHALLOW_H