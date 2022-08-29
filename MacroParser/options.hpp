#ifndef OPTIONS_HPP
#define OPTIONS_HPP

/// Static options ///
// Options dealt with at compilation time


/// Dynamic options ///
// Options that can be modified and configured by the user at runtime

#include <fstream>
#include <string>
#include "config.hpp"

class Options
{
public:
    Options();
    void toStream(std::ostream& stream) const;

    // Getters
    bool doesImportOnlySourceFileExtension() const;
    bool doesImportMacroCommented() const;
    bool doesPrintReplacements() const;
    bool doesPrintExprAtEveryStep() const;

private:
    bool saveToFile(const char* filename) const;
    bool loadFromFile(const char* filename);
    static bool loadBooleanValue(const std::string& input, bool& boolean);
    void resetToDefault();

    bool importOnlySourceFileExtension;
    bool importMacroCommented; // should macro that are inside comment such as // and /* be imported ?
    bool printReplacements; //
    bool printExprAtEveryStep;

friend void askUserAboutConfig(Options& options);
};


#endif // OPTIONS_HPP
