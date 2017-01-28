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

#ifndef READER_ENVIRONMENT_H
#define READER_ENVIRONMENT_H

#include <map>
#include <memory>

#include "options.h"

namespace gdfm {

class ReaderEnvironment {
public:
    ReaderEnvironment();
    ReaderEnvironment(std::shared_ptr<DfmOptions> options);

    std::shared_ptr<DfmOptions> getOptions() const;
    void setOptions(std::shared_ptr<DfmOptions> options);

    const std::string& getDirectory() const;
    void setDirectory(const std::string& directory);

    /*
     * Sets the variable given by name to value. Overwrites the current value
     * if it exists. Calling hasVariable() after this method will return true.
     */
    void setVariable(const std::string& name, const std::string& value);
    /*
     * Removes the value of the variable given by name so that it has no value
     * and hasVariable returns false. Diffrent from setVariable(name, "")
     * because the variable still exists, its value is just the empty string.
     * With unsetVariable, that access fails.
     */
    void unsetVariable(const std::string& name);
    /*
     * Returns whether or not the given variable is currently in the
     * environment. This is true if setVariable has been called, but returns
     * false if unsetVariable() is called after that.
     *
     * This method is meant to be called directory before getVariable() so that
     * it doesn't throw an error when accessing it.
     *
     * Returns whether or not the environment has a variable with name.
     */
    bool hasVariable(const std::string& name);
    /*
     * Gets the value of the variable given by name. Throws an
     * std::runtime_error if the variable is not set in the current
     * environment.
     *
     * Returns the value of the variable given by name if it exists.
     */
    std::string getVariable(const std::string& name);
    /*
     * First checks to see if the variable given by name exists. If it does,
     * true is returned and value is set to the value. Returns false if the
     * variable is not set and dosen't affect value.
     *
     * Returns whether or not the variable given by name is set.
     */
    bool accessVariable(const std::string& name, std::string& value);

private:
    std::shared_ptr<DfmOptions> options;
    std::string directory;
    std::map<std::string, std::string> variables;
};
} /* namespace 2016 */

#endif /* READER_ENVIRONMENT_H */
