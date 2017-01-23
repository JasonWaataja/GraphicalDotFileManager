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

#ifndef CONFIG_FILE_READER_H
#define CONFIG_FILE_READER_H

#include <err.h>
#include <stdarg.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "command.h"
#include "installaction.h"
#include "messageaction.h"
#include "module.h"
#include "options.h"
#include "readerenvironment.h"
#include "removeaction.h"
#include "shellaction.h"

namespace gdfm {

/*
 * The default name that the program should look for in the source directory as
 * the config file.
 */
const char CONFIG_FILE_NAME[] = "config.dfm";

const char COMMENT_DELIMITER = '#';

class ConfigFileReader {
public:
    ConfigFileReader(const std::string& path);
    /*
     * This one is included to prevent ambiguity when using a string literal.
     */
    ConfigFileReader(const char* path);
    ConfigFileReader(
        const std::string& path, std::shared_ptr<DfmOptions> options);

    const std::string& getPath() const;
    void setPath(const std::string& path);

    /* I'm not sure if this one accessor should be const or not. */
    std::shared_ptr<DfmOptions> getOptions();
    void setOptions(std::shared_ptr<DfmOptions> options);

    const ReaderEnvironment& getEnvironment() const;
    void setEnvironment(const ReaderEnvironment& environment);

    bool isOpen();
    void close();

    /*
     * These functions print errorr messages by passing the argumetns to warnx,
     * and optionally may be passed a line that is printed at the end.
     */
    void errorMessageNoLine(const char* format, ...);
    void vErrorMessageNoLine(const char* format, va_list argumentList);
    void errorMessage(const std::string& line, const char* format, ...);
    void vErrorMessage(
        const std::string& line, const char* format, va_list argumentList);
    /*
     * Read the modules in the given file and write them to the given iterator,
     * which must contain elements of type Module.
     *
     * Returns true on success, false on failure.
     */
    template <class OutputIterator> bool readModules(OutputIterator output);

    /*
     * Adds a command with the given action and given names. It takes a list of
     * null terminated list of C strings. The last argument must be NULL or
     * else something will go wrong.
     */
    void addCommand(std::function<std::shared_ptr<ModuleAction>(
                        const std::vector<std::string>&, ReaderEnvironment&)>
                        createActionFunction,
        const char* firstName, ...);
    void addCommand(std::function<std::shared_ptr<ModuleAction>(
                        const std::vector<std::string>&, ReaderEnvironment&)>
                        createActionFunction,
        Command::ArgumentCheck argumentCheckingType, int expectedArgumentCount,
        const char* firstName, ...);

    static std::string getCurrentDirectory();

    /*
     * These functions create actions based on the given arguments and
     * environment.
     */
    static std::shared_ptr<ModuleAction> createMessageAction(
        const std::vector<std::string>& arguments,
        ReaderEnvironment& environment);

    static std::shared_ptr<ModuleAction> createDependenciesAction(
        const std::vector<std::string>& arguments,
        ReaderEnvironment& environment);

    static std::shared_ptr<ModuleAction> createRemoveAction(
        const std::vector<std::string>& arguments,
        ReaderEnvironment& environment);

