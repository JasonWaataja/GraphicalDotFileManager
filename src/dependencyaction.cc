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

#include "dependencyaction.h"

#include <err.h>
#include <stdarg.h>
#include <stdlib.h>

#include <iostream>

#include "dependencyeditor.h"

namespace gdfm {

DependencyAction::DependencyAction() : dependencies()
{
    updateName();
}

DependencyAction::DependencyAction(
    const std::vector<std::string>& dependencies)
    : dependencies(dependencies)
{
    updateName();
}

DependencyAction::DependencyAction(const char* firstDependency, ...)
{
    dependencies.push_back(std::string(firstDependency));

    va_list argumentList;
    va_start(argumentList, firstDependency);
    const char* dependency = va_arg(argumentList, const char*);
    while (dependency != NULL) {
        dependencies.push_back(std::string(dependency));
        dependency = va_arg(argumentList, const char*);
    }
    va_end(argumentList);
    updateName();
}

const std::vector<std::string>&
DependencyAction::getDependencies() const
{
    return dependencies;
}

void
DependencyAction::setDependencies(const std::vector<std::string>& dependencies)
{
    this->dependencies = dependencies;
}

void
DependencyAction::setDependencies(const char* firstDependency, ...)
{
    dependencies.clear();
    dependencies.push_back(std::string(firstDependency));

    va_list argumentList;
    va_start(argumentList, firstDependency);
    const char* dependency = va_arg(argumentList, const char*);
    while (dependency != NULL) {
        dependencies.push_back(std::string(dependency));
        dependency = va_arg(argumentList, const char*);
    }
    va_end(argumentList);
}

bool
DependencyAction::performAction()
{
    if (isInteractive()) {
        std::cout << "Do you have the following packages installed?"
                  << std::endl;
        std::cout << std::endl;
        std::cout << getDependenciesAsString() << std::endl;
        std::cout << std::endl;
        std::cout
            << "Enter command to install dependencies or leave blank to do nothing."
            << std::endl;

        /*
         * Print a dollar sign and a space to simulate being in a shell. I'm
         * not sure of anything better to do here, maybe add color some day.
         */
        std::cout << std::endl;
        std::cout << "$ ";
        std::string userInput;
        std::getline(std::cin, userInput);
        if (userInput.length() != 0) {
            int status = system(userInput.c_str());
            if (status == -1) {
                warnx("Failed to create process to execute command %s.",
                    userInput.c_str());
                return false;
            }
            /*
             * I'm not sure if I can assume that zero indicates success, but
             * I'm pretty sure that's a standard in most shells. I could
             * compare to EXIT_SUCCESS, but I'm not sure if that directly
             * corresponds to the shell's 0 return. I'm almost certain it is,
             * but this is easier.
             */
            if (status != 0) {
                warnx("Failed to execute command %s.", userInput.c_str());
                return false;
            }
        }
    }
    return true;
}

std::string
DependencyAction::getDependenciesAsString(const std::string& delimiter) const
{
    std::string dependencyString;
    if (dependencies.size() < 1)
        return dependencyString;
    dependencyString = dependencies[0];
    for (std::vector<std::string>::size_type i = 1; i < dependencies.size();
         i++) {
        dependencyString += delimiter + dependencies[i];
    }
    return dependencyString;
}

std::string
DependencyAction::getDependenciesAsString() const
{
    return getDependenciesAsString(" ");
}

void
DependencyAction::updateName()
{
    setName("Dependency Check");
}

std::vector<std::string>
DependencyAction::createConfigLines() const
{
    std::string line = "depend";
    for (const auto& dependency : dependencies)
        line += " " + dependency;
    std::vector<std::string> lines;
    lines.push_back(line);
    return lines;
}

void
DependencyAction::addDependency(const std::string& dependency)
{
    dependencies.push_back(dependency);
}

void
DependencyAction::graphicalEdit(Gtk::Window& parent)
{
    DependencyEditor editor(parent, this);
    editor.run();
}
} /* namespace gdfm */
