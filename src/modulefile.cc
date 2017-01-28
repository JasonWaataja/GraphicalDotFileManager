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

#include "modulefile.h"

#include "modulefileeditor.h"
#include "util.h"

namespace gdfm {

ModuleFile::ModuleFile()
{
}

ModuleFile::ModuleFile(const std::string& filename)
    : ModuleFile(filename, "~", filename)
{
}

ModuleFile::ModuleFile(
    const std::string& filename, const std::string& destinationDirectory)
    : ModuleFile(filename, destinationDirectory, filename)
{
}

ModuleFile::ModuleFile(const std::string& filename,
    const std::string& destinationDirectory,
    const std::string& destinationFilename)
    : filename(filename),
      destinationDirectory(destinationDirectory),
      destinationFilename(destinationFilename)
{
}

const std::string&
ModuleFile::getFilename() const
{
    return filename;
}

void
ModuleFile::setFilename(const std::string& filename)
{
    this->filename = filename;
}

const std::string&
ModuleFile::getDestinationDirectory() const
{
    return destinationDirectory;
}

void
ModuleFile::setDestinationDirectory(const std::string& destinationDirectory)
{
    this->destinationDirectory = destinationDirectory;
}

const std::string&
ModuleFile::getDestinationFilename() const
{
    return destinationFilename;
}

void
ModuleFile::setDestinationFilename(const std::string& destinationFilename)
{
    this->destinationFilename = destinationFilename;
}

std::string
ModuleFile::getSourcePath(const std::string& sourceDirectory) const
{
    std::string sourcePath = shellExpandPath(sourceDirectory + "/" + filename);
    return sourcePath;
}

std::string
ModuleFile::getDestinationPath() const
{
    std::string destinationPath =
        shellExpandPath(destinationDirectory + "/" + destinationFilename);
    return destinationPath;
}

std::shared_ptr<InstallAction>
ModuleFile::createInstallAction(const std::string& sourceDirectory) const
{
    return std::shared_ptr<InstallAction>(new InstallAction(
        filename, sourceDirectory, destinationFilename, destinationDirectory));
}

std::shared_ptr<RemoveAction>
ModuleFile::createUninstallAction() const
{
    return std::shared_ptr<RemoveAction>(
        new RemoveAction(getDestinationPath()));
}

std::shared_ptr<FileCheckAction>
ModuleFile::createUpdateAction(const std::string& sourceDirectory) const
{
    return std::shared_ptr<FileCheckAction>(new FileCheckAction(
        getSourcePath(sourceDirectory), getDestinationPath()));
}

std::vector<std::string>
ModuleFile::createConfigLines() const
{
    std::string line = filename;
    line += " " + destinationDirectory;
    line += " " + destinationFilename;
    std::vector<std::string> lines;
    lines.push_back(line);
    return lines;
}

void
ModuleFile::graphicalEdit(Gtk::Window& window)
{
    ModuleFileEditor editor(window, this);
    editor.run();
}
} /* namespace gdfm */
