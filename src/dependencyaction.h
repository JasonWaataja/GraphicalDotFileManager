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

#ifndef DEPENDENCY_ACTION_H
#define DEPENDENCY_ACTION_H

#include <vector>

#include "moduleaction.h"

namespace gdfm {

class DependencyAction : public ModuleAction {
public:
    DependencyAction();
    DependencyAction(const std::vector<std::string>& dependencies);
    /* Constructor that takes a NULL terminated string of dependencies. */
    DependencyAction(const char* firstDependency, ...);

    const std::vector<std::string>& getDependencies() const;
    void setDependencies(const std::vector<std::string>& dependencies);
    /* Sets dependencies to the NULL terminated list of dependencies. */
    void setDependencies(const char* firstDependency, ...);
    void addDependency(const std::string& dependency);

    bool performAction() override;

    /*
     * Creates a string of the dependencies for use with the prompt. Uses space
     * by default.
     *
     * Returns a string of the dependencies separates by the delimiter, which
     * is space by default.
     */
    std::string getDependenciesAsString() const;
    std::string getDependenciesAsString(const std::string& delimiter) const;

    void updateName() override;
    void graphicalEdit(Gtk::Window& parent) override;
    std::vector<std::string> createConfigLines() const override;

private:
    std::vector<std::string> dependencies;
};
} /* namespace 2016 */

#endif /* DEPENDENCY_ACTION_H */
