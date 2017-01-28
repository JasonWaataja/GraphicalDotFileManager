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
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include <iostream>

#include "removeactioneditor.h"
#include "util.h"

namespace gdfm {

RemoveAction::RemoveAction() : ModuleAction(DEFAULT_REMOVE_ACTION_NAME)
{
}

RemoveAction::RemoveAction(const std::string& filePath) : filePath(filePath)
{
    updateName();
}

RemoveAction::RemoveAction(
    const std::string& filename, const std::string& directory)
    : filePath(directory + "/" + filename)
{
    updateName();
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
    updateName();
}

void
RemoveAction::setFilePath(
    const std::string& filename, const std::string& directory)
{
    filePath = directory + "/" + filename;
    updateName();
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
    return deleteFile(shellExpandPath(filePath));
}

void
RemoveAction::updateName()
{
    char* pathCopy = strdup(filePath.c_str());
    char* name = basename(pathCopy);
    setName(name);
    free(pathCopy);
}

std::vector<std::string>
RemoveAction::createConfigLines() const
{
    std::vector<std::string> lines;
    lines.push_back("remove " + filePath);
    return lines;
}

void
RemoveAction::graphicalEdit(Gtk::Window& parent)
{
    RemoveActionEditor editor(parent, this);
    editor.run();
}
} /* namespace gdfm */
