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

#include "configfilereader.h"

#include <ctype.h>
#include <stdarg.h>
#include <stdlib.h>

#include <exception>
#include <regex>

#include "dependencyaction.h"
#include "filecheckaction.h"
#include "removeaction.h"
#include "util.h"

namespace gdfm {

ConfigFileReader::ConfigFileReader(const std::string& path)
    : path(path), reader(path)
{
    options = std::shared_ptr<DfmOptions>(new DfmOptions());
    environment = ReaderEnvironment(options);
    addDefaultCommands();
    addDefaultVariables();
}

ConfigFileReader::ConfigFileReader(
    const std::string& path, std::shared_ptr<DfmOptions> options)
    : path(path), reader(path), options(options), environment(options)
{
    addDefaultCommands();
}

ConfigFileReader::ConfigFileReader(const char* path)
    : ConfigFileReader(std::string(path))
{
}

std::shared_ptr<DfmOptions>
ConfigFileReader::getOptions()
{
    return options;
}

void
ConfigFileReader::setOptions(std::shared_ptr<DfmOptions> options)
{
    /*
     * I read the page for operator= on std::shared_ptr on cppreference.com.
     * What I'm worried about is that it will not decrement the reference count
     * of the pointer currently stored in this->options when using the =
     * operator on its own. The cppreference.com page for reset says it does,
     * though, so I'm resetting it first just in case.
     */
    this->options.reset();
    this->options = options;
    environment.setOptions(options);
}

const std::string&
ConfigFileReader::getPath() const
{
    return path;
}

void
ConfigFileReader::setPath(const std::string& path)
{
    this->path = path;
}

const ReaderEnvironment&
ConfigFileReader::getEnvironment() const
{
    return environment;
}

void
ConfigFileReader::setEnvironment(const ReaderEnvironment& environment)
{
    this->environment = environment;
}

bool
ConfigFileReader::isOpen()
{
    return this->reader.is_open();
}

bool
ConfigFileReader::isEmptyLine(const std::string& line) const
{
    return line.length() == 0;
}

bool
ConfigFileReader::isComment(
    const std::string& line, unsigned int expectedIndents) const
{
    std::string::size_type currentIndex = 0;
    for (; currentIndex < expectedIndents + 1 && currentIndex < line.length();
         currentIndex++) {
        if (line[currentIndex] == COMMENT_DELIMITER)
            return true;
        else if (line[currentIndex] != '\t')
            return false;
    }
    if (currentIndex < line.length()
        && line[currentIndex] == COMMENT_DELIMITER)
        return true;

    return false;
}

int
ConfigFileReader::indentCount(const std::string& line) const
{
    std::string::size_type indents = 0;
    /* I could probably use an empty for loop for this. */
    while (indents < line.length() && line[indents] == '\t')
        indents++;
    return indents;
}

int
ConfigFileReader::getExpectedIndents() const
{
    if (inVariables)
        return 0;
    if (inShell)
        return 2;
    if (inModule())
        return 1;
    return 0;
}

bool
ConfigFileReader::isModuleLine(
    const std::string& line, std::string& moduleName)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    /*
     * Match a non-white character at the start, then any more characters and a
     * colon at the end. Capture the line, except the final colon and any
     * whitespace between the colon and the name. The name may include
     * whitespace. There may be trailing whitespace after the colon.
     */
    try {
        std::regex re("^(\\S+(?:\\s+\\S+)*)\\s*:\\s*$");
        std::smatch match;
        if (std::regex_match(line, match, re)) {
            moduleName = match.str(1);
            return true;
        }
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return false;
}

bool
ConfigFileReader::isModuleLine(const std::string& line)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    std::regex re("^(\\S+(?:\\s+\\S+)*)\\s*:\\s*$");
    return std::regex_match(line, re);
}

bool
ConfigFileReader::isInstallLine(const std::string& line)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    std::regex re("^install\\s*:\\s*$");
    return std::regex_match(line, re);
}

bool
ConfigFileReader::isUninstallLine(const std::string& line)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    /*
     * The line must start with uninstall, then there must be a colon, which
     * may be surrounded by whitespace.
     */
    std::regex re("^uninstall\\s*:\\s*$");
    return std::regex_match(line, re);
}

