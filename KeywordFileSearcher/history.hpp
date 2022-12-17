#ifndef HISTORY_HPP
#define HISTORY_HPP

#include <fstream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm>
#include <iostream>

#define HISTORY_FILEPATH "history.txt"
#define MAX_HISTORY_ENTRY 5

class History
{
public:
    // Automatically loads the file
    History();

    // Load and save history
    bool loadFromFile(const std::string& str);
    bool saveToFile(const std::string& str);

    // Access and modify history
    std::deque<std::string>* getTableFrom(const std::string& str);
    void pushHistory(const std::string& name, const std::string& value);

    // High-level functions related to console application
    void showPossibilities(const std::string& name);
    bool tryPossibilities(std::string& userInput, const std::string& name);
    void describeContent();

private:
    std::vector< std::pair< std::string, std::deque<std::string> > > tables;
};



#endif // HISTORY_HPP
