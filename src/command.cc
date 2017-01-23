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

#include "command.h"

#include <assert.h>
#include <err.h>

#include <algorithm>
#include <iostream>
#include <sstream>

namespace gdfm {

Command::Command()
    : createActionFunction(getDefaultAction()),
      argumentCheckingType(NO_ARGUMENT_CKECK)
{
}

Command::Command(const std::string& name)
    : callableNames(1),
      createActionFunction(getDefaultAction()),
      argumentCheckingType(NO_ARGUMENT_CKECK)
{
    callableNames.push_back(name);
}

Command::Command(const std::string& name,
    std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
        createActionFunction)
    : callableNames(1),
      createActionFunction(createActionFunction),
      argumentCheckingType(NO_ARGUMENT_CKECK)
{
    callableNames.push_back(name);
}

void
Command::setNoArgumentChecking()
{
    argumentCheckingType = NO_ARGUMENT_CKECK;
    expectedArgumentCount = -1;
}

void
Command::setExactAgumentChecking(int argc)
{
    assert(argc >= 0);
    argumentCheckingType = EXACT_COUNT_ARGUMENT_CHECK;
    expectedArgumentCount = argc;
}

void
Command::setMinimumCountArgumentCheck(int argc)
{
    assert(argc >= 0);
    argumentCheckingType = MINIMUM_COUNT_ARGUMENT_CHECK;
    expectedArgumentCount = argc;
}

Command::ArgumentCheck
Command::getArgumentCheckingType() const
{
    return argumentCheckingType;
}

int
Command::getExpectedArgumentCount() const
{
    return (argumentCheckingType != NO_ARGUMENT_CKECK) ? expectedArgumentCount
                                                       : -1;
}

const std::vector<std::string>&
Command::getCallableNames() const
{
    return callableNames;
}

void
Command::addCallableName(const std::string& name)
{
    callableNames.push_back(name);
}

void
Command::setCallableNames(const std::vector<std::string>& names)
{
    /*
     * I hope this is copied quickly. I'm not sure if I should use std::copy
     * here.
     */
    callableNames = names;
}

std::function<std::shared_ptr<ModuleAction>(
    const std::vector<std::string>&, ReaderEnvironment&)>
Command::getCreateActionFunction() const
{
    return createActionFunction;
}

void
Command::setCreateActionFunction(std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
        createActionFunction)
{
    this->createActionFunction = createActionFunction;
}

std::shared_ptr<ModuleAction>
Command::createAction(const std::vector<std::string>& arguments,
    ReaderEnvironment& environment) const
{
    if (argumentCheckingType == EXACT_COUNT_ARGUMENT_CHECK
        && !checkArgumentCountEqual(arguments, expectedArgumentCount))
        return std::shared_ptr<ModuleAction>();
    if (argumentCheckingType == MINIMUM_COUNT_ARGUMENT_CHECK
        && !checkArgumentCountAtLeast(arguments, expectedArgumentCount))
        return std::shared_ptr<ModuleAction>();
    return createActionFunction(arguments, environment);
}

std::shared_ptr<ModuleAction>
Command::createAction(
    int argc, const std::string argv[], ReaderEnvironment& environment) const
{
    assert(argc >= 0);
    std::vector<std::string> arguments(argc);
    for (int i = 0; i < argc; i++)
        arguments[i] = argv[i];
    return createAction(arguments, environment);
}

std::function<std::shared_ptr<ModuleAction>(
    const std::vector<std::string>&, ReaderEnvironment&)>
Command::getDefaultAction()
{
    auto createActionFunction = [](const std::vector<std::string>&,
        const ReaderEnvironment&) -> std::shared_ptr<ModuleAction> {
        warnx("Calling command without behavior.");
        return std::shared_ptr<ModuleAction>();
    };
    return createActionFunction;
}

bool
Command::checkArgumentCountEqual(
    const std::vector<std::string>& arguments, int argc)
{
    assert(argc >= 0);

    if (arguments.size() == argc)
        return true;
    /*
     * Used a string stream here because it makes things more portable. On
     * my system, arguments.size() is an unsigned long, but I'm not sure if
     * that will be true everywhere so I went with this.
     */
    std::ostringstream messageStream;
    messageStream << "Incorrect number of arguments, expected exactly " << argc
                  << ", got " << arguments.size() << ".";
    warnx("%s", messageStream.str().c_str());
    return false;
}

bool
Command::checkArgumentCountAtLeast(
    const std::vector<std::string>& arguments, int argc)
{
    assert(argc >= 0);

    if (arguments.size() == argc)
        return true;
    /*
     * Used a string stream here because it makes things more portable. On
     * my system, arguments.size() is an unsigned long, but I'm not sure if
     * that will be true everywhere so I went with this.
     */
    std::ostringstream messageStream;
    messageStream << "Incorrect number of arguments, expected at least "
                  << argc << ", got " << arguments.size() << ".";
    warnx("%s", messageStream.str().c_str());
    return false;
}

bool
Command::matchesName(const std::string& name) const
{
    return std::find(callableNames.begin(), callableNames.end(), name)
        != callableNames.end();
}
} /* namespace gdfm */
