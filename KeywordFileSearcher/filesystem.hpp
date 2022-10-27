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


using namespace std;

typedef std::vector<std::string> stringvec;

// Fucntions available

bool directoryExists(const char* basepath);

bool readFile(const string& path, const stringvec& words);

bool readDirectory(string dir, vector< pair<string, string> >& defineList, stringvec& redefinedMacros);

void explore_directory(const std::string& dirname, std::vector<std::string>& files);

bool replaceKeywordFile(const string& path, const string& initialKeyword, const string& finalKeyword);

bool readFileWithLine(std::string& path, stringvec& results, const stringvec& words, std::ostream& output);

#endif // FILESYSTEM_HPP