bool
ConfigFileReader::isUpdateLine(const std::string& line)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    std::regex re("^update\\s*:\\s*$");
    return std::regex_match(line, re);
}

bool
ConfigFileReader::isShellCommand(const std::string& commandName)
{
    return commandName == "sh" || commandName == "shell";
}

std::string
ConfigFileReader::stripIndents(const std::string& line, int indents)
{
    std::string newLine = line;
    int indentsToErase = 0;
    for (; indentsToErase < line.length() && indentsToErase < indents
         && newLine[indentsToErase] == '\t';
         indentsToErase++)
        ;
    newLine.erase(0, indentsToErase);
    return newLine;
}

void
ConfigFileReader::addShellAction(const std::string& line)
{
    if (inShell)
        currentShellAction->addCommand(stripIndents(line, 2));
}

void
ConfigFileReader::flushShellAction()
{
    if (inModuleInstall) {
        currentModule->addInstallAction(
            std::shared_ptr<ModuleAction>(currentShellAction));
        inShell = false;
        currentShellAction = nullptr;
    } else if (inModuleUninstall) {
        currentModule->addUninstallAction(
            std::shared_ptr<ModuleAction>(currentShellAction));
        inShell = false;
        currentShellAction = nullptr;
    } else if (inModuleUpdate) {
        currentModule->addUpdateAction(
            std::shared_ptr<ModuleAction>(currentShellAction));
        inShell = false;
        currentShellAction = nullptr;
    }
}

bool
ConfigFileReader::processLineAsCommand(const std::string& line)
{
    std::string localLine = stripIndents(line, 1);
    /*
     * Match a string that's not whitespace at the beginning, which is the
     * command. Capture the command and the rest of the line.
     */
    std::regex commandRe("^(\\S+).*$");
    std::smatch matchResults;
    if (!std::regex_match(localLine, matchResults, commandRe)) {
        errorMessage(line, "No command found.");
        return false;
    }
    std::string command = matchResults.str(1);
    localLine =
        localLine.substr(matchResults.position(1) + matchResults.length(1),
            localLine.length() - matchResults.length(1));
    std::vector<std::string> arguments;
    bool success = splitArguments(localLine, arguments);
    if (!success) {
        errorMessage(line, "Failed to extract arguments.");
        return false;
    }
    if (isShellCommand(command)) {
        inShell = true;
        currentShellAction = new ShellAction;
        /* Match everything after one group of whitespace. */
        std::regex re("^\\s+(.*)$");
        std::smatch match;
        if (std::regex_match(localLine, match, re))
            currentShellAction->addCommand(match.str(1));
        return true;
    }
    return processCommand(command, arguments);
}

bool
ConfigFileReader::processLineAsFile(const std::string& line)
{
    std::string localLine = stripIndents(line, 1);
    std::vector<std::string> arguments;
    if (!splitArguments(line, arguments)) {
        errorMessage(line, "Failed to extract arguments");
        return false;
    }
    int argumentCount = arguments.size();
    if (argumentCount == 1)
        currentModule->addFile(arguments[0]);
    else if (argumentCount == 2)
        currentModule->addFile(arguments[0], arguments[1]);
    else if (argumentCount == 3)
        currentModule->addFile(arguments[0], arguments[1], arguments[2]);
    else {
        errorMessage(line, "Too many arguments to file line.");
        return false;
    }
    return true;
}

bool
ConfigFileReader::processCommand(
    const std::string& commandName, const std::vector<std::string>& arguments)
{
    for (const auto& command : commands) {
        if (command.matchesName(commandName)) {
            std::shared_ptr<ModuleAction> action =
                command.createAction(arguments, environment);
            setModuleActionFlags(action);
            if (inModuleInstall) {
                currentModule->addInstallAction(action);
                return true;
            } else if (inModuleUninstall) {
                currentModule->addUninstallAction(action);
                return true;
            } else if (inModuleUpdate) {
                currentModule->addUpdateAction(action);
                return true;
            } else {
                errorMessageNoLine(
                    "Trying to add action when not in module install, uninstall, or update: \'%s\"",
                    commandName.c_str());
                return false;
            }
        }
    }
    errorMessageNoLine(
        "No matching command for name \"%s\".", commandName.c_str());
    return false;
}

