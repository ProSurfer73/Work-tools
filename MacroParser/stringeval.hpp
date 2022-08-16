#ifndef STRINGEVAL_HPP
#define STRINGEVAL_HPP






#include <vector>
#include <string>
#include <sstream>
#include <regex>
#include <cassert>

#include "config.hpp"
#include "hexa.hpp"

using namespace std;

void clearSpaces(string& str);

double evaluateArithmeticExpr(const std::string& expr);

bool calculateExpression(string& expr, const vector< pair<string,string> >& dictionary, const vector<string>& redefinedMacros);


#endif // STRINGEVAL_HPP
