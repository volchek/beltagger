#include "TarDeep.h"
#include "defines.h"
#include "constants.h"

tagger::TarDeep::TarDeep() 
{
}

tagger::TarDeep::TarDeep(std::locale& inputLocale) :
	TarShallow(inputLocale)
{
}


tagger::TarDeep::~TarDeep()
{
}


std::string tagger::TarDeep::tweakTar(const std::string& wordform)
{
	std::string cleanWordform = TarShallow::tweakTar(wordform);
	// дз => дс
	cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("дз([тк])", bx::regex_constants::icase), "дс$1");
	// ждж => здж
	cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("([ея])ждж", bx::regex_constants::icase), "$1здж");
	// nominal inflection
	cleanWordform = bx::regex_replace(cleanWordform, compiler.compile("([бк])ох$", bx::regex_constants::icase), "$1ах");
	// л' => л
	cleanWordform = replaceLj(cleanWordform);
	// Cons => Cons'
	cleanWordform = replaceCons(cleanWordform);

	return cleanWordform;
}


std::string& tagger::TarDeep::replaceLj(std::string& wordform)
{
	if (bx::regex_search(wordform, compiler.compile("л[ьёюя]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("алькаг([ао])", bx::regex_constants::icase), "алкаг$1");
		wordform = bx::regex_replace(wordform, compiler.compile("бляк(ад|ір|но)", bx::regex_constants::icase), "блак$1");
		wordform = bx::regex_replace(wordform, compiler.compile("клюб", bx::regex_constants::icase), "клуб");
		wordform = bx::regex_replace(wordform, compiler.compile("кляс([аны])", bx::regex_constants::icase), "клас$1");
		wordform = bx::regex_replace(wordform, compiler.compile("лягі([зсч])", bx::regex_constants::icase), "лагі$1");
		wordform = bx::regex_replace(wordform, compiler.compile("лё(гі|ід)", bx::regex_constants::icase), "ло$1");
		wordform = bx::regex_replace(wordform, compiler.compile("люрг", bx::regex_constants::icase), "лург");
		wordform = bx::regex_replace(wordform, compiler.compile("лянд([зс])", bx::regex_constants::icase), "ланд$1");	// BEWARE! "фінляндскі"
		wordform = bx::regex_replace(wordform, compiler.compile("([оё])ляг", bx::regex_constants::icase), "$1лаг");
		wordform = bx::regex_replace(wordform, compiler.compile("пля(ды|зм|кат|мб|ст|цк)", bx::regex_constants::icase), "пла$1");
		wordform = bx::regex_replace(wordform, compiler.compile("плянэт", bx::regex_constants::icase), "планет");	// OUT OF DOMAIN нэ => не CHANGE
		wordform = bx::regex_replace(wordform, compiler.compile("філязоф", bx::regex_constants::icase), "філасоф");	// OUT OF DOMAIN з => с CHANGE
		if (bx::regex_search(wordform, compiler.compile("фля"))){
			wordform = bx::regex_replace(wordform, compiler.compile("фля(г([аеімуш-])|м([ае])|н([гкэ])|р)", bx::regex_constants::icase), "фла$1");
		}
	}
	return wordform;
}


std::string& tagger::TarDeep::replaceCons(std::string& wordform)
{
	if(bx::regex_search(wordform, compiler.compile("[взмнпсф][ыэ]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("([взмнпсф])э([кн])([тц])", bx::regex_constants::icase), "$1е$2$3");	// BEWARE! "эсэнцыя", "квінтэсэнцыя"
		wordform = bx::regex_replace(wordform, compiler.compile("([взмнф])эа", bx::regex_constants::icase), "$1еа");
	}
	// з => з'
	if (bx::regex_search(wordform, compiler.compile("з[ыэ]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("зыгзаг", bx::regex_constants::icase), "зігзаг");
		wordform = bx::regex_replace(wordform, compiler.compile("рэзыд", bx::regex_constants::icase), "рэзід");
		wordform = bx::regex_replace(wordform, compiler.compile("фізы([кч])", bx::regex_constants::icase), "фізі$1");
		wordform = bx::regex_replace(wordform, compiler.compile("зэніт", bx::regex_constants::icase), "зеніт");
	}

	// с => с'
	if (bx::regex_search(wordform, compiler.compile("с[ыэ]"))){
		wordform = bx::regex_replace(wordform, compiler.compile("псых", bx::regex_constants::icase), "псіх");
		wordform = bx::regex_replace(wordform, compiler.compile("сэрв", bx::regex_constants::icase), "серв");	// BEWARE! "сэрвіс", "сэрвісны"
		wordform = bx::regex_replace(wordform, compiler.compile("сын([дт])", bx::regex_constants::icase), "сін$1");
		wordform = bx::regex_replace(wordform, compiler.compile("сыст", bx::regex_constants::icase), "сіст");
		wordform = bx::regex_replace(wordform, compiler.compile("сычн", bx::regex_constants::icase), "січн");
	}

	// б => б'
	if (bx::regex_search(wordform, compiler.compile("бэ"))){
		wordform = bx::regex_replace(wordform, compiler.compile("бэ(гем|летр|н([гз]|эф)|р(бэр|гам|ку[тц]|[нэ])|тон)", bx::regex_constants::icase), "бе$1");
	}

	// п => п'
	if (bx::regex_search(wordform, compiler.compile("пэ"))){
		wordform = bx::regex_replace(wordform, compiler.compile("пэ(д|[ўр]т|тч)", bx::regex_constants::icase), "пе$1");
	}

	// в => в'
	if (bx::regex_search(wordform, compiler.compile("вэ"))){
		wordform = bx::regex_replace(wordform, compiler.compile("вэ(р([гбст])|гет|[нт]эр)", bx::regex_constants::icase), "ве$1");
	}

	// ф => ф'
	if (bx::regex_search(wordform, compiler.compile("фэ"))){
		wordform = bx::regex_replace(wordform, compiler.compile("фэ([дклмрт]|сі)", bx::regex_constants::icase), "фе$1");	// BEWARE! "куфэрак", "Фэлька"
	}

	// м => м'
	if (bx::regex_search(wordform, compiler.compile("мэ"))){
		wordform = bx::regex_replace(wordform, compiler.compile("мэ([гэ]а|д[^зл])", bx::regex_constants::icase), "ме$1");
		wordform = bx::regex_replace(wordform, compiler.compile("мэлёд", bx::regex_constants::icase), "мелод");	// OUT OF DOMAIN лё => ло CHANGE
		wordform = bx::regex_replace(wordform, compiler.compile("мэлядр", bx::regex_constants::icase), "меладр");	// OUT OF DOMAIN ля => ла CHANGE
		wordform = bx::regex_replace(wordform, compiler.compile("(\\w)мэтр", bx::regex_constants::icase), "$1метр");
		wordform = bx::regex_replace(wordform, compiler.compile("мэтал", bx::regex_constants::icase), "метал");
		wordform = bx::regex_replace(wordform, compiler.compile("экзамэн", bx::regex_constants::icase), "экзамен");
		wordform = bx::regex_replace(wordform, compiler.compile("камэр", bx::regex_constants::icase), "камер");
		wordform = bx::regex_replace(wordform, compiler.compile("амэрык", bx::regex_constants::icase), "амерык");
	}

	// н => н'
	if (bx::regex_search(wordform, compiler.compile("нэ"))){
		wordform = bx::regex_replace(wordform, compiler.compile("нэ([гкў][рм]|рг)", bx::regex_constants::icase), "не$1");
		wordform = bx::regex_replace(wordform, compiler.compile("([вг])енэр", bx::regex_constants::icase), "$1енер");
		wordform = bx::regex_replace(wordform, compiler.compile("нэўтр", bx::regex_constants::icase), "нейтр");	// OUT OF DOMAIN ў => й CHANGE
	}
	return wordform;
}