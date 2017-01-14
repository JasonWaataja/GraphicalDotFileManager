/*
 * Copyright (c) 2016 Jason Waataja
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>
#include <vector>

namespace gdfm {

/* Inital colon gets getopt to return ":" on missing required argument.  */
const char GETOPT_SHORT_OPTIONS[] = "iuaIcvgGpd:";

class DfmOptions {
public:
    DfmOptions();

    /*
     * Load options from argc and argv, which are meant to be directly from the
     * main function. This does not set any values back to their default, it
     * only changes the ones represented by the arguments. If you want the
     * default values, use a new DfmOptions object, which has the correct
     * default values.
     *
     * Returns true on success, false on failure.
     */
    bool loadFromArguments(int argc, char* argv[]);
    bool verifyArguments() const;

    bool installModulesFlag;
    bool uninstallModulesFlag;
    bool updateModulesFlag;
    bool allFlag;
    bool verboseFlag;
    bool interactiveFlag;
    bool generateConfigFileFlag;
    bool dumpConfigFileFlag;
    bool printModulesFlag;
    std::vector<std::string> remainingArguments;
    bool hasSourceDirectory;
    std::string sourceDirectory;

    /*
     * The getopt_long function sets flags sometimes. I want 1 to be true and 0
     * to be false. Converts the flag, which is an int, into a boolean.
     *
     * Returns the boolean equivalent of how an int is evaluated in a
     * conditional, so true for non-zero and false for zero.
     */
    static bool getoptFlagToBool(int flag);
    static void usage();

private:
    bool verifyFlagsConsistency() const;
    bool verifyDirectoryExists() const;
};
} /* namespace gdfm */

#endif /* OPTIONS_H */
