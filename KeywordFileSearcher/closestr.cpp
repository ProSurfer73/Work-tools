#include "closestr.hpp"

#ifdef ENABLE_CLOSESTR

bool isRoughlyEqualTo(const std::string& original, const std::string& current)
{
    int errors=2;

    for(int i=0, j=0;j<original.size(); ++i,++j)
    {
        if(i>=current.size())
        {
            errors -= 2;
        }
        else if(original[j]!=current[i])
        {
            if(current[i]==original[j+1])
            {
                errors--;
                j++;
            }
            else if(i>0 && current[i]==original[j-1])
            {
                errors--;
                j--;
            }
            else
            {
                errors -= 2;
            }
        }

        if(errors < 0) {
            return false;
        }
    }

    return true;
}

#else

bool isRoughlyEqualTo(const std::string& original, const std::string& current)
{
    return original == current;
}

#endif

#include <cstring>

bool tamperInsensitive(std::vector<std::string>& vstr)
{
    bool myreturn=false;

    for(std::string& str: vstr)
    {
        if(tamperInsensitive(str))
            myreturn=true;

    }

    return myreturn;
}

bool tamperInsensitive(std::string& str)
{
    if(!str.empty() && str.front()=='#')
    {
        for(char& c: str) {
            c = std::toupper(c);
        }

        return true;
    }

    return false;
}
