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

#include "installaction.h"

#include <dirent.h>
#include <err.h>

#include <iostream>

#include "installactioneditor.h"
#include "util.h"

namespace gdfm {

InstallAction::InstallAction() : ModuleAction("generic install action")
{
}

InstallAction::InstallAction(const std::string& filename,
    const std::string& sourceDirectory,
    const std::string& destinationDirectory)
    : ModuleAction(filename),
      filename(filename),
      sourceDirectory(sourceDirectory),
      installFilename(filename),
      destinationDirectory(destinationDirectory)
{
}

InstallAction::InstallAction(const std::string& filename,
    const std::string& sourceDirectory, const std::string& installFilename,
    const std::string& destinationDirectory)
    : ModuleAction(filename),
      filename(filename),
      sourceDirectory(sourceDirectory),
      installFilename(installFilename),
      destinationDirectory(destinationDirectory)
{
}

std::string
InstallAction::getFilePath() const
{
    return sourceDirectory + "/" + filename;
}

std::string
InstallAction::getInstallationPath() const
{
    return destinationDirectory + "/" + installFilename;
}

const std::string&
InstallAction::getFilename() const
{
    return filename;
}

void
InstallAction::setFilename(const std::string& filename)
{
    this->filename = filename;
    updateName();
}

const std::string&
InstallAction::getSourceDirectory() const
{
    return sourceDirectory;
}

void
InstallAction::setSourceDirectory(const std::string& sourceDirectory)
{
    this->sourceDirectory = sourceDirectory;
}

const std::string&
InstallAction::getDestinationDirectory() const
{
    return destinationDirectory;
}

void
InstallAction::setDestinationDirectory(const std::string& destinationDirectory)
{
    this->destinationDirectory = destinationDirectory;
}

const std::string&
InstallAction::getInstallFilename() const
{
    return installFilename;
}

void
InstallAction::setInstallFilename(const std::string& installFilename)
{
    this->installFilename = installFilename;
}

bool
InstallAction::performAction()
{
    std::string sourcePath = shellExpandPath(getFilePath());
    std::string destinationPath = shellExpandPath(getInstallationPath());

    if (isInteractive()) {
        std::string prompt =
            "Install " + sourcePath + " to " + destinationPath + "?";
        if (!getYesOrNo(prompt))
            return true;
        std::cout << std::endl;
    }
    verboseMessage("Installing %s to %s.\n\n", sourcePath.c_str(),
        destinationPath.c_str());

    if (!fileExists(sourcePath)) {
        warnx(
            "File %s doesn't exist, can't be installed.", sourcePath.c_str());
        return false;
    }
    std::string installDir = shellExpandPath(destinationDirectory);
    if (!ensureDirectoriesExist(installDir)) {
        warnx(
            "Failed to use destination directory %s, isn't directory or couldn't be created.",
            destinationDirectory.c_str());
    }
    return copyFile(sourcePath, destinationPath);
}

void
InstallAction::updateName()
{
    setName(filename);
}

std::vector<std::string>
InstallAction::createConfigLines() const
{
    std::string line = "install";
    line += " " + filename;
    line += " " + sourceDirectory;
    line += " " + installFilename;
    line += " " + destinationDirectory;
    std::vector<std::string> lines;
    lines.push_back(line);
    return lines;
}

void
InstallAction::graphicalEdit(Gtk::Window& parent)
{
    InstallActionEditor editor(parent, this);
    editor.run();
}
} /* namespace gdfm */
