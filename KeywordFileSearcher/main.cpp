#include <iostream>
#if (defined(_WIN32) || defined(_WIN64) )
    #include <windows.h>
#endif

#include "filesystem.hpp"
#include "closestr.hpp"
#include "history.hpp"

using namespace std;


void addNumberToStr(std::string& str, unsigned number)
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

    str += strnum;
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
        if(keywords.empty() || readFile(str1, keywords)){
            output << str1 << endl;
            nbOccurences++;
        }
    }

    return nbOccurences;
}


unsigned searchKeywordsWithLines(stringvec& fileCollection, stringvec& results, const stringvec& keywords, std::ostream& output)
{
    unsigned nbOccurences=0;

    for(string& str1: fileCollection)
    {
        if(keywords.empty() || readFileWithLine(str1, results, keywords, output)){
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

    std::string real(keywordReplaced);

    for(const string& str1: fileCollection)
    {
        if(addNumberAtTheEnd){
            // Copy the initial string
            real = keywordReplaced;

            // Add the number string to it all
            addNumberToStr(real, nbOccurences);
        }


        if(readFile(str1, {keywordSearched})
        && replaceKeywordFile(str1, keywordSearched, real)){
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


void filterFilepathByEnding(std::vector<std::string>& fileCollection, std::vector<std::string>& extensions)
{
    // Let's check waht type of list it is
    bool listOfExtensionsToKeep = true;
    for(string& ext : extensions)
    {
        if(ext.front()=='$'){
            listOfExtensionsToKeep = false;
            ext = ext.substr(1);
        }
    }

    if(listOfExtensionsToKeep)
    {

    // Case 1: list of extensions to keep
    for(auto it = fileCollection.begin(); it!=fileCollection.end();)
    {
        // Is the extension of the file among the list of extensions to keep
        bool shouldKeep=false;
        for(const string& curExt : extensions)
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
    else
    {

    // Case 2: list of extensions to avoid

    for(auto it = fileCollection.begin(); it!=fileCollection.end();)
    {
        // Is the extension of the file among the list of extensions to keep
        bool shouldKeep=true;

        for(string& ext : extensions)
        {
            if(hasEnding(*it, ext)){
                shouldKeep = false;
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
}

#include <windows.h>

bool launchProgram(History& history);

int main()
{
    std::cout << "WELCOME TO KEYWORD SEARCHER" << endl;
    std::cout << "List files containing potentially string, with potential extension, inside a specified folder.\n" << endl;

    History history;

    try
    {
        // Let's launch and restart the program as long as the user wants to
        while(launchProgram(history));
    }
    catch(const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
        std::string tmp;
        getline(cin, tmp);
    }


    return 0;
}

bool launchProgram(History& history)
{
    history.showPossibilities("directories");

    reaskDirectory:

    std::cout << "Please type the directory: ";

    string input;

    getline(cin, input);

    if(input.empty())
        return false;

    history.tryPossibilities(input, "directories");

    if(!directoryExists(input.c_str())) {
        std::cout << "/!\\ The directory you entered does not seem to exist, please retry. /!\\\n" << std::endl;
        goto reaskDirectory;
    }

    stringvec fileCollection;

    std::thread thread(explore_directory, input, std::ref(fileCollection) );




    stringvec keywords;


    std::cout << std::endl;
    history.showPossibilities("search");

    bool savedDirectory=false;

    do
    {
        std::cout << "Please type a string you would like to search: ";

        if(!input.empty())
        {
            if(savedDirectory)
            {
                history.tryPossibilities(input, "search");
                keywords.emplace_back(input);
                history.pushHistory("search", input);
            }
            else
            {
                history.pushHistory("directories", input);
                savedDirectory=true;
            }
        }

        getline(cin, input);
    }
    while(!input.empty());

    stringvec extensionsToKeep;

    std::cout << std::endl;

    input.clear();

    history.showPossibilities("extension");

    do
    {
        cout << "Please type extension (or nothing to continue): ";

        if(!input.empty())
        {
            history.tryPossibilities(input, "extension");
            extensionsToKeep.emplace_back(input);
            history.pushHistory("extension", input);
        }

        getline(cin, input);
    }
    while(!input.empty());




    if(!extensionsToKeep.empty())
    {
        thread.join();
        std::cout << "size: " << fileCollection.size() << std::endl;
        thread = std::thread(filterFilepathByEnding, std::ref(fileCollection), std::ref(extensionsToKeep));
    }


    //std::ofstream resultFile("C:\\Users\\divouxje\\Desktop\\output.txt");

    reask:

    cout << "\nType 1 to search and 2 to replace (+: print involved lines):" << endl;

    getline(cin, input);

    unsigned nb=0;

    stringvec results;

    // search mode
    if(input == "1")
    {
        thread.join();
        nb = searchKeywords(fileCollection, keywords, cout);
    }
    else if(input == "1+")
    {
        thread.join();

        if(keywords.empty())
            nb = searchKeywords(fileCollection, keywords, cout);
        else
            nb = searchKeywordsWithLines(fileCollection, results, keywords, cout);
    }

    // replace mode
    else if(input == "2")
    {
        history.showPossibilities("replace");

        cout << "Type the string to replace:" << endl;
        string str2;
        getline(cin, str2);

        history.tryPossibilities(str2, "replace");

        cout << "add localising number to it ? (type 'y' if okay)" << endl;
        getline(cin, input);

        thread.join();


         nb = replaceKeyword(fileCollection, keywords.front(), str2.c_str(), cout, (input[0]=='y'));
    }

    // Incorrect input
    else
    {
        goto reask;
    }





    std::cout << nb << " results found." << std::endl;

    std::cout << "\nThanks for trusting Search&Replace program." << endl;
    std::cout << "Press enter to exit, or notepad to open files in notepad++" << endl;

    while(getline(cin, input))
    {
        if(isRoughlyEqualTo("notepad",input))
        {
            // Let's launch notepad++ with all the files here


        }
        else if(isRoughlyEqualTo("restart",input))
        {
            return true;
        }
        else if(input.empty())
        {
            break;
        }
    }





    return false;
}
