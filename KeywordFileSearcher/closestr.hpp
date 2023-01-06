#ifndef CLOSESTR_HPP
#define CLOSESTR_HPP

#include <string>
#include <vector>

// Let's enable it
#define ENABLE_CLOSESTR

bool isRoughlyEqualTo(const std::string& original, const std::string& current);

bool tamperInsensitive(std::vector<std::string>& vstr);
bool tamperInsensitive(std::string& str);


#endif // CLOSESTR_HPP
