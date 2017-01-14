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

#include "removeaction.h"

#include <err.h>

#include <iostream>

#include "util.h"

namespace gdfm {

RemoveAction::RemoveAction(const std::string& filePath)
    : ModuleAction(DEFAULT_REMOVE_ACTION_NAME), filePath(filePath)
{
}

RemoveAction::RemoveAction(
    const std::string& filename, const std::string& directory)
    : ModuleAction(DEFAULT_REMOVE_ACTION_NAME),
      filePath(directory + "/" + filename)
{
}

const std::string&
RemoveAction::getFilePath() const
{
    return filePath;
}

void
RemoveAction::setFilePath(const std::string& filePath)
{
    this->filePath = filePath;
}

void
RemoveAction::setFilePath(
    const std::string& filename, const std::string& directory)
{
    filePath = directory + "/" + filename;
}

bool
RemoveAction::performAction()
{
    if (isInteractive()) {
        std::string prompt = "Remove " + filePath + "?";
        if (!getYesOrNo(prompt))
            return true;
        std::cout << std::endl;
    }
    verboseMessage("Removing %s.\n\n", filePath.c_str());
    return deleteFile(filePath);
}
} /* namespace gdfm */
