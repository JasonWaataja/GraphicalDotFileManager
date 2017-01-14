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

#ifndef MODULE_H
#define MODULE_H

#include <memory>
#include <string>
#include <vector>

#include "moduleaction.h"

namespace gdfm {

const char DEFAULT_MODULE_NAMES[] = "Generic Module";

class Module {
public:
    Module();
    Module(const std::string& name);
    void addInstallAction(std::shared_ptr<ModuleAction> action);
    void addUninstallAction(std::shared_ptr<ModuleAction> action);
    void addUpdateAction(std::shared_ptr<ModuleAction> action);
    bool install() const;
    bool uninstall() const;
    bool update() const;
    const std::string& getName() const;
    void setName(const std::string& name);

private:
    std::string name;
    std::vector<std::shared_ptr<ModuleAction>> installActions;
    std::vector<std::shared_ptr<ModuleAction>> uninstallActions;
    std::vector<std::shared_ptr<ModuleAction>> updateActions;
};
} /* namespace gdfm */

#endif /* MODULE_H */