    static std::shared_ptr<ModuleAction> createInstallAction(
        const std::vector<std::string>& arguments,
        ReaderEnvironment& environment);

private:
    /* The path to the config file. */
    std::string path;
    /* The reader to open path with. */
    std::ifstream reader;
    /*
     * The options for the program to be used when reading this file. These are
     * meant to be read using getopt and passed to this object.
     *
     * It's a shared pointer because it might be used somewhere else in the
     * program and I don't want to think about memory management.
     */
    std::shared_ptr<DfmOptions> options;
    /*
     * The current environment for the parser. This was added after the options
     * variable above, and should have replaced it but I'm lazy. This holds
     * things like variables if they are ever added, and are an easy way to
     * pass around options.
     */
    ReaderEnvironment environment;
    /*
     * Wheter or not the reader is at the start of the file and currently
     * setting variables.
     */
    bool inVariables = true;
    /*
     * Whether or not the reader is currently reading files to install. Lines
     * are processed as files to intall
     */
    bool inFiles = false;
    /*
     * Whether or not the reader is in a module install which determines what
     * command lines are used for.
     */
    bool inModuleInstall = false;
    /*
     * Whether or not the reader is in a module and whether or not it was
     * changed to an uninstall.
     */
    bool inModuleUninstall = false;
    /*
     * Whether or not the reader is in a module and whether or not it is in an
     * update set of actions.
     */
    bool inModuleUpdate = false;
    /*
     * Pointer to a current module being constructed. Set to nullptr when not
     * currently in a module and is meant to be deleted when the module is
     * copied to the output.
     */
    Module* currentModule = nullptr;
    /*
     * Whether or not the reader should process lines as shell commands to be
     * added to the current module install or uninstall.
     */
    bool inShell = false;
    /* The current action to append shell commands to. */
    ShellAction* currentShellAction = nullptr;
    /*
     * The current line number of the reader, meant to be used with error
     * messages.
     */
    int currentLineNo = 1;
    /*
     * The list of commands, which is checked against when processing a normal
     * command. It looks through these commands in order, so higher priority
     * commands should be first and if two commands share a name the one first
     * will be executed.
     */
    std::vector<Command> commands;

    /*
     * This is the function that determines which commands can be run and what
     * kind of arguments checking they require.
     */
    void addDefaultCommands();
    /*
     * Sets some initial variables that are required for normal functioning to
     * work to sensible defaults.
     */
    void addDefaultVariables();
    /* Equivalent to line.length() == 0. */
    bool isEmptyLine(const std::string& line) const;
    /*
     * Tests if the given line is a comment. A line is a comment if the first
     * character in the line is COMMENT_DELIMITER, or if any of the next
     * expectedIndents characters are COMMENT_DELIMITER. The reason for this is
     * to embed the COMMENT_DELIMITER character in commands and in shell
     * commands.
     *
     * Returns whether or not the given line is a comment.
     */
    bool isComment(
        const std::string& line, unsigned int expectedIndents) const;
    /* Returns the number of '\t' characters at the start of the line. */
    int indentCount(const std::string& line) const;
    /*
     * Gets the expected number of indents based on the current state of the
     * reader. Being in a shell means it expects two, being in a module install
     * or uninstall makes it 1, and anything else makes it 0.
     *
     * Returns the number of expected indentations based on the reader state.
     */
    int getExpectedIndents() const;
    /*
     * Removes indents indents from the start of line and returns it.
     *
     * Returns a new string with indents indents stripped from it.
     */
    std::string stripIndents(const std::string& line, int indents);
    /*
     * Tests to see if line assigns a variable. A line assigns a variable if
     * line begins with no whitespace, the first token is a valid variable
     * name, the second token is an equals sign, and the third is either a
     * normal string or a quoted string.
     *
     * Returns whether or not line is a line that assigns a variable.
     */
    bool isAssignmentLine(const std::string& line);
    bool isAssignmentLine(
        const std::string& line, std::string& name, std::string& value);
    /*
     * Tests to see if the line starts a module and sets moduleName to the name
     * if this is so. These don't check if the line is blank or a comment and
     * process as is because it is assumes the checking was already done.
     *
     * If it does match the module syntax, stores the module name in line. Does
     * not affect line if the line is not a module line.
     *
     * Returns whether or not line matches the syntax for a new module line.
     */
    bool isModuleLine(const std::string& line, std::string& moduleName);
    /*
     * Tests to see if the line starts a module and sets moduleName to the name
     * if this is so. These don't check if the line is blank or a comment and
     * process as is because it is assumes the checking was already done.
     *
     * Returns whether or not line matches the syntax for a new module line.
     */
    bool isModuleLine(const std::string& line);
    /*
     * Tests line to see if it reprsents a line that starts install actions.
     *
     * Returns if the line starts with install: with optional whitespace.
     */
    bool isInstallLine(const std::string& line);
    /*
     * Tests line to see if it represents a line that uninstalls.
     *
     * Returns whether or not line matches the syntax for an uninstall line.
     */
    bool isUninstallLine(const std::string& line);
    /*
     * Tests line to see if it represents a line that starts update actions.
     *
     * Returns if the line starts with update: with optional whitespace.
     */
    bool isUpdateLine(const std::string& line);
    /*
     * Tests commandName to see if it's a command that should activate shell
     * action.
     *
     * Returns if commandName represents a recognized command.
     */
    bool isShellCommand(const std::string& commandName);
    /* Processing commands that affect object state. */
    void addShellAction(const std::string& line);
    /* Behavior changes if install or uninstall. */
    void flushShellAction();
    /*
     * Assumes that line represents an assignment. Calls the correct variable
     * methods on environment that assigns the variable values correctly.
     *
     * Returns true if the line was well formed and the variable was correctly
     * set, false otherwise.
     */
    bool processLineAsAssignment(const std::string& line);
    /*
     * Executes command or starts new shell.
     *
     * Split given line after the given number of indents. The first token must
     * be a word with no spaces. Each remaining token can either be a word on
     * its own, or a string surrounded by double-quote characters. Any double
     * quote characters on the inside will be part of a token. If the command
     * is successfully parsed, then the command is stored in command. The
     * arguments vector is cleared regardless, and any arguments are added to
     * it.
     *
     * Returns true on success, false on failure.
     */
    bool processLineAsCommand(const std::string& line);
    /* Executes command string with given arguments. */
    bool processCommand(const std::string& commandName,
        const std::vector<std::string>& arguments);
    bool processLineAsFile(const std::string& line);

