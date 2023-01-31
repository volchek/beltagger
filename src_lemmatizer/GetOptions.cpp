#include "GetOptions.h"
#include "functions.h"
#include "LemMemo.h"
#include "LemNomemo.h"
#include "LemWithGuessing.h"

#include <iostream>
#include <fstream>
#include <boost/program_options.hpp>
#include <memory>

tagger::GetOptions::GetOptions()
{
	opts.lastColumnIndex = 2;
	opts.logUnk = false;
	opts.memo = true;
	opts.tar = "sh";
	opts.guessing = false;
	opts.suffixTable = "none";
	// Set default values
//	opts.logGuessed = false;
}

tagger::GetOptions::~GetOptions()
{
}

void tagger::GetOptions::getOpt(int argc, char** argv)
{
	namespace po = boost::program_options;
	po::options_description desc("Options");
	desc.add_options()
		("input-file,i", po::value<std::string>(), "input file")
		("output-file,o", po::value<std::string>(), "output file")
		("unknowns-file,u", po::value<std::string>(), "unknown tokens logging")
		("columns,c", po::value<int>(), "number of columns")
		("memo,m", po::value<std::string>(), "lemma and tag memoization")
		("tar,t", po::value<std::string>(), "tarashkevitsa handling")
		("suf,s", po::value<std::string>(), "suffix table file")
		("guessing,g", po::value<std::string>(), "guessed lemmas and tag logging");
	po::variables_map vm;
	try {
		po::store(po::parse_command_line(argc, argv, desc), vm);
		po::notify(vm);

		// -i: Input file
		if (vm.count("input-file")){ 
			opts.inputFile =  vm["input-file"].as<std::string>(); 
		}
		else { 
			std::cerr << "No input file supplied" << std::endl;
			exit(1);
		}
		std::ifstream filein(opts.inputFile);
		if (!filein.is_open()){
			std::cerr << "Invalid input file" << std::endl;
			exit(1);
		}
		filein.close();


		// -o: Output file ("tagged_" + input file name by default)
		if (vm.count("output-file")){
			opts.outputFile = vm["output-file"].as<std::string>();
		}
		else { 
			opts.outputFile = prependFileName(opts.inputFile, "tagged_");
		}

		// -u: Unknown tokens logging
		/* Without -u flag, the logging is off. With "-u default", unknown tokens 
		and their frequencies will be logged into the default file ("unknown_" + input file name).
		Otherwise, the parameter will be understood as a path to log file. */
		if (vm.count("unknowns-file")){
			opts.logUnk = true;
			if (vm["unknowns-file"].as<std::string>() == "default"){
				opts.unknownsFile = prependFileName(opts.inputFile, "unknown_");
			}
			else { opts.unknownsFile = vm["unknowns-file"].as<std::string>(); }
		}
		
		// -c: Number of columns (3 by default)
		if (vm.count("columns")){
			if (vm["columns"].as<int>() > 0) {
				opts.lastColumnIndex = vm["columns"].as<int>() - 1;
			}
			else {
				std::cerr << "Invalid -c flag (positive integer expected)" << std::endl;
				exit(1);
			}
		}

		// -m: Lemma and tag memoization (on by default)
		if (vm.count("memo")){
			if (vm["memo"].as<std::string>() == "on"){
				opts.memo = true;
			}
			else if (vm["memo"].as<std::string>() == "off"){
				opts.memo = false;
			}
			else {
				std::cerr << "Invalid -m flag('on' or 'off' expected)" << std::endl;
				exit(1);
			}
		}

		// -t: Tarashkevitsa handling (shallow by default)
		if (vm.count("tar")){
			if (vm["tar"].as<std::string>() == "shallow"){
				opts.tar = "sh";
			}
			else if (vm["tar"].as<std::string>() == "deep"){
				opts.tar = "d";
			}
			else {
				std::cerr << "Invalid -t flag('shallow' or 'deep' expected)" << std::endl;
				exit(1);
			}
		}

		// -s: Suffix table for lemma and tag guessing (none by default)
		if (vm.count("suf")){
			opts.suffixTable = vm["suf"].as<std::string>();
			opts.guessing = true;
		}

		// -g: Guessed lemmas and tags logging
		/* Without -g flag, the logging is off. There is also nothing to log 
		when the suffix table insn't specified in -s. Otherwise, with "-g default",
		tokens with guesse lemmas and tags will be logged into default file ("guessed_" + 
		input file name). Yet otherwise, the parameter will be understood as a path to log file.
		*/
		if (vm.count("guessing")){
			if (opts.suffixTable == "none"){ std::cerr << "Useless -g flag (no suffix table specified)" << std::endl; }
			else {
//				opts.logGuessed = true;
				if (vm["guessing"].as<std::string>() == "default"){
					opts.guessedFile = prependFileName(opts.inputFile, "guessed_");
				}
				else {
					opts.guessedFile = vm["guessing"].as<std::string>();
				}
			}
		}
				
		createLem();
	}
	catch (std::exception& e){
		std::cerr << e.what() << std::endl;
		exit(1);
	}
}

void tagger::GetOptions::createLem()
{
	if (opts.memo == true){
		if (opts.guessing){
			std::shared_ptr<tagger::LemWithGuessing<tagger::LemMemo> > l(new tagger::LemWithGuessing<tagger::LemMemo>(opts.inputFile, opts.outputFile, opts.lastColumnIndex, opts.logUnk, opts.unknownsFile, opts.tar, opts.suffixTable, opts.guessedFile));
		}
		else {
			std::shared_ptr<tagger::LemMemo> l(new tagger::LemMemo(opts.inputFile, opts.outputFile, opts.lastColumnIndex, opts.logUnk, opts.unknownsFile, opts.tar));
		}
	}
	else {
		if (opts.guessing){
			std::shared_ptr<tagger::LemWithGuessing<tagger::LemNomemo> > l(new tagger::LemWithGuessing<tagger::LemNomemo>(opts.inputFile, opts.outputFile, opts.lastColumnIndex, opts.logUnk, opts.unknownsFile, opts.tar, opts.suffixTable, opts.guessedFile));
		}
		else {
			std::shared_ptr<tagger::LemNomemo> l(new tagger::LemNomemo(opts.inputFile, opts.outputFile, opts.lastColumnIndex, opts.logUnk, opts.unknownsFile, opts.tar));
		}
	}
}
