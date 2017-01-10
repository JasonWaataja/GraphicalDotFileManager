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


#ifndef FILEFINDER_H
#define FILEFINDER_H

#include <string>

namespace gdfm {

class FileFinder {
public:
    /*
     * Searches a set of locations. They are /usr/share, /usr/local/share, and
     * the current directory. Terminates the program if a file with given name
     * could not be found. This is so that you don't have to check if it was
     * found and write cleaner code.
     *
     * Returns the full path to the resource.
     */
    static std::string findResource(const std::string& resourceName);
    /*
     * Searches for the given resource with resourceName as in findResource().
     * Unlike findResource(), though, it doesn't kill the program if the
     * resource could not be found. If it is found, destinationString is set to
     * the full path, but is not affected if it is not found.
     *
     * Returns true if a resource was found, false otherwise.
     */
    static bool searchResource(
        const std::string& resourceName, std::string& destinationString);
};
} /* namespace 2017 */

#endif /* FILEFINDER_H */
