#ifndef FILESYSTEM_HPP
#define FILESYSTEM_HPP

#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <windows.h>
#include <iostream>
#include <thread>
#include <mutex>

#include "stringeval.hpp"
#include "config.hpp"

using namespace std;

typedef std::vector<std::string> stringvec;

// Fucntions available

BOOL DirectoryExists(LPCTSTR szPath);

bool readFile(const string& pathToFile, std::vector< pair<string,string> >& defineList, stringvec& redefinedMacros);

bool readDirectory(string dir, vector< pair<string, string> >& defineList, stringvec& redefinedMacros);

#endif // FILESYSTEM_HPP
