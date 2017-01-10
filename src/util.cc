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

#include "util.h"
#include "config.h"

#include <sys/stat.h>

#include <err.h>
#include <libgen.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
/* For compatability with OpenBSD, which doesn't include wordexp.h. */
#ifdef HAVE_WORDEXP_H
#include <wordexp.h>
#endif

#include <fstream>

namespace dfm {

bool
getYesOrNo()
{
    /* Do the same behavior as with a prompt but with no prompt. */
    return getYesOrNo("");
}

bool
getYesOrNo(const std::string& prompt)
{
    std::cout << prompt << " ";
    std::string input;
    if (!std::getline(std::cin, input))
        errx(EXIT_FAILURE, "Failed to read input.");
    bool yesOrNo = true;
    while (!lineIsYesOrNo(input, yesOrNo)) {
        std::cout << "Please enter y or n.";
        if (!std::getline(std::cin, input))
            errx(EXIT_FAILURE, "Failed to read input.");
    }
    return yesOrNo;
}

bool
lineIsYesOrNo(const std::string& input, bool& yesOrNo)
{
    /*
     * This could be done by first checking if input's lenght is zero, then
     * checking the first character with input[0], but this is cooler and could
     * be extended to non-zero length character sequences.
     */
    if (input.find("Y") == 0 || input.find("y") == 0) {
        yesOrNo = true;
        return true;
    }
    if (input.find("N") == 0 || input.find("n") == 0) {
        yesOrNo = false;
        return true;
    }
    return false;
}

std::string
getCurrentDirectory()
{
    size_t currentSize = 128;
    char* currentDirectory = new char[currentSize];
    char* result = getcwd(currentDirectory, currentSize);
    while (result == NULL) {
        if (errno == ERANGE) {
            delete[] currentDirectory;
            currentSize *= 2;
            currentDirectory = new char[currentSize];
        } else
            err(EXIT_FAILURE, "Failed to get current directory.");
        result = getcwd(currentDirectory, currentSize);
    }
    std::string directoryString(currentDirectory);
    delete[] currentDirectory;
    return directoryString;
}

std::string
shellExpandPath(const std::string& path)
{
#ifdef HAVE_WORDEXP_H
    wordexp_t expr;
    if (wordexp(path.c_str(), &expr, 0) != 0)
        errx(EXIT_FAILURE, "Failed to expand path.");
    if (expr.we_wordc != 1)
        errx(EXIT_FAILURE, "Path expanded into multiple words.");
    std::string expandedPath = expr.we_wordv[0];
    wordfree(&expr);
    return expandedPath;
#else
    std::string pathCopy = path;
    size_t charPosition = pathCopy.find("~");
    while (charPosition != std::string::npos) {
        pathCopy.replace(charPosition, 1, getHomeDirectory());
        charPosition = pathCopy.find("~");
    }
    return pathCopy;
#endif
}

std::string
getHomeDirectory()
{
    struct passwd* userInfo = getpwuid(getuid());
    if (userInfo == NULL) {
        err(EXIT_FAILURE, "Failed to get user info.");
    }
    return userInfo->pw_dir;
}

bool
fileExists(const std::string& path)
{
    return access(path.c_str(), F_OK) == 0;
}

bool
isRegularFile(const std::string& path)
{
    struct stat pathInfo;
    return stat(path.c_str(), &pathInfo) == 0 && S_ISREG(pathInfo.st_mode);
}

bool
isDirectory(const std::string& path)
{
    struct stat pathInfo;
    return stat(path.c_str(), &pathInfo) == 0 && S_ISDIR(pathInfo.st_mode);
}

bool
deleteRegularFile(const std::string& path)
{
    struct stat pathInfo;
    if (stat(path.c_str(), &pathInfo) != 0)
        return true;
    if (!S_ISREG(pathInfo.st_mode))
        return false;
    if (remove(path.c_str()) != 0) {
        warnx("Failed to remove file %s.", path.c_str());
        return false;
    }
    return true;
}

int
deleteDirectoryHelper(
    const char* fpath, const struct stat* sb, int typeflag, struct FTW* ftwbuf)
{
    if (typeflag == FTW_F)
        return (deleteRegularFile(fpath)) ? 0 : -1;
    /*
     * Because this function is meant to be called by nftw with the FTW_DEPTH
     * flag, all of the regular files and directories that are children of this
     * directory should have been processed before this one so it should be
     * empty if it all of its children are regular files and directories. If
     * there was a symlink or something somewhere, then this will fail and stop
     * the tree walk.
     *
     * Also, I should only have to check against FTW_DP because that is what is
     * passed when FTW_DEPTH is passed, but I'm checking against all of them
     * just in case.
     */
    if ((typeflag == FTW_D || typeflag == FTW_DNR || typeflag == FTW_DP)
        && rmdir(fpath) == 0)
        return 0;
    /*
     * The code only reacher here if typeflag was not a regular file or a
     * directory, in which case the directory walking should be stopped, or
     * rmdir failed and the function should also stop the walk.
     */
    return -1;
}

bool
deleteDirectory(const std::string& path)
{
    struct stat pathInfo;
    if (stat(path.c_str(), &pathInfo) != 0)
        return true;
    if (!S_ISDIR(pathInfo.st_mode))
        return false;
    return nftw(path.c_str(), deleteDirectoryHelper, MAX_FILE_DESCRIPTORS,
               FTW_DEPTH)
        == 0;
}

bool
deleteFile(const std::string& path)
{
    struct stat pathInfo;
    if (stat(path.c_str(), &pathInfo) != 0)
        return true;
    if (S_ISREG(pathInfo.st_mode))
        return remove(path.c_str()) == 0;
    if (S_ISDIR(pathInfo.st_mode))
        return nftw(path.c_str(), deleteDirectoryHelper, MAX_FILE_DESCRIPTORS,
                   FTW_DEPTH)
            == 0;
    /* The file at path is not a regular file or a directory. */
    return false;
}

bool
ensureDirectoriesExist(const std::string& path)
{
    struct stat pathInfo;
    if (stat(path.c_str(), &pathInfo) != 0) {
        char* pathCopy = strdup(path.c_str());
        if (pathCopy == NULL)
            err(EXIT_FAILURE, NULL);
        char* parentPath = dirname(pathCopy);
        bool parentPathExists = ensureDirectoriesExist(parentPath);
        free(pathCopy);
        if (!parentPathExists)
            return false;
        /*
         * I think that using 0777 here uses the default permissions uses the
         * user's umask, but I might be wrong. I'm also not sure if it's bad
         * directory to use octal here. I think it normally would be to emulate
         * values passed to chmod in C, but I think 777 is guaranteed to be all
         * ones.
         */
        return mkdir(path.c_str(), 0777) == 0;
    }
    return S_ISDIR(pathInfo.st_mode);
}

bool
ensureParentDirectoriesExist(const std::string& path)
{
    char* pathCopy = strdup(path.c_str());
    if (pathCopy == NULL)
        err(EXIT_FAILURE, NULL);
    char* parentPath = dirname(pathCopy);
    bool directoriesExist = ensureDirectoriesExist(parentPath);
    free(pathCopy);
    return directoriesExist;
}

bool
copyRegularFile(
    const std::string& sourcePath, const std::string& destinationPath)
{
    std::ifstream reader(sourcePath, std::ios::binary);
    if (!reader.is_open())
        return false;
    reader.seekg(0, std::ios::end);
    /*
     * TBH I have no idea what type I'm supposed to be using here. The tellg()
     * function returns an std::streampos and I'm not sure at all how that
     * interacts with a streamsize, which is supposed to be the standard for
     * representing IO stuff in C++ I think. This compiles so whatever.
     */
    std::streamsize sourceSize = reader.tellg();
    reader.seekg(0, std::ios::beg);
    if (!ensureParentDirectoriesExist(destinationPath))
        return false;
    std::ofstream writer(destinationPath, std::ios::binary);
    if (!writer.is_open()) {
        reader.close();
        return false;
    }
    char readBuffer[FILE_READ_SIZE];
    std::streamsize remainingBytes = sourceSize;
    /*
     * Another thing I'm not sure of is error checking. There are members of
     * the stream classes that give this stuff, but it's way different than C
     * IO operations where they return an error code or the number of bytes
     * read or written, and I don't want to research this.
     */
    while (remainingBytes > 0) {
        std::streamsize bytesToRead = (remainingBytes < FILE_READ_SIZE)
            ? remainingBytes
            : FILE_READ_SIZE;
        reader.read(readBuffer, bytesToRead);
        writer.write(readBuffer, bytesToRead);
        remainingBytes -= bytesToRead;
    }
    reader.close();
    writer.close();
    return true;
}

bool
copyDirectory(
    const std::string& sourcePath, const std::string& destinationPath)
{
    struct dirent** entries = nullptr;
    int entryCount =
        scandir(sourcePath.c_str(), &entries, returnOne, alphasort);
    if (entryCount == -1)
        return false;
    if (!ensureDirectoriesExist(destinationPath)) {
        free(entries);
        return false;
    }
    for (int i = 0; i < entryCount; i++) {
        std::string entryName = entries[i]->d_name;
        if (entryName == "." || entryName == "..")
            continue;
        std::string sourceEntryPath = sourcePath + "/" + entryName;
        std::string destinationEntryPath = destinationPath + "/" + entryName;
        if (!copyFile(sourceEntryPath, destinationEntryPath)) {
            free(entries);
            return false;
        }
    }
    free(entries);
    return true;
}

bool
copyFile(const std::string& sourcePath, const std::string& destinationPath)
{
    struct stat sourcePathInfo;
    if (stat(sourcePath.c_str(), &sourcePathInfo) != 0)
        return false;
    if (S_ISREG(sourcePathInfo.st_mode))
        return copyRegularFile(sourcePath, destinationPath);
    if (S_ISDIR(sourcePathInfo.st_mode))
        return copyDirectory(sourcePath, destinationPath);
    return false;
}

int
returnOne(const struct dirent* entry)
{
    return 1;
}
} /* namespace dfm */
