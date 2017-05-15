#ifndef LEMWITHGUESSING_H
#define LEMWITHGUESSING_H
#include "Guessing.h"
#include "constants.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>

namespace tagger {
	
	template <class baseLem>
	class LemWithGuessing :	public Guessing, public baseLem
	{
	public:
			LemWithGuessing(std::string& inputFile, std::string& outputFile, int lastIndex, bool logUnknowns, std::string& unkFile, std::string tar, std::string sufTable, std::string logGuessing = "") :
			Guessing(sufTable, logGuessing),
			baseLem(inputFile, outputFile, lastIndex, logUnknowns, unkFile, tar, true),
			guessedFile(logGuessing)
			{
				startLem();
			}
		
		~LemWithGuessing(){
		};

	private:
		std::map<std::string, int> guessed;
		std::string guessedFile;

		void startLem(){
			Guessing::startGuessing();
			baseLem::startLem();
		}

		bool processWordform(std::string& inputLine, std::string& wordform){
			if (baseLem::processWordform(inputLine, wordform)){
				return true;
			}

			wordInfo data;
			if (this->guessTag(wordform, data)){
				std::string res = baseLem::plainOutput(data, inputLine, guessPostfix);
				this->printLine(res);
				++guessed[res];
				return true;
			}
	
			return false;
		}
		
		void printLogInfo(){
			baseLem::printLogInfo();
			printGuessed();
		}

		void printGuessed(){
			if (guessedFile.empty()){
				return;
			}
			std::ofstream fileout(guessedFile);
			if (!fileout.is_open()){
				std::cerr << "Invalid file for unknown tokens" << std::endl;
				return;
			}
	
			for (std::map<std::string, int>::const_iterator it = guessed.begin(); it != guessed.end(); ++it){
				fileout << it->first << "\n";
			}
			fileout.close();
		}
	};
}
#endif // LEMWITHGUESSING_H