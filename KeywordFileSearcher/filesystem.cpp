#include "filesystem.hpp"


void replace_all(
    std::string& s,
    std::string const& toReplace,
    std::string const& replaceWith
) {
    std::ostringstream oss;
    std::size_t pos = 0;
    std::size_t prevPos = pos;

    while (true) {
        prevPos = pos;
        pos = s.find(toReplace, pos);
        if (pos == std::string::npos)
            break;
        oss << s.substr(prevPos, pos - prevPos);
        oss << replaceWith;
        pos += toReplace.size();
    }

    oss << s.substr(prevPos);
    s = oss.str();
}


static bool hasEnding (std::string const &fullString, std::string const &ending)
{
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}


BOOL DirectoryExists(LPCTSTR szPath)
{
  DWORD dwAttrib = GetFileAttributes(szPath);

  return (dwAttrib != INVALID_FILE_ATTRIBUTES &&
         (dwAttrib & FILE_ATTRIBUTE_DIRECTORY));
}


void read_directory(const std::string& name, stringvec& v)
{
    std::string pattern(name);
    pattern.append("\\*");
    WIN32_FIND_DATA data;
    HANDLE hFind;
    if ((hFind = FindFirstFile(pattern.c_str(), &data)) != INVALID_HANDLE_VALUE) {
        do {
            v.emplace_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
    }
}

void explore_directory(std::string directory_name, stringvec& fileCollection)
{
   stringvec sv;

    // We look for all the files and folders that are in that directory
    read_directory(directory_name, sv);

    // We explore all teh different inputs
    for(size_t i=0; i < sv.size(); ++i)
    {
        // IF there is a point then, it's a file
        // Also it's important to note that filenames "." and ".." aren't files
        // oldimplemntation: if(sv[i].find('.') != std::string::npos && sv[i]!="." && sv[i]!="..")
        if(!DirectoryExists((directory_name+'\\'+sv[i]).c_str()) && sv[i]!="." && sv[i]!="..")
            fileCollection.emplace_back(directory_name+'\\'+sv[i]);

        // Else it's a folder, and you have to reexecute the function recursilvely
        else if(sv[i]!="." && sv[i]!=".."){
            explore_directory
            (directory_name+'\\'+sv[i], fileCollection);
        }
    }
}

bool readFile(const string& path, const stringvec& words)
{
    ifstream file(path);

    if(!file){
        cerr << "Can't open file:" << path << endl;
        return false;
    }

    bool found = false;
    string const* keyword = nullptr;
    unsigned counter = 0;
    char readCharacter;

    while(!found && file.get(readCharacter))
    {
        if(!keyword)
        {
            for(unsigned i=0; i<words.size(); ++i){
                if(words[i][0] == readCharacter){
                    keyword = &(words[i]);
                    counter = 1;
                }
            }
        }
        else if( (*keyword)[counter] == readCharacter)
        {
            counter++;
        }
        else if( (*keyword)[counter] == '\0')
        {
            found = true;
        }
        else
            counter = 0;
    }

    return found;
}



// Function to replace all the occurrences
// of the substring S1 to S2 in string S
void modifyString(string& s, const string& s1, const string& s2)
{
    // Stores the resultant string
    string ans = "";

    // Traverse the string s
    for (int i = 0; i < s.length(); i++)
    {
        int k = 0;

        // If the first character of
        // string s1 matches with the
        // current character in string s
        if (s[i] == s1[k]
            && i + s1.length()
                   <= s.length())
        {
            int j;

            // If the complete string
            // matches or not
            for (j = i; j < i + s1.length(); j++) {

                if (s[j] != s1[k]) {
                    break;
                }
                else {
                    k = k + 1;
                }
            }

            // If complete string matches
            // then replace it with the
            // string s2
            if (j == i + s1.length()) {
                ans.append(s2);
                i = j - 1;
            }

            // Otherwise
            else {
                ans.push_back(s[i]);
            }
        }

        // Otherwise
        else {
            ans.push_back(s[i]);
        }
    }

    // Print the resultant string
    cout << ans;
}




bool replaceKeywordFile(const string& path, const string& initialKeyword, const string& finalKeyword)
{
    ifstream file(path, ios::in);

    if(!file){
        cerr << "Can't read file:" << path << endl;
        return false;
    }

    string total;
    string curLine;

    while(getline(file, curLine))
    {
        if(!total.empty())
            total += '\n';

        total += curLine;
    }

    file.close();

    // If the word is in it
    if(total.find(initialKeyword) != string::npos)
    {
        // Replace the word
        replace_all(total, initialKeyword, finalKeyword);
    }

    // Let's rewrite the modified content into the file
    ofstream file2(path);

    if(!file2){
        cerr << "Can't write file:" << path << endl;
        cerr << strerror(errno) << endl; // displays "Permission denied"
        return false;
    }

    //cout << "content of total: " << total << endl;

    file2 << total;

    file2.close();

    return true;

}



