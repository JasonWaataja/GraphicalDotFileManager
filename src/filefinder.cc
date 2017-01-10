/*
 * Copyright (c) 2017 Jason Waataja
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

#include "filefinder.h"

#include <sys/stat.h>

#include <err.h>
#include <stdlib.h>

namespace gdfm {

std::string
FileFinder::findResource(const std::string& resourceName)
{
    std::string locations[] = { "/usr/share", "/usr/local/share", "." };
    for (const auto& location : locations) {
        std::string fullPath = location + "/" + resourceName;
        struct stat fileInfo;
        if (stat(fullPath.c_str(), &fileInfo) == 0)
            return fullPath;
    }
    err(EXIT_FAILURE, "Failed to find file with name \"%s\".",
        resourceName.c_str());
    /*
     * It shouldn't reach this point because the program should have just
     * terminated.
     */
    return "";
}

bool
FileFinder::searchResource(
    const std::string& resourceName, std::string& destinationString)
{
    std::string locations[] = { "/usr/share", "/usr/local/share", "." };
    for (const auto& location : locations) {
        std::string fullPath = location + "/" + resourceName;
        struct stat fileInfo;
        if (stat(fullPath.c_str(), &fileInfo) == 0) {
            destinationString = fullPath;
            return true;
        }
    }
    return false;
}
} /* namespace gdfm */
