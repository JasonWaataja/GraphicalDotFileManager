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

#ifndef MODULE_ACTION_H
#define MODULE_ACTION_H

#include <stdarg.h>

#include <string>
#include <vector>

#include <gtkmm.h>

namespace gdfm {

const char DEFAULT_ACTION_NAME[] = "generic action";

class ModuleAction {
public:
    ModuleAction();
    ModuleAction(const std::string& name);
    virtual bool performAction() = 0;

    void verboseMessage(const char* format, ...);
    void vVerboseMessage(const char* format, va_list argumentList);

    const std::string& getName() const;
    void setName(const std::string& name);
    bool isVerbose() const;
    void setVerbose(bool verbose);
    bool isInteractive() const;
    void setInteractive(bool interactive);

    virtual void updateName();
    /*
     * This method is a method to open a dialog to edit the given action. This
     * is not meant to be in the DFM version of this file because it requires
     * pulling in gtkmm which is not what I want for the command line version.
     * This method should create a popup with several fields for editing.
     */
    virtual void graphicalEdit(Gtk::Window& parent);
    Gtk::Window* getParent() const;
    void setParent(Gtk::Window* parent);
    /*
     * Creates a list of lines that would create the given command when used in
     * a dfm config file.
     *
     * NOTE: This does not have indentation. The indentation is left to caller
     * of this method when writing to config files.
     *
     * Another thing is that although most of the commands shouldn't generate
     * any indentation, something like a shell command would because it is
     * indented relative to itself.
     *
     * Returns a list of lines that when put into a config file would generate
     * the given command.
     */
    virtual std::vector<std::string> createConfigLines() const;

private:
    std::string name;
    bool verbose = false;
    bool interactive = false;
    Gtk::Window* parent = nullptr;
};
} /* namespace gdfm */

#endif /* MODULE_ACTION_H */