    /*
     * If the reader is in a module install or uninstall, finishes the module
     * and writes it to output, which must be an output iterator with type
     * module. This must be called or there will be a memory leak.
     */
    template <class OutputIterator> void flushModule(OutputIterator output);
    /*
     * Creates a new module with the given name and sets the current module to
     * it. This assumes that the last module has already be flushed, and don't
     * forget to do so.
     */
    void startNewModule(const std::string& name);
    /* Changes to actions representing install actions. */
    void changeToInstall();
    /* Changes to actions representing uninstall actions. */
    void changeToUninstall();
    /* Changes to actions representing update actions. */
    void changeToUpdate();

    /*
     * The main function for processing a config file. Takes a line, and takes
     * the appropriate action based on the content of the line.
     *
     * This function does not increment currentLineNo. Do that in the method
     * that reads line the lines from the file reader.
     *
     * First checks to see if the line is empty or a comment based on the
     * expected indents. It then executes the appropriate action based on the
     * contents of the line, whether that be to add a shell action, flush the
     * current module, or start a new one. Does a lot of error checking and
     * gives warnings for malformed config files.
     *
     * Returns whether or not parsing the line was successful, which would
     * likely fail because of a malformed line.
     */
    template <class OutputIterator>
    bool processLine(const std::string& line, OutputIterator output);

    /*
     * Matches a string entirely to whitespace. This could be implement with a
     * regex or the isspace function, depedning on what you want to count as
     * whitespace.
     *
     * Returns whether the whole string is whitespace, returns false for an
     * empty string.
     */
    static bool isWhiteSpace(const std::string& string);
    static bool isWhiteSpace(const char* string);
    /* The same as above, except for single characters. */
    static bool isWhiteSpace(char c);

    /*
     * Takes the given string containing arguments and extracts the arguments
     * from them according to the rules of quotations. A token may be
     * surrounded by quotes on its own, which allow for whitespace inside.
     * Quotes are allowed in the middle of tokens, but don't make anything
     * literal. Quotes are not allowed as the first or last character of a
     * token. To do that, create a quoted string with an escaped quote
     * character with \".
     *
     * This function takes a string that has been stripped of the command
     * portion. If you don't take out the command portion first, it will return
     * it as an argument.
     *
     * Clears the given vector regardless of any arguments found or any errors
     * encountered. Stores any found arguments in the vector as it goes.
     *
     * In quotations, quote characters are escaped as \", backslash characters
     * are escaped as \\.
     *
     * This used to be done through a regex, and it worked, but I wanted to
     * give more detailed error messages like unclosed quotation mark. It
     * wasn't worth it and I hate this.
     *
     * Returns true on success, false on failure.
     */
    bool splitArguments(
        const std::string& argumentsLine, std::vector<std::string>& arguments);