void
ConfigFileReader::startNewModule(const std::string& name)
{
    currentModule = new Module(name);
    inFiles = true;
    inModuleInstall = false;
    inModuleUninstall = false;
    inModuleUpdate = false;
}

void
ConfigFileReader::changeToInstall()
{
    inFiles = false;
    inModuleUninstall = false;
    inModuleUpdate = false;

    inModuleInstall = true;
}

void
ConfigFileReader::changeToUninstall()
{
    inFiles = false;
    inModuleInstall = false;
    inModuleUpdate = false;

    inModuleUninstall = true;
}

void
ConfigFileReader::changeToUpdate()
{
    inFiles = false;
    inModuleInstall = false;
    inModuleUninstall = false;

    inModuleUpdate = true;
}

void
ConfigFileReader::close()
{
    if (inModule())
        warnx("Attempting to close reader while still reading.");
    reader.close();
}

void
ConfigFileReader::addCommand(
    std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
        createActionFunction,
    const char* firstName, ...)
{
    Command command(std::string(firstName), createActionFunction);
    va_list argumentList;
    va_start(argumentList, firstName);
    const char* name = va_arg(argumentList, const char*);
    while (name != NULL) {
        command.addCallableName(std::string(name));
        name = va_arg(argumentList, const char*);
    }
    commands.push_back(command);
    va_end(argumentList);
}

void
ConfigFileReader::addCommand(
    std::function<std::shared_ptr<ModuleAction>(
        const std::vector<std::string>&, ReaderEnvironment&)>
        createActionFunction,
    Command::ArgumentCheck argumentCheckingType, int expectedArgumentCount,
    const char* firstName, ...)
{
    Command command(std::string(firstName), createActionFunction);
    va_list argumentList;
    va_start(argumentList, firstName);
    const char* name = va_arg(argumentList, const char*);
    while (name != NULL) {
        command.addCallableName(std::string(name));
        name = va_arg(argumentList, const char*);
    }
    va_end(argumentList);

    switch (argumentCheckingType) {
    case Command::NO_ARGUMENT_CKECK:
        command.setNoArgumentChecking();
        break;
    case Command::EXACT_COUNT_ARGUMENT_CHECK:
        command.setExactAgumentChecking(expectedArgumentCount);
        break;
    case Command::MINIMUM_COUNT_ARGUMENT_CHECK:
        command.setMinimumCountArgumentCheck(expectedArgumentCount);
        break;
    }

    commands.push_back(command);
}

std::shared_ptr<ModuleAction>
ConfigFileReader::createMessageAction(
    const std::vector<std::string>& arguments, ReaderEnvironment& environment)
{
    return std::shared_ptr<ModuleAction>(new MessageAction(arguments[0]));
}

std::shared_ptr<ModuleAction>
ConfigFileReader::createDependenciesAction(
    const std::vector<std::string>& arguments, ReaderEnvironment& environment)
{
    return std::shared_ptr<DependencyAction>(new DependencyAction(arguments));
}

std::shared_ptr<ModuleAction>
ConfigFileReader::createRemoveAction(
    const std::vector<std::string>& arguments, ReaderEnvironment& environment)
{
    if (arguments.size() == 1) {
        std::shared_ptr<RemoveAction> action(new RemoveAction(arguments[0]));
        return action;
    } else if (arguments.size() == 2) {
        std::shared_ptr<RemoveAction> action(
            new RemoveAction(arguments[0], arguments[1]));
        return action;
    }

    warnx("Too many arguments to create remove action, can only accept two.");
    return std::shared_ptr<ModuleAction>();
}

std::shared_ptr<ModuleAction>
ConfigFileReader::createInstallAction(
    const std::vector<std::string>& arguments, ReaderEnvironment& environment)
{
    InstallAction* action = nullptr;
    std::string installationDirectory =
        shellExpandPath(environment.getVariable("default-directory"));
    if (arguments.size() == 1)
        action = new InstallAction(
            arguments[0], environment.getDirectory(), installationDirectory);
    /* Assume that we are working in the current directory. */
    if (arguments.size() == 2)
        action = new InstallAction(arguments[0], environment.getDirectory(),
            shellExpandPath(arguments[1]));
    if (arguments.size() == 3)
        action = new InstallAction(arguments[0], shellExpandPath(arguments[1]),
            shellExpandPath(arguments[2]));
    if (arguments.size() == 4)
        action = new InstallAction(arguments[0], shellExpandPath(arguments[1]),
            arguments[2], shellExpandPath(arguments[3]));

    if (action == nullptr) {
        warnx(
            "Too many arguments to create an install action, can only accept two to four.");
        return std::shared_ptr<ModuleAction>();
    }
    return std::shared_ptr<ModuleAction>(action);
}

