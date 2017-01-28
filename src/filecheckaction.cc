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

#include "filecheckaction.h"

#include <sys/stat.h>

#include <assert.h>
#include <err.h>
#include <libgen.h>
#include <stdlib.h>
#include <stdlib.h>
#include <string.h>

#include <fstream>

#include "filecheckeditor.h"
#include "installaction.h"
#include "util.h"

namespace gdfm {

FileCheckAction::FileCheckAction()
{
}

FileCheckAction::FileCheckAction(
    const std::string& sourcePath, const std::string& destinationPath)
    : sourcePath(sourcePath), destinationPath(destinationPath)
{
    updateName();
}

const std::string&
FileCheckAction::getSourcePath() const
{
    return sourcePath;
}

void
FileCheckAction::setSourcePath(const std::string& sourcePath)
{
    this->sourcePath = sourcePath;
    updateName();
}

const std::string&
FileCheckAction::getDestinationPath() const
{
    return destinationPath;
}

void
FileCheckAction::setDestinationPath(const std::string& destinationPath)
{
    this->destinationPath = destinationPath;
}

void
FileCheckAction::setFiles(
    const std::string& sourcePath, const std::string& destinationPath)
{
    setSourcePath(sourcePath);
    updateName();
    setDestinationPath(destinationPath);
}

bool
FileCheckAction::shouldUpdateRegularFile(
    const std::string& sourcePath, const std::string& destinationPath) const
{
    if (sourcePath == destinationPath)
        return false;
    if (sourcePath.length() == 0 || destinationPath.length() == 0)
        return false;

    std::ifstream sourceReader(sourcePath.c_str());
    if (!sourceReader.is_open()) {
        return false;
    }
    /*
     * This section returns true on failure because it means that it could open
     * the file it is supposed to be a copy of but not the copied file, meaning
     * it needs to be updated.
     */
    std::ifstream destinationReader(destinationPath.c_str());
    if (!destinationReader.is_open()) {
        sourceReader.close();
        return true;
    }
    std::string sourceLine;
    std::string destinationLine;
    bool sourceReadStatus = false;
    bool destinationReadStatus = false;

    sourceReadStatus = (std::getline(sourceReader, sourceLine)) ? true : false;
    destinationReadStatus =
        (std::getline(destinationReader, destinationLine)) ? true : false;

    if (sourceReadStatus != destinationReadStatus) {
        sourceReader.close();
        destinationReader.close();
        return true;
    }
    while (sourceReadStatus && destinationReadStatus) {
        if (sourceLine != destinationLine) {
            sourceReader.close();
            destinationReader.close();
            return true;
        }
        sourceReadStatus =
            (std::getline(sourceReader, sourceLine)) ? true : false;
        destinationReadStatus =
            (std::getline(destinationReader, destinationLine)) ? true : false;
        if (sourceReadStatus != destinationReadStatus) {
            sourceReader.close();
            destinationReader.close();
            return true;
        }
    }
    sourceReader.close();
    destinationReader.close();
    return false;
}

bool
FileCheckAction::shouldUpdate() const
{
    if (!hasFiles()) {
        warnx("Missing file to check for updates.");
        return false;
    }
    return shouldUpdateFile(
        shellExpandPath(sourcePath), shellExpandPath(destinationPath));
}

bool
FileCheckAction::shouldUpdateDirectory(
    const std::string& sourcePath, const std::string& destinationPath) const
{
    if (sourcePath == destinationPath)
        return false;
    if (sourcePath.size() == 0 || destinationPath.size() == 0)
        return false;

    struct dirent** sourceEntries = nullptr;
    struct dirent** destinationEntries = nullptr;

    int sourceCount =
        scandir(sourcePath.c_str(), &sourceEntries, returnOne, alphasort);
    if (sourceCount == -1) {
        return false;
    }
    int destinationCount = scandir(
        destinationPath.c_str(), &destinationEntries, returnOne, alphasort);
    /*
     * The logic here is the same as in the regular file function. If it can't
     * open the destination direcvory, there must be a problem and should
     * thusly be updated.
     */
    if (destinationCount == -1) {
        return true;
    }
    if (sourceCount != destinationCount) {
        return true;
    }
    /*
     * Both lists were sorted using the same algorithm, alphasort, which means
     * that if they contain the same entries they should be in the same order.
     */
    for (int i = 0; i < sourceCount; i++) {
        struct dirent* sourceEntry = sourceEntries[i];
        struct dirent* destinationEntry = destinationEntries[i];
        /*
         * I dont' think I have to check destinationEntry here because they
         * should both start with "." and ".." anyways.
         */
        if (strcmp(sourceEntry->d_name, ".") == 0
            || strcmp(sourceEntry->d_name, "..") == 0
            || strcmp(destinationEntry->d_name, ".") == 0
            || strcmp(destinationEntry->d_name, "..") == 0)
            continue;
        if (strcmp(sourceEntry->d_name, destinationEntry->d_name) != 0)
            return true;
        std::string sourceEntryPath = sourcePath + "/" + sourceEntry->d_name;
        std::string destinationEntryPath =
            destinationPath + "/" + destinationEntry->d_name;
        if (shouldUpdateFile(sourceEntryPath, destinationEntryPath))
            return true;
    }
    return false;
}

bool
FileCheckAction::shouldUpdateFile(
    const std::string& sourcePath, const std::string& destinationPath) const
{
    struct stat sourceInfo;
    if (stat(sourcePath.c_str(), &sourceInfo) != 0) {
        return false;
    }
    struct stat destinationInfo;
    if (stat(destinationPath.c_str(), &destinationInfo) != 0) {
        return true;
    }
    if (!S_ISREG(sourceInfo.st_mode) && !S_ISDIR(sourceInfo.st_mode))
        return false;
    if (!S_ISREG(destinationInfo.st_mode) && !S_ISDIR(destinationInfo.st_mode))
        return true;
    if (sourceInfo.st_mode != destinationInfo.st_mode)
        return true;

    if (S_ISREG(sourceInfo.st_mode))
        return shouldUpdateRegularFile(sourcePath, destinationPath);
    /*
     * It was already checked about that the source mode is either a regular
     * file or a directory, so if it's not a regular file then it must be a
     * directory here.
     */
    return shouldUpdateDirectory(sourcePath, destinationPath);
}

bool
FileCheckAction::hasFiles() const
{
    return sourcePath.size() != 0 && destinationPath.size() != 0;
}

bool
FileCheckAction::performAction()
{
    if (!shouldUpdate())
        return true;
    /*
     * I shouldn't have to create a non-const copy of the string, but the
     * dirname function and basename function (when including libgen.h)
     * requires that the argument to be non-const on my system. Making a
     * non-const copy allows for more portability, I guess.
     */
    char* sourcePath = strdup(shellExpandPath(this->sourcePath).c_str());
    if (sourcePath == NULL)
        err(EXIT_FAILURE, NULL);
    char* destinationPath =
        strdup(shellExpandPath(this->destinationPath).c_str());
    /*
     * I'm not sure if I still should free sourcePath since it's a fatal error.
     * */
    if (destinationPath == NULL) {
        free(sourcePath);
        err(EXIT_FAILURE, NULL);
    }

    /*
     * I'm making copies here because on some system the returned pointer is
     * overwritten in later calls. This might create an error in the future
     * because these two functions are allowed to modify their contents.
     * However, I think that basename is only allowed to remove trailing
     * slashes so if I'm calling it first I think it's alright.
     */
    std::string sourceBasename = basename(sourcePath);
    std::string sourceDirectory = dirname(sourcePath);
    free(sourcePath);

    std::string destinationBasename = basename(destinationPath);
    std::string destinationDirectory = dirname(destinationPath);
    free(destinationPath);

    InstallAction action(sourceBasename, sourceDirectory, destinationBasename,
        destinationDirectory);
    action.setVerbose(isVerbose());
    action.setInteractive(isInteractive());
    return action.performAction();
}

void
FileCheckAction::updateName()
{
    char* sourceCopy = strdup(sourcePath.c_str());
    char* name = basename(sourceCopy);
    setName(name);
    free(sourceCopy);
}

std::vector<std::string>
FileCheckAction::createConfigLines() const
{
    std::vector<std::string> lines;
    lines.push_back("remove " + sourcePath + " " + destinationPath);
    return lines;
}

void
FileCheckAction::graphicalEdit(Gtk::Window& parent)
{
    FileCheckEditor editor(parent, this);
    editor.run();
}
} /* namespace gdfm */
