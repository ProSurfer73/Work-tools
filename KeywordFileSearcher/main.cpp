#include <iostream>

#include "filesystem.hpp"

using namespace std;


void addNumberToStr(char* str, unsigned number)
{
    char strnum[15];

    if(number < 10)
        snprintf(strnum, 15, "000%u", number);
    else if(number < 100)
        snprintf(strnum, 15, "00%u", number);
    else if(number < 1000)
        snprintf(strnum, 15, "0%u", number);
    else
        snprintf(strnum, 15, "%u", number);

    strcat(str, strnum);
}


/** \brief Look and list the files containing the keyword
 *
 * \param the list of the files to be looked at
 * \param the list of keywords to be searched
 * \param the output stream where the path to the found files will be printed
 * \return the number of files having at least one occurences
 *
 */
unsigned searchKeywords(const stringvec& fileCollection, const stringvec& keywords, std::ostream& output)
{
    unsigned nbOccurences=0;

    for(const string& str1: fileCollection)
    {
        if(readFile(str1, keywords)){
            output << str1 << endl;
            nbOccurences++;
        }
    }

    return nbOccurences;
}


/** \brief Replace keywords (can add occurence number)
 *
 * \param
 * \param
 * \return
 *
 */

unsigned replaceKeyword(const stringvec& fileCollection, const string& keywordSearched, const char* keywordReplaced, std::ostream& output, bool addNumberAtTheEnd)
{
    unsigned nbOccurences=0;

    char real[50];

    strncpy(real, keywordReplaced, 50);

    for(const string& str1: fileCollection)
    {
        if(addNumberAtTheEnd){
            // Copy the initial string
            strncpy(real, keywordReplaced, 50);

            // Add the number string to it all
            addNumberToStr(real, nbOccurences);
        }


        if(replaceKeywordFile(str1, keywordSearched, real)){
            output << str1 << endl;
            nbOccurences++;
        }
    }

    return nbOccurences;
}


bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}



int main()
{
    std::cout << "WELCOME TO KEYWORD SEARCHER" << endl;
    std::cout << "List all the files containing a specified string inside a specified folder.\n" << endl;


    std::cout << "Please type the directory:" << endl;

    string dir;
    getline(cin, dir);

    if(dir.empty())
        return 0;

    stringvec keywords;

    string input;

    do
    {
        std::cout << "Please type a string you would like to search:" << endl;
        getline(cin, input);

        if(!input.empty())
            keywords.emplace_back(input);
    }
    while(!input.empty());


    stringvec fileCollection;

    explore_directory(dir, fileCollection);

    stringvec extensionsToKeep;

    do
    {
        cout << "please type extension (or nothing to continue): ";
        getline(cin, input);

        if(!input.empty())
            extensionsToKeep.emplace_back(input);
    }
    while(!input.empty());


    if(!extensionsToKeep.empty())
    {
        for(auto it = fileCollection.begin(); it!=fileCollection.end();)
        {
            // Is the extension of the file among the list of extensions to keep
            bool shouldKeep=false;
            for(string& curExt : extensionsToKeep)
            {
                if(hasEnding(*it, curExt)){
                    shouldKeep = true;
                    break;
                }
            }

            // Erase the filepath
            if(!shouldKeep)
                it = fileCollection.erase(it);
            else
                ++it;
        }
    }


    std::ofstream resultFile("C:\\Users\\divouxje\\Desktop\\output.txt");

    reask:

    cout << "Type 1 to search and 2 to replace:" << endl;

    getline(cin, input);

    unsigned nb=0;

    // search mode
    if(input == "1")
    {
         nb = searchKeywords(fileCollection, keywords, resultFile);
    }

    // replace mode
    else if(input == "2")
    {
        cout << "Type the string to replace:" << endl;
        string str2;
        getline(cin, str2);

        cout << "add localising number to it ? (type 'y' if okay)" << endl;
        getline(cin, input);


         nb = replaceKeyword(fileCollection, keywords.front(), str2.c_str(), resultFile, (input[0]=='y'));
    }

    // Incorrect input
    else
    {
        goto reask;
    }





    std::cout << nb << " results found." << std::endl;


    return 0;
}
