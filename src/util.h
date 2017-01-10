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

#include <dirent.h>
#include <ftw.h>

#include <iostream>
#include <string>

#ifndef UTIL_H
#define UTIL_H

namespace dfm {

/*
 * The maximum number of file descriptors for use with nftw. I have no idea
 * what a good value actually is, but it only affects performance.
 */
const int MAX_FILE_DESCRIPTORS = 30;
/* The size of the buffer to use when reading from a binary file. */
const std::streamsize FILE_READ_SIZE = 1024;
/*
 * Waits for the user to input a yes or no input on the current line. Accepts
 * any string that starts with a "y" or "Y" as true and any string that starts
 * with a "n" or "N" as false.  Prompts the user again if they don't enter a
 * vaild input.
 *
 * Returns true if the user responded with a yes, and false if they responded
 * with a no.
 */
bool getYesOrNo();
/*
 * Outputs prompt on the current line, then waits for the user to input a yes
 * or no answer. Accepts any string that starts with a "y" or "Y" as true and
 * any string that starts with a "n" or "N" as false.
 *
 */
bool getYesOrNo(const std::string& prompt);
/*
 * WARNING: Does not return if the line is a yes or no, just if it has yes or
 * no in it. That information is stored in yesOrNo.
 *
 * Checks to see if the input is vaild and results in yes or no. If it is a
 * valid yes or no string, then yesOrNo is set to the correct result and true
 * is returned, leaves yesOrNo unchanged otherwise and returns false.
 *
 * Returns true if input is a valid yes-or-no input, false otherwise.
 */
bool lineIsYesOrNo(const std::string& input, bool& yesOrNo);
/* Returns the current working directory. */
std::string getCurrentDirectory();
/*
 * Performs shell expansion on the given path. Throws a error if it encounters
 * an errorr or if the string path expands to more than one word.
 */
std::string shellExpandPath(const std::string& path);
/*
 * Returns the current user's home directory. Throws a runtime error when
 * encountering an error.
 */
std::string getHomeDirectory();

/*
 * Determines if the file given by path exists. Exits the program if an error
 * is encountered.
 *
 * Returns whether the file represented by path exists in the filesystem.
 */
bool fileExists(const std::string& path);
/*
 * Determines whether the given path is a regular file. Exits the program if
 * the
 * filetype cannot be determined, which means you should make sure the file
 * exists first.
 *
 * Returns if the file represented by path is a regular file.
 */
bool isRegularFile(const std::string& path);
/*
 * Determines whether the given path is a directory. Exits the program if the
 * filetype cannot be determined, which means you should make sure the file
 * exists first.
 *
 * Returns if the file reprented by path is a directory.
 */
bool isDirectory(const std::string& path);
/*
 * Removes the given regular file from the filesystem. Fails if the path
 * doesn't exist, the path isn't a regular file, or if there was an error
 * removing it.
 *
 * Returns true on success, false on failure.
 */
bool deleteRegularFile(const std::string& path);
/* Helper function for deleteDirectory, passed to ntfw. */
int deleteDirectoryHelper(const char* fpath, const struct stat* sb,
    int typeflag, struct FTW* ftwbuf);
/*
 * Removes the given directory from the filesystem. Fails if the path doesn't
 * exist, the path isn't a directory, or if there was an error removing it.
 * Operates recursively and will delete all the contents of the directory by
 * default.
 *
 * Returns true on success, false on failure.
 */
bool deleteDirectory(const std::string& path);
/*
 * Removes the given regular file or directory from the filesystem. Fails if
 * the path doesn't exist, the path isn't a regular file or directory, or if
 * there was an error removing it.
 */
bool deleteFile(const std::string& path);
/*
 * Checks to see if the given directory given by path exists and all its parent
 * directories exist. Path must be intended to be a directory. If the file at
 * path doesn't exist, or any of its parents exist, then they are created. If,
 * at any point, creating a directory fails or if a parent directory exists and
 * is not a directory, then the function fails.
 *
 * Returns true if the file at path already exists and is a directory or if it
 * was successfully created, false otherwise.
 */
bool ensureDirectoriesExist(const std::string& path);
/*
 * Same as ensureDirectoriesExist() except that it operates on the parent
 * directory of the given path.
 *
 * Returns true if the file at path already exists or if its parent directories
 * were successfully created, false otherwise.
 */
bool ensureParentDirectoriesExist(const std::string& path);
/*
 * Copies the given regular file byte for byte. Fails if the source path
 * doesn't exist, the destination path can't be accessed, or if the process
 * failed. Attempts to create parent directories if they don't exist.
 *
 * Returns true on success, false on failure.
 */
bool copyRegularFile(
    const std::string& sourcePath, const std::string& destinationPath);
/*
 * Copies the contents of the directory at sourcePath and all its children,
 * recursively. Fails if sourcePath couldn't be read as a directory, or if
 * destinationPath couldn't be written to as a directory. Attempts to create
 * parent directories if they don't exist for destinatinPath.
 *
 * Returns true on success, false on failure.
 */
bool copyDirectory(
    const std::string& sourcePath, const std::string& destinationPath);
/*
 * Copies the given file to the path at destinationPath. Works on regular files
 * and directories, and fails if it is not one of those two.
 *
 * Returns true on success, false on failure.
 */
bool copyFile(
    const std::string& sourcePath, const std::string& destinationPath);
/*
 * Function to be used with scandir as a filter that doesn't filter anything.
 *
 * Returns one.
 */
int returnOne(const struct dirent* entry);
} /* namespace dfm */

#endif /* UTIL_H */
