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

#ifndef COMMAND_H
#define COMMAND_H

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "module.h"
#include "readerenvironment.h"

namespace gdfm {

class Command {
public:
    /*
     * An enum to reprent various types of argument checking for commands.
     * These don't preclude any argument checking when generating an action,
     * they are just preliminary checks, and using NO_ARGUMENT_CHECK and doing
     * the checking yourself is valid.
     */
    enum ArgumentCheck {
        NO_ARGUMENT_CKECK,
        EXACT_COUNT_ARGUMENT_CHECK,
        MINIMUM_COUNT_ARGUMENT_CHECK
    };

    Command();
    Command(const std::string& name);
    Command(const std::string& name,
        std::function<std::shared_ptr<ModuleAction>(
            const std::vector<std::string>&, ReaderEnvironment&)>
            createActionFunction);

    void setNoArgumentChecking();
    void setExactAgumentChecking(int argc);
    void setMinimumCountArgumentCheck(int argc);

    ArgumentCheck getArgumentCheckingType() const;
    /*
     * Returns the exact argument count expected if exact checking is enable,
     * the minimum number of arguments if there's a minimum number of
     * arguments, -1 otherwise.
     */
    int getExpectedArgumentCount() const;

    std::shared_ptr<ModuleAction> createAction(
        const std::vector<std::string>& arguments,
        ReaderEnvironment& environment) const;
    std::shared_ptr<ModuleAction> createAction(
        int argc, const std::string[], ReaderEnvironment& environment) const;

    /*
     * Returns whether or not the given name matches any one of the names in
     * callableNames.
     */
    bool matchesName(const std::string& name) const;

    const std::vector<std::string>& getCallableNames() const;
    void addCallableName(const std::string& name);
    void setCallableNames(const std::vector<std::string>& names);

    std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
    getCreateActionFunction() const;
    void setCreateActionFunction(std::function<std::shared_ptr<ModuleAction>(
            const std::vector<std::string>&, ReaderEnvironment&)>
            createActionFunction);

    static std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
    getDefaultAction();

    /*
     * Checks whether or not the given argument list has exactly argc
     * arguments. Issues a warning if that is not the case.
     *
     * Returns whether or not the size of arguments is argc.
     */
    static bool checkArgumentCountEqual(
        const std::vector<std::string>& arguments, int argc);
    /*
     * Checks whether or not the given argument list has argc or greater
     * arguments.
     *
     * Returns whether or not the size of arguments is greater than or equal to
     * argc.
     */
    static bool checkArgumentCountAtLeast(
        const std::vector<std::string>& arguments, int argc);

private:
    std::vector<std::string> callableNames;
    ArgumentCheck argumentCheckingType;
    int expectedArgumentCount = -1;

    std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
        createActionFunction;
};
} /* namespace gdfm */

#endif /* COMMAND_H */
