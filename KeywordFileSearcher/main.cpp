#include <iostream>
#include <cstdio>
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
unsigned searchKeywords(stringvec& fileCollection, stringvec& keywords, std::ostream& output, std::vector<std::string>& warnings)
{
    unsigned nbOccurences=0;
    bool hasInsensitive = tamperInsensitive(keywords);

    for(string& str1: fileCollection)
    {
        try
        {
            if(keywords.empty() || readFile(str1, keywords, hasInsensitive)){
                output << str1 << endl;
                nbOccurences++;
            }
        }
        catch(const FileException& e) {
            // we couldn't open this file, so let's add it to the warning list
            warnings.emplace_back(str1);
        }
    }

    return nbOccurences;
}


unsigned searchKeywordsWithLines(stringvec& fileCollection, stringvec& results, stringvec& keywords, std::ostream& output, stringvec& warnings)
{
    unsigned nbOccurences=0;
    bool hasInsensitive = tamperInsensitive(keywords);

    for(string& str1: fileCollection)
    {
        try
        {
            if(keywords.empty() || readFileWithLine(str1, results, keywords, output, hasInsensitive)){
                nbOccurences++;
            }
        }
        catch(const FileException& e)
        {
            warnings.push_back(str1);
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
unsigned replaceKeyword(const stringvec& fileCollection, const string& keywordSearched, const char* keywordReplaced, std::ostream& output, bool addNumberAtTheEnd, stringvec& warnings)
{
    unsigned nbOccurences=0;

    std::string real(keywordReplaced);
    std::vector<std::string> tmp = {keywordSearched};
    bool hasInsensitive = tamperInsensitive(tmp);

    for(const string& str1: fileCollection)
    {
        if(addNumberAtTheEnd){
            // Copy the initial string
            real = keywordReplaced;

            // Add the number string to it all
            addNumberToStr(real, nbOccurences);
        }

        try
        {

        if(readFile(str1, tmp, hasInsensitive)
        && replaceKeywordFile(str1, keywordSearched, real)){
            output << str1 << endl;
            nbOccurences++;
        }

        }
        catch(const FileException& e)
        {
            warnings.push_back(str1);
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

// only works with ending that are case insensitive and that have been tampered with
bool hasEndingWhatever(const std::string& fullString, const std::string& ending)
{
    int start = fullString.size()-ending.size();

    for(int i=1; i<ending.size(); ++i){
        if(toupper(fullString[i+start])!=ending[i])
            return false;
    }
    return true;
}


void filterFilepathByEnding(std::vector<std::string>& fileCollection, std::vector<std::string>& extensions)
{
    // Let's toupper extension that are marked case insensitive
    tamperInsensitive(extensions);

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
            if((curExt.front()=='#' && hasEndingWhatever(*it, curExt))
             ||(curExt.front()!='#' && hasEnding(*it, curExt))){
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

        for(string& curExt : extensions)
        {
            if((curExt.front()=='#' && hasEndingWhatever(*it, curExt))
             ||(curExt.front()!='#' && hasEnding(*it, curExt))){
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
        //std::cout << "size: " << fileCollection.size() << std::endl;
        thread = std::thread(filterFilepathByEnding, std::ref(fileCollection), std::ref(extensionsToKeep));
    }


    //std::ofstream resultFile("C:\\Users\\divouxje\\Desktop\\output.txt");

    reask:

    cout << "\nType 1 to search and 2 to replace (+: print involved lines):" << endl;

    std::vector<std::string> warnings;
    warnings.reserve(fileCollection.size()/5);

    getline(cin, input);

    unsigned nb=0;

    stringvec results;

    // search mode
    if(input == "1")
    {
        thread.join();
        nb = searchKeywords(fileCollection, keywords, cout, warnings);
    }
    else if(input == "1+")
    {
        thread.join();

        if(keywords.empty())
            nb = searchKeywords(fileCollection, keywords, cout, warnings);
        else
            nb = searchKeywordsWithLines(fileCollection, results, keywords, cout, warnings);
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


         nb = replaceKeyword(fileCollection, keywords.front(), str2.c_str(), cout, (input[0]=='y'), warnings);
    }

    // Incorrect input
    else
    {
        goto reask;
    }





    std::cout << nb << " results found." << std::endl;
    if(!warnings.empty())
        std::cout << "warning: " << warnings.size() << " files could not be opened." << std::endl;

    std::cout << "\nThanks for trusting Search&Replace program." << endl;
    std::cout << "Press enter to exit";
    if(!results.empty())
        std::cout << ", or notepad to open files in notepad++";
    if(!warnings.empty())
        std::cout << ", or 'errors' to show the list of files that could not be opened.";
    std::cout << std::endl;

    while(std::cout << "\n > " && getline(cin, input))
    {
        if(isRoughlyEqualTo("notepad",input))
        {
            // Let's launch notepad++ with all the files here

            // ! To be implemented !
        }
        else if(isRoughlyEqualTo("restart",input))
        {
            return true;
        }
        else if(isRoughlyEqualTo("errors", input))
        {
            // Let's display warnings

            std::cout << "\Below, the list of files that could not be opened:" << std::endl;
            for(const std::string& s: warnings)
                std::cout << s << std::endl;
        }
        else if(input == "deleteall")
        {
            // Let's ask confirmation from the user
            std::cout << "Are you sure you wanna erase all the files listed above ?" << std::endl;
            std::cout << "Please type 'yes' to confirm." << std::endl;


            if(std::getline(std::cin, input) && input == "yes")
            {
                std::cout << "Please press enter to confirm every file being deleted." << std::endl;

                for(const std::string& s: fileCollection)
                {
                    std::cout << s;
                    std::getline(std::cin, input);

                    if(remove(s.c_str())!=0)
                        std::cout << "The file could not be deleted." << std::endl;
                }
            }
        }

        else if(input.empty() || isRoughlyEqualTo(input,"exit"))
        {
            break;
        }
    }





    return false;
}
