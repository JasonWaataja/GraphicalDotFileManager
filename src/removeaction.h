/*
 * Copyright (c) 2017 Jason Waataja
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

#ifndef REMOVE_ACTION_H
#define REMOVE_ACTION_H

#include "moduleaction.h"

namespace gdfm {

const char DEFAULT_REMOVE_ACTION_NAME[] = "remove action";

class RemoveAction : public ModuleAction {
public:
    RemoveAction();
    RemoveAction(const std::string& filePath);
    RemoveAction(const std::string& filename, const std::string& directory);
    const std::string& getFilePath() const;
    void setFilePath(const std::string& filePath);
    void setFilePath(
        const std::string& filename, const std::string& directory);
    bool performAction() override;

    void updateName() override;
    void graphicalEdit(Gtk::Window& parent) override;
    std::vector<std::string> createConfigLines() const override;

private:
    std::string filePath;
};
} /* namespace gdfm */

#endif /* REMOVE_ACTION_H */