void
ConfigFileReader::addDefaultCommands()
{
    addCommand(&ConfigFileReader::createMessageAction,
        Command::EXACT_COUNT_ARGUMENT_CHECK, 1, "message", "msg", "echo", "m",
        NULL);
    addCommand(&ConfigFileReader::createDependenciesAction,
        Command::NO_ARGUMENT_CKECK, -1, "dependencies", "dep", "depend", NULL);
    addCommand(&ConfigFileReader::createRemoveAction,
        Command::MINIMUM_COUNT_ARGUMENT_CHECK, 1, "remove", "rem", "rm",
        "delete", "uninstall", NULL);
    addCommand(&ConfigFileReader::createInstallAction,
        Command::MINIMUM_COUNT_ARGUMENT_CHECK, 1, "install", "in", "i", NULL);
}

void
ConfigFileReader::addDefaultVariables()
{
    environment.setVariable("default-directory", getHomeDirectory());
}

bool
ConfigFileReader::isWhiteSpace(const std::string& string)
{
    std::regex whiteRe("\\s+");
    return std::regex_match(string, whiteRe);
}

bool
ConfigFileReader::isWhiteSpace(const char* string)
{
    std::regex whiteRe("\\s+");
    return std::regex_match(string, whiteRe);
}

bool
ConfigFileReader::isWhiteSpace(char c)
{
    std::regex whiteRe("\\s");
    char string[] = { c, '\0' };
    return std::regex_match(string, whiteRe);
}

bool
ConfigFileReader::splitArguments(
    const std::string& argumentsLine, std::vector<std::string>& arguments)
{
    arguments.clear();
    /*
     * Most of these could be done with a char lastChar variable, but it's
     * harder for me to think through the logic that way.
     */
    bool inQuotes = false;
    bool inWord = false;
    bool lastCharEscape = false;
    bool lastCharClosingQuote = false;
    bool lastCharQuoteInNonQuoteWord = false;

    std::string currentWord;
    /*
     * I could probably do something to make this loop more readable, but I
     * don't want to. I tried nesting some of the statements but that led
     * to
     * too much indentation.
     */
    for (std::string::size_type i = 0; i < argumentsLine.length(); i++) {
        char currentChar = argumentsLine[i];
        bool isWhite = isWhiteSpace(currentChar);
        if (inWord && inQuotes && currentChar == '"') {
            if (!lastCharEscape) {
                if (currentWord.length() == 0)
                    warnx("Using empty string as argument: \"%s\".",
                        argumentsLine.c_str());
                inQuotes = false;
                inWord = false;
                lastCharClosingQuote = true;
                arguments.push_back(currentWord);
                currentWord.clear();
            } else {
                lastCharEscape = false;
                currentWord += '"';
            }
        } else if (inWord && inQuotes && currentChar == '\\') {
            if (!lastCharEscape)
                lastCharEscape = true;
            else {
                lastCharEscape = false;
                currentWord += '\\';
            }
        } else if (inWord && inQuotes && lastCharEscape) {
            lastCharEscape = false;
            /* It's escaped so don't add it to the word. */
        } else if (inWord && inQuotes) {
            currentWord += currentChar;
        } else if (inWord && currentChar == '"') {
            /* It already failed inQuotes above so this is not in quotes.
             */
            currentWord += '"';
            lastCharQuoteInNonQuoteWord = true;
        } else if (inWord && !isWhite) {
            currentWord += currentChar;
            lastCharQuoteInNonQuoteWord = false;
        } else if (lastCharQuoteInNonQuoteWord) {
            warnx("Quote at end of token: \"%s\".", argumentsLine.c_str());
            return false;
        } else if (inWord) {
            inWord = false;
            arguments.push_back(currentWord);
            lastCharQuoteInNonQuoteWord = false;
            currentWord.clear();
        } else if (lastCharClosingQuote && !isWhite) {
            warnx("Missing space after quoted token: \"%s\".",
                argumentsLine.c_str());
            return false;
        } else if (currentChar == '"') {
            inWord = true;
            inQuotes = true;
            lastCharClosingQuote = false;
        } else if (!isWhite) {
            inWord = true;
            inQuotes = false;
            lastCharClosingQuote = false;
            currentWord += currentChar;
        } else {
            lastCharClosingQuote = false;
        }
    }
    if (inQuotes) {
        warnx("Unclosed quote in word: \"%s\".", argumentsLine.c_str());
        return false;
    }
    if (lastCharQuoteInNonQuoteWord) {
        warnx("Quote at end of token: \"%s\".", argumentsLine.c_str());
        return false;
    }
    if (inWord)
        arguments.push_back(currentWord);
    return true;
}

