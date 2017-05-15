#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "taggerTypes.h"
#include <string>
#include <vector>
#include <map>
#include <set>

const std::string accessError(const std::string);
const std::string prependFileName(std::string, std::string);
std::string toLowerStr(const std::string&);

const std::set<std::string> intersect(std::vector<std::string>&, std::vector<std::string>&);
bool sortLenCondition(std::string&, std::string&);
int sumMapValues(const std::map<std::string, int>&);
const std::pair<std::string, std::string> conjoinParts(tagger::wordInfo& map);
std::vector<std::vector<std::string> > getCartesianProduct(std::vector<std::vector<std::string> >&);

#endif // FUNCTIONS_H