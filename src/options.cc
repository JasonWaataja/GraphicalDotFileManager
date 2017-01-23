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

#include "options.h"

#include <err.h>
#include <getopt.h>

#include <iostream>

#include "util.h"

namespace gdfm {

/*
 * I couyld use default values since this requires C++ 11 and it's easier and
 * easier to read, but it only really offers a benefit it there are multiple
 * constructors.
 */
DfmOptions::DfmOptions()
    : installModulesFlag(false),
      uninstallModulesFlag(false),
      updateModulesFlag(false),
      allFlag(false),
      verboseFlag(false),
      interactiveFlag(false),
      generateConfigFileFlag(false),
      dumpConfigFileFlag(false),
      printModulesFlag(false),
      hasSourceDirectory(false)
{
}

bool
DfmOptions::loadFromArguments(int argc, char* argv[])
{
    int optionIndex = 0;
    /*
     * ClangFormat does a weird thing here, but I don't want to add a
     * suppression and I'll just leave it.
     */
    struct option longOptions[] = { { "install", no_argument, NULL, 'i' },
        { "uninstall", no_argument, NULL, 'u' },
        { "all", no_argument, NULL, 'a' },
        { "interactive", no_argument, NULL, 'I' },
        { "check", no_argument, NULL, 'c' },
        { "verbose", no_argument, NULL, 'v' },
        { "generate-config-file", no_argument, NULL, 'g' },
        { "dump-config-file", no_argument, NULL, 'G' },
        { "print-modules", no_argument, NULL, 'p' },
        { "directory", required_argument, NULL, 'd' }, { 0, 0, 0, 0 } };

    int getoptValue = getopt_long_only(
        argc, argv, GETOPT_SHORT_OPTIONS, longOptions, &optionIndex);

    while (getoptValue != -1) {
        switch (getoptValue) {
        case 0:
            /*
             * A flag was set, so do nothing. In the getopt example on the
             * glibc documentation, they do an additional check to see if it
             * sets a flag and something else if it's not, but in the
             * documentation they give I don't see any other reason it could
             * take 0. I'm kind of confused about it.
             */
            break;
        case 'i':
            installModulesFlag = true;
            break;
        case 'u':
            uninstallModulesFlag = true;
            break;
        case 'a':
            allFlag = true;
            break;
        case 'I':
            interactiveFlag = true;
            break;
        case 'c':
            updateModulesFlag = true;
            break;
        case 'g':
            generateConfigFileFlag = true;
            break;
        case 'G':
            dumpConfigFileFlag = true;
            break;
        case 'd':
            hasSourceDirectory = true;
            sourceDirectory = shellExpandPath(optarg);
            break;
        case 'p':
            printModulesFlag = true;
            break;
        case 'v':
            verboseFlag = true;
            break;
        case '?':
            usage();
            return false;
        case ':':
            /*
             * I'm not sure if this case is possible if optstring doesn't start
             * with a colon, but I'm including it just to be safe.
             */
            usage();
            return false;
        }
        getoptValue = getopt_long_only(
            argc, argv, GETOPT_SHORT_OPTIONS, longOptions, &optionIndex);
    }
    for (int i = optind; i < argc; i++)
        remainingArguments.push_back(std::string(argv[i]));
    return true;
}

bool
DfmOptions::verifyArguments() const
{
    if (!verifyFlagsConsistency())
        return false;
    if (!verifyDirectoryExists())
        return false;
    return true;
}

bool
DfmOptions::verifyFlagsConsistency() const
{
    int operationsCount = 0;
    if (installModulesFlag)
        operationsCount++;
    if (uninstallModulesFlag)
        operationsCount++;
    if (updateModulesFlag)
        operationsCount++;
    if (generateConfigFileFlag)
        operationsCount++;
    if (dumpConfigFileFlag)
        operationsCount++;
    if (printModulesFlag)
        operationsCount++;

    if (operationsCount == 0) {
        warnx("Must specify an operation.");
        usage();
        return false;
    }
    if (operationsCount > 1) {
        warnx("May only specify one operation.");
        usage();
        return false;
    }

    if (generateConfigFileFlag || dumpConfigFileFlag) {
        if (remainingArguments.size() > 0) {
            warnx("No arguments expected when creating config file.");
            usage();
            return false;
        }
        return true;
    }
    if (printModulesFlag) {
        if (remainingArguments.size() > 0) {
            warnx("No arguments expected when printing modules.");
            usage();
            return false;
        }
        return true;
    }
    /*
     * Fails if the all flag is passed and there are remaining arguments or the
     * all flag isn't passed and there are also no additional argumens. I used
     * the == operator here as an xnor operator. If both are true or both are
     * false, then fail.
     */
    if (allFlag == (remainingArguments.size() > 0)) {
        warnx(
            "Must specify either the --all flag or at least one remaining argument, but not both.");
        usage();
        return false;
    }
    return true;
}

bool
DfmOptions::verifyDirectoryExists() const
{
    if (hasSourceDirectory) {
        if (!fileExists(sourceDirectory)) {
            warnx("Directory doesn't exist: %s.", sourceDirectory.c_str());
            return false;
        }
        if (!isDirectory(sourceDirectory)) {
            warnx("Given file isn't a directory: %s", sourceDirectory.c_str());
            return false;
        }
    }
    return true;
}

void
DfmOptions::usage()
{
    std::cout
        << "usage: dfm [-Iv] [-c|-g|-G|-i|-u|-p] [-d directory] [-a|[MODULES]]"
        << std::endl;
}
} /* namespace gdfm */
