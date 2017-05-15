#include "functions.h"
#include "defines.h"

#include <iostream>
#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <locale>

/***********************
  Auxiliary functions
***********************/

std::locale loc(LOCNAME);

const std::string accessError(const std::string fileName)
{
	return "No " + fileName + " found. Make sure you haven't removed it accidentally.";
}

const std::string prependFileName(std::string filehandle, std::string prefix)
{
	const int filehandleLength = filehandle.length();
	if(filehandle[filehandleLength-1] == '/' || filehandle[filehandleLength-1] == '\\'){
		filehandle.pop_back();
	}

	std::vector<std::string> pathElem;
	std::string tmp = filehandle;
	boost::split(pathElem, filehandle, boost::is_any_of("\\//"));
	const size_t vecSize = pathElem.size();
	if (vecSize < 1) {
		std::cerr << "Invalid path '" << filehandle << "' in prependFileName. The program will now exit." << std::endl;
		exit(1);
	}
	pathElem[vecSize-1] = (prefix.append(pathElem[vecSize-1]));
	filehandle = "";
	for (std::vector<std::string>::const_iterator it = pathElem.begin(); it != pathElem.end(); ++it){
		filehandle += *it;
		filehandle += '/';
	}
	filehandle.pop_back();
	return filehandle;
}

std::string toLowerStr(const std::string& str){
	return boost::to_lower_copy(str, loc);
}


const std::set<std::string> intersect(std::vector<std::string>& firstVec, std::vector<std::string>& secondVec)
{
	std::set<std::string> oneVecElems;
	for (std::vector<std::string>::const_iterator it = firstVec.begin(); it != firstVec.end(); ++it){
		oneVecElems.insert(*it);
	}
	std::set<std::string> vecIntersect;
	for (std::vector<std::string>::const_iterator it = secondVec.begin(); it != secondVec.end(); ++it){
		if (oneVecElems.find(*it) != oneVecElems.end()){
			vecIntersect.insert(*it);
		}
	}
	return vecIntersect;
}

bool sortLenCondition(std::string& str1, std::string& str2)
{
	return str1.length() > str2.length();
}

int sumMapValues(const std::map<std::string, int>& m)
{
	int sum = 0;
	for (std::map<std::string, int>::const_iterator it = m.begin(); it != m.end(); ++it){
		sum += it->second;
	}
	return sum;
}

const std::pair<std::string, std::string> conjoinParts(tagger::wordInfo& map)
{
	std::pair<std::string, std::string> res;
	std::string lemmas, tags;
	for (tagger::wordInfo::const_iterator it = map.begin(); it != map.end(); ++it){
		lemmas += it->first;
		lemmas += '|';
		for (std::set<std::string>::const_iterator inIter = it->second.begin() ; inIter != it->second.end(); ++inIter){
			tags += *inIter;
			tags += '|';
		}
		tags += '|';
	}
	lemmas.pop_back();
	tags.pop_back(); tags.pop_back();
	return std::make_pair(lemmas, tags);
}

std::vector<std::vector<std::string> > getCartesianProduct(std::vector<std::vector<std::string> >& sets)
{
	std::vector<std::vector<std::string> > cartProd;
	for (std::vector<std::vector<std::string> >::const_iterator it = sets.begin(); it != sets.end(); ++it){
		if (cartProd.empty()){
			for (std::vector<std::string>::const_iterator inIt = it->begin(); inIt != it->end(); ++inIt){
				std::vector<std::string> currentProd;
				currentProd.push_back(*inIt);
				cartProd.push_back(currentProd);
			}
		}
		else {
			if (it->size() == 1){
				std::vector<std::string>::const_iterator strIt = it->begin();
				for (std::vector<std::vector<std::string> >::iterator prodIt = cartProd.begin(); prodIt != cartProd.end(); ++prodIt){
					prodIt->push_back(*strIt);
				}
			}
			else {
				size_t prodSize = cartProd.size();
				size_t vecSize = it->size();
				cartProd.resize(prodSize * vecSize);
				for (size_t i = 1; i < vecSize; ++i){
					for (size_t j = 0; j < prodSize; ++j){
						cartProd[i * prodSize + j] = cartProd[j];
					}
				}

				for (size_t i = 0; i < it->size(); ++i){
					for (size_t j = 0; j < prodSize; ++j){
						std::vector<std::string>::const_iterator vecIt = it->begin() + i;
						cartProd[i * prodSize + j].push_back(*vecIt);
					}
				}
			}
		}
	}
	return cartProd;
}
