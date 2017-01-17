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

#include "shellaction.h"

#include <err.h>
#include <stdio.h>

#include <iostream>

#include "shelleditor.h"

namespace gdfm {

ShellAction::ShellAction()
    : ModuleAction(DEFAULT_SHELL_ACTION_NAME), shellCommands()
{
}

ShellAction::ShellAction(const std::string& name)
    : ModuleAction(name), shellCommands()
{
}

const std::vector<std::string>&
ShellAction::getShellCommands() const
{
    return shellCommands;
}

void
ShellAction::setShellCommands(const std::vector<std::string>& shellCommands)
{
    this->shellCommands = shellCommands;
}

bool
ShellAction::performAction()
{
    if (shellCommands.size() < 1)
        return true;
    if (isVerbose() && shellCommands.size() > 0) {
        std::cout << "Executing with shell:";
        if (shellCommands.size() == 1)
            std::cout << " \"" << shellCommands[0] << "\"" << std::endl;
        else {
            std::cout << std::endl;
            for (const auto& commandName : shellCommands)
                std::cout << "\t" << commandName << std::endl;
        }
    }
    std::string command = shellCommands[0];
    for (std::vector<std::string>::size_type i = 1; i < shellCommands.size();
         i++)
        command += "; " + shellCommands[i];
    return system(command.c_str()) == 0;
}

void
ShellAction::addCommand(const std::string& command)
{
    shellCommands.push_back(command);
}

void
ShellAction::updateName()
{
    setName("shell command");
}

void
ShellAction::graphicalEdit(Gtk::Window& parent)
{
    ShellEditor editor(parent, this);
    editor.run();
}

std::vector<std::string>
ShellAction::createConfigLines() const
{
    std::vector<std::string> lines;
    lines.push_back("sh");
    for (const auto& command : shellCommands)
        lines.push_back("\t" + command);
    return lines;
}
} /* namespace gdfm */
