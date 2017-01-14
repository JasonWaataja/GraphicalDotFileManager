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

#ifndef GDFMWINDOW_H
#define GDFMWINDOW_H

#include <string>
#include <vector>

#include <gtkmm.h>

#include "gdfmmodelcolumnrecord.h"
#include "module.h"

namespace gdfm {

class GdfmWindow : public Gtk::ApplicationWindow {
public:
    GdfmWindow(
        BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~GdfmWindow();

    /*
     * Reads the modules in the file given by path. Sets the current file to
     * the given config file.
     *
     * Returns if the modules was successfully loaded and the window is
     * currently editing it.
     */
    bool loadFile(const std::string& filePath);
    /*
     * Reads the modules in the file named config.dfm in the given directory
     * and sets the current file to that file.
     *
     * Returns if the modules were successfully loaded and the window is
     * currently editing it.
     */
    bool loadDirectory(const std::string& directoryPath);

    void setModulesViewFromModules();

    const std::string& getCurrentFilePath() const;

private:
    std::string currentFilePath;
    std::vector<gdfm::Module> modules;

    Glib::RefPtr<Gtk::Builder> builder;

    /* Widgets from the glade file. */
    Gtk::Button* addModuleButton;
    Gtk::TreeView* modulesView;

    GdfmModelColumnRecord columns;
    Glib::RefPtr<Gtk::TreeStore> modulesStore;

    /*
     * This method must be called before accessing any of the widgets specified
     * in the builder file. It assigns the pointers for the member widgets of
     * this class to the pointers in the builder. This does not initialize
     * members that are not in the builder file, specifically the modules
     * store.
     */
    void initChildren();
    /*
     * Connects the signals for the widgets to their correct signal handlers.
     * It is recommended to call this before doing substantial work on this
     * class.
     */
    void connectSignals();
    /*
     * Adds the correct actions with the correct names to the window's list of
     * actions. This is so that the actions specified in the builder file for
     * the menu items activate the correct actions.
     */
    void addActions();

    /*
     * Initializes the tree view for the modules. This includes initializing
     * modulesStore.
     */
    void initModulesView();

    /* Signal handlers. */
    void onAddModuleButtonClicked();

    /* Actions for use with bar. */
    void onActionOpenFile();
    void onActionOpenDirectory();
    void onActionSave();
    void onActionSaveAs();
    void onActionQuit();

    void appendModule(const Module& module);
};
} /* namespace gdfm */

#endif /* GDFMWINDOW_H */
