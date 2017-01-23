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

#ifndef INSTALL_ACTION_H
#define INSTALL_ACTION_H

#include <string>

#include "moduleaction.h"

namespace gdfm {

class InstallAction : public ModuleAction {
public:
    InstallAction();
    InstallAction(const std::string& filename,
        const std::string& sourceDirectory,
        const std::string& destinationDirectory);
    InstallAction(const std::string& filename,
        const std::string& sourceDirectory, const std::string& installFilename,
        const std::string& destinationDirectory);
    bool performAction() override;
    std::string getFilePath() const;
    std::string getInstallationPath() const;
    const std::string& getFilename() const;
    void setFilename(const std::string& filename);
    const std::string& getSourceDirectory() const;
    void setSourceDirectory(const std::string& sourceDirectory);
    const std::string& getDestinationDirectory() const;
    void setDestinationDirectory(const std::string& destinationDirectory);
    const std::string& getInstallFilename() const;
    void setInstallFilename(const std::string& installFilename);

    void updateName() override;
    void graphicalEdit(Gtk::Window& parent) override;
    std::vector<std::string> createConfigLines() const override;

private:
    std::string filename;
    std::string sourceDirectory;
    std::string installFilename;
    std::string destinationDirectory;
};
} /* namespace gdfm */

#endif /* INSTALL_ACTION_H */