bool
ConfigFileReader::inModule() const
{
    return inFiles || isCreatingModuleActions();
}

bool
ConfigFileReader::isCreatingModuleActions() const
{
    return inModuleInstall || inModuleUninstall || inModuleUpdate;
}

void
ConfigFileReader::errorMessageNoLine(const char* format, ...)
{
    va_list argumentList;
    va_start(argumentList, format);
    vErrorMessageNoLine(format, argumentList);
    va_end(argumentList);
}

void
ConfigFileReader::vErrorMessageNoLine(const char* format, va_list argumentList)
{
    vwarnx(format, argumentList);
    std::cerr << getPath() << ": line " << currentLineNo << std::endl;
}

void
ConfigFileReader::errorMessage(
    const std::string& line, const char* format, ...)
{
    va_list argumentList;
    va_start(argumentList, format);
    vErrorMessage(line, format, argumentList);
    va_end(argumentList);
}

void
ConfigFileReader::vErrorMessage(
    const std::string& line, const char* format, va_list argumentList)
{
    vwarnx(format, argumentList);
    std::cerr << getPath() << ": line " << currentLineNo << ":" << std::endl;
    std::cerr << line << std::endl;
}

void
ConfigFileReader::setModuleActionFlags(std::shared_ptr<ModuleAction> action)
{
    action->setVerbose(options->verboseFlag);
    action->setInteractive(options->interactiveFlag);
}

void
ConfigFileReader::setModuleActionFlags(ModuleAction* action)
{
    action->setVerbose(options->verboseFlag);
    action->setInteractive(options->interactiveFlag);
}

bool
ConfigFileReader::isAssignmentLine(const std::string& line)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    /*
     * Match a group of non-white characters at the start of a line that also
     * cannot contain a colon characters. Then some whitespace and an equals
     * sign. Then a group of words with optional whitespace in between them and
     * some optionsal space at the end. Even though this method doesn't save
     * the variable name or value, it still has to capture the words here to
     * make sure that it follows the rules of quotations.
     */
    std::regex assignmentRe("^[^\\s:]+\\s+=((?:\\s*\\S+)+)\\s*$");
    std::smatch match;
    if (!std::regex_match(line, match, assignmentRe))
        return false;
    std::string valueWords = match.str(1);
    std::vector<std::string> valueArguments;
    return (splitArguments(valueWords, valueArguments))
        ? valueArguments.size() == 1
        : false;
}

bool
ConfigFileReader::isAssignmentLine(
    const std::string& line, std::string& name, std::string& value)
{
    if (isEmptyLine(line) || isComment(line, 0))
        return false;
    /*
     * Match a group of non-white characters at the start of a line that also
     * cannot contain a colon characters. Then some whitespace and an equals
     * sign. Then a group of words with optional whitespace in between them and
     * some optionsal space at the end. Capture the initial group, as well as
     * the part containing all the words.
     */
    std::regex assignmentRe("^([^\\s:]+)\\s+=((?:\\s*\\S+)+)\\s*$");
    std::smatch match;
    if (!std::regex_match(line, match, assignmentRe))
        return false;
    name = match.str(1);
    std::string valueWords = match.str(2);
    std::vector<std::string> valueArguments;
    if (!splitArguments(valueWords, valueArguments))
        return false;
    if (valueArguments.size() != 1)
        return false;
    value = valueArguments[0];
    return true;
}
} /* namespace gdfm */
