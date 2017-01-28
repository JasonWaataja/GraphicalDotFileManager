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

#ifndef MODULE_FILE_H
#define MODULE_FILE_H

#include <memory>
#include <string>

#include "filecheckaction.h"
#include "installaction.h"
#include "removeaction.h"

namespace gdfm {

class ModuleFile {
public:
    ModuleFile();
    ModuleFile(const std::string& filename);
    ModuleFile(
        const std::string& filename, const std::string& destinationDirectory);
    ModuleFile(const std::string& filename,
        const std::string& destinationDirectory,
        const std::string& destinationFilename);

    const std::string& getFilename() const;
    void setFilename(const std::string& filename);
    const std::string& getDestinationDirectory() const;
    void setDestinationDirectory(const std::string& destinationDirectory);
    const std::string& getDestinationFilename() const;
    void setDestinationFilename(const std::string& destinationFilename);

    std::string getSourcePath(const std::string& sourceDirectory) const;
    std::string getDestinationPath() const;

    std::shared_ptr<InstallAction> createInstallAction(
        const std::string& sourceDirectory) const;
    std::shared_ptr<RemoveAction> createUninstallAction() const;
    std::shared_ptr<FileCheckAction> createUpdateAction(
        const std::string& sourceDirectory) const;

    std::vector<std::string> createConfigLines() const;

    void graphicalEdit(Gtk::Window& window);

private:
    std::string filename;
    std::string destinationDirectory;
    std::string destinationFilename;
};
} /* namespace gdfm */

#endif /* MODULE_FILE_H */
