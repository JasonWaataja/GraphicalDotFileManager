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

#ifndef FILE_CHECK_H
#define FILE_CHECK_H

#include <dirent.h>

#include <memory>
#include <string>
#include <vector>

#include "moduleaction.h"

namespace gdfm {

class FileCheckAction : public ModuleAction {
public:
    FileCheckAction();
    FileCheckAction(
        const std::string& sourcePath, const std::string& destinationPath);

    const std::string& getSourcePath() const;
    void setSourcePath(const std::string& sourcePath);
    const std::string& getDestinationPath() const;
    void setDestinationPath(const std::string& destinationPath);

    void setFiles(
        const std::string& sourcePath, const std::string& destinationPath);

    bool performAction() override;

    bool shouldUpdate() const;

    void updateName() override;
    void graphicalEdit(Gtk::Window& parent) override;
    std::vector<std::string> createConfigLines() const override;

private:
    /* Returns if neither path is a zero-length string. */
    bool hasFiles() const;

    bool shouldUpdateFile(const std::string& sourcePath,
        const std::string& destinationPath) const;
    bool shouldUpdateRegularFile(const std::string& sourcePath,
        const std::string& destinationPath) const;
    bool shouldUpdateDirectory(const std::string& sourcePath,
        const std::string& destinationPath) const;

    std::string sourcePath;
    std::string destinationPath;
};
} /* namespace 2016 */

#endif /* FILE_CHECK_H */
