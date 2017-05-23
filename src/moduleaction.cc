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

#include "moduleaction.h"

#include <stdio.h>

namespace gdfm {

ModuleAction::ModuleAction() : name(DEFAULT_ACTION_NAME)
{
}

ModuleAction::ModuleAction(const std::string& name) : name(name)
{
}

const std::string&
ModuleAction::getName() const
{
    return name;
}

void
ModuleAction::setName(const std::string& name)
{
    this->name = name;
}

bool
ModuleAction::isVerbose() const
{
    return verbose;
}

void
ModuleAction::setVerbose(bool verbose)
{
    this->verbose = verbose;
}

bool
ModuleAction::isInteractive() const
{
    return interactive;
}

void
ModuleAction::setInteractive(bool interactive)
{
    this->interactive = interactive;
}

void
ModuleAction::verboseMessage(const char* format, ...)
{
    if (!verbose)
        return;

    va_list argumentList;
    va_start(argumentList, format);
    vVerboseMessage(format, argumentList);
    va_end(argumentList);
}

void
ModuleAction::vVerboseMessage(const char* format, va_list argumentList)
{
    if (!verbose)
        return;

    vprintf(format, argumentList);
}

void
ModuleAction::updateName()
{
    name = DEFAULT_ACTION_NAME;
}

void
ModuleAction::graphicalEdit(Gtk::Window& parent)
{
}

Gtk::Window*
ModuleAction::getParent() const
{
    return parent;
}

void
ModuleAction::setParent(Gtk::Window* parent)
{
    this->parent = parent;
}

std::vector<std::string>
ModuleAction::createConfigLines() const
{
    return std::vector<std::string>();
}
} /* namespace gdfm */