    bool inModule() const;
    bool isCreatingModuleActions() const;

    /*
     * Sets the given ModuleAction to the correct verbose and interactive flags
     * based on the current options.
     */
    void setModuleActionFlags(std::shared_ptr<ModuleAction> action);
    void setModuleActionFlags(ModuleAction* action);
};

template <class OutputIterator>
bool
ConfigFileReader::readModules(OutputIterator output)
{
    if (!isOpen()) {
        warnx("Attempting to read from non-open file reader");
        return false;
    }

    currentLineNo = 1;
    inVariables = true;
    inFiles = false;
    inModuleInstall = false;
    inModuleUninstall = false;
    inModuleUpdate = -false;
    currentModule = nullptr;
    inShell = false;
    currentShellAction = nullptr;

    bool noErrors = true;
    std::string line;
    /* Don't read a line if processing the last line wasn't successful. */
    while (noErrors && getline(reader, line)) {
        noErrors = processLine<OutputIterator>(line, output);
        if (noErrors)
            currentLineNo++;
    }
    if (inShell)
        flushShellAction();
    if (inModule())
        flushModule(output);
    if (!noErrors)
        errorMessageNoLine(
            "Failed to read config file %s.", getPath().c_str());
    return noErrors;
}

template <class OutputIterator>
bool
ConfigFileReader::processLine(const std::string& line, OutputIterator output)
{
    if (isEmptyLine(line))
        return true;
    int expectedIndents = getExpectedIndents();
    if (isComment(line, expectedIndents))
        return true;

    int indents = indentCount(line);

    if (inVariables) {
        std::string variableName;
        std::string variableValue;
        if (isAssignmentLine(line, variableName, variableValue)) {
            environment.setVariable(variableName, variableValue);
            return true;
        } else
            inVariables = false;
    }
    if (inShell) {
        if (indents >= 2) {
            addShellAction(line);
            return true;
        } else
            flushShellAction();
    }
    if (inFiles) {
        if (indents == 1)
            return processLineAsFile(line);
        else if (indents > 1) {
            errorMessage(line, "Unexpected indentation.");
            return false;
        }
    }
    if (isCreatingModuleActions()) {
        if (indents == 1)
            return processLineAsCommand(line);
        else if (indents > 1) {
            errorMessage(line, "Unexpected indentation.");
            return false;
        }
    }
    if (isInstallLine(line)) {
        if (!inModule()) {
            errorMessage(line, "Install without named module.");
            return false;
        }
        changeToInstall();
        return true;
    }
    if (isUninstallLine(line)) {
        if (!inModule()) {
            errorMessage(line, "Uninstall without named module.");
            return false;
        }
        changeToUninstall();
        return true;
    }
    if (isUpdateLine(line)) {
        if (!inModule()) {
            errorMessage(line, "Update without named module.");
            return false;
        }
        changeToUpdate();
        return true;
    }
    std::string moduleName;
    if (isModuleLine(line, moduleName)) {
        if (inModule())
            flushModule(output);
        startNewModule(moduleName);
        return true;
    }
    errorMessage(line, "Unable to process line.");
    return false;
}

template <class OutputIterator>
void
ConfigFileReader::flushModule(OutputIterator output)
{
    *output = *currentModule;
    delete currentModule;
    inFiles = false;
    inModuleInstall = false;
    inModuleUninstall = false;
    inModuleUpdate = false;
    output++;
}
} /* namespace gdfm */

#endif /* CONFIG_FILE_READER_H */
