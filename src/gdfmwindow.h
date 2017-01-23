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

#include <memory>
#include <string>
#include <vector>

#include <gtkmm.h>

#include "module.h"

namespace gdfm {

class GdfmWindow : public Gtk::ApplicationWindow {
public:
    GdfmWindow(
        BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder);
    virtual ~GdfmWindow();

    /*
     * This is a type to make testing what type of row is much easier. Before,
     * it would check to see if certain columns were filled and what their
     * content was, but having an extra hidden column with the type makes the
     * row activated signal handler easier to write.
     */
    enum RowType {
        MODULE_ROW,
        MODULE_TYPE_ROW,
        MODULE_FILE_ROW,
        MODULE_ACTION_ROW
    };
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
    void setModulesViewFromModules(const std::vector<Module>& modules);
    std::string getSourceDirectory() const;
    /*
     * If there is no current filename, then prompts the user for if it is okay
     * to use the home directory.
     *
     * Returns true if there is a current filename or if there isn't and the
     * user answers "yes" to the question.
     */
    bool promptContinueIfNoDirectory();
    const std::string& getCurrentFilePath() const;
    /*
     * Creates a dialog to add a new module. If the user accepts, then a smart
     * pointer with the module is returned.
     *
     * Returns a pointer to a new module if the module was created, a null
     * pointer otherwise.
     */
    std::shared_ptr<Module> createModuleDialog();

private:
    std::string currentFilePath;

    Glib::RefPtr<Gtk::Builder> builder;

    /* Widgets from the glade file. */
    Gtk::Button* addModuleButton;
    Gtk::TreeView* modulesView;
    Gtk::Button* installAllModulesButton;
    Gtk::Button* uninstallAllModulesButton;
    Gtk::Button* updateAllModuleButton;
    Gtk::Button* moveUpButton;
    Gtk::Button* moveDownButton;

    /* Tree view related items. */
    Gtk::TreeModelColumnRecord columns;
    Gtk::TreeModelColumn<Glib::ustring> moduleNameColumn;
    Gtk::TreeModelColumn<Glib::ustring> actionNameColumn;
    Gtk::TreeModelColumn<Glib::ustring> fileColumn;
    Gtk::TreeModelColumn<RowType> rowTypeColumn;
    Gtk::TreeModelColumn<std::shared_ptr<Module>> moduleColumn;
    Gtk::TreeModelColumn<std::shared_ptr<ModuleFile>> moduleFileColumn;
    Gtk::TreeModelColumn<std::shared_ptr<ModuleAction>> actionColumn;
    Glib::RefPtr<Gtk::TreeStore> modulesStore;
    Glib::RefPtr<Gtk::TreeSelection> modulesSelection;

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

    /*
     * Creates a set of modules for use with saving. The point of this is that
     * orders in the tree view can be changed and modules will be added and
     * removed without being update in the modules variable.
     *
     * Returns a list of modules represented by the current state of
     * modulesView.
     */
    std::vector<Module> createModulesFromView() const;
    Module createModuleForRow(const Gtk::TreeRow& row) const;
    /*
     * Installs the module and creates a popup if it failed notifying the use.
     *
     * Returns true if the module was successfully installed, false otherwise.
     */
    bool installModuleWithPopups(
        const Module& module, const std::string& sourceDirectory);
    /*
     * Uninstall the module and creates a popup if it failed notifying the use.
     *
     * Returns true if the module was successfully uninstalled, false
     * otherwise.
     */
    bool uninstallModuleWithPopups(
        const Module& module, const std::string& sourceDirectory);
    /*
     * Updates the module and creates a popup if it failed notifying the use.
     *
     * Returns true if the module was successfully updated, false otherwise.
     */
    bool updateModuleWithPopups(
        const Module& module, const std::string& sourceDirectory);
    /*
     * Show the correct buttons in the action area on the right of the view
     * based on the current selection. This needs to be called whenever the
     * selection is changed to keep it in sync.
     *
     * Shows moveUpButton and moveDownButton if there is no action selected,
     * hides them otherwise.
     */
    void updateVisibleButtons();

    /* Signal handlers. */
    void onAddModuleButtonClicked();
    void onModulesViewRowActivated(
        const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
    void onModulesViewButtonPressEvent(GdkEventButton* button);
    void onInstallAllModulesButtonClicked();
    void onUninstallAllModulesButtonClicked();
    void onUpdateAllModulesButtonClicked();
    void onMoveUpButtonClicked();
    void onMoveDownButtonClicked();
    void onModulesSelectionChanged();
    /*
     * These signal handlers are specifically for the popup menu that can
     * be
     * created on the tree view.
     */
    void onAddModuleItemActivated();
    void onModuleEditItemActivated(Gtk::TreeRowReference row);
    void onModuleRemoveItemActivated(Gtk::TreeRowReference row);
    void onModuleAddFileItemActivated(Gtk::TreeRowReference row);
    void onModuleAddInstallActionItemActivated(Gtk::TreeRowReference row);
    void onModuleAddUninstallActionItemActivated(Gtk::TreeRowReference row);
    void onModuleAddUpdateActionItemActivated(Gtk::TreeRowReference row);
    void onModuleInstallItemActivated(Gtk::TreeRowReference row);
    void onModuleUninstallItemActivated(Gtk::TreeRowReference row);
    void onModuleUpdateItemActivated(Gtk::TreeRowReference row);
    void onModuleFileEditItemActivated(Gtk::TreeRowReference row);
    void onModuleFileRemoveItemActivated(Gtk::TreeRowReference row);
    void onModuleActionEditItemActivated(Gtk::TreeRowReference row);
    void onModuleActionRemoveItemActivated(Gtk::TreeRowReference row);

    /* Actions for use with bar. */
    void onActionOpenFile();
    void onActionOpenDirectory();
    void onActionSave();
    void onActionSaveAs();
    void onActionQuit();
    void onActionAbout();

    void appendModule(const Module& module);

    /*
     * Gets the row that has the children representing install actions for
     * the given row that represents a module. Also creates the row if it
     * doesn't exist.
     *
     * Returns a Gtk::TreeRow for the install section if it existed or a new
     * row that was just created if it didn't.
     */
    Gtk::TreeRowReference getInstallRow(const Gtk::TreeRow& moduleRow);
    /* Same as above getInstallRow() with uninstall. */
    Gtk::TreeRowReference getUninstallRow(const Gtk::TreeRow& moduleRow);
    /* Same as getInstallRow() with update. */
    Gtk::TreeRowReference getUpdateRow(const Gtk::TreeRow& moduleRow);
};
} /* namespace gdfm */

#endif /* GDFMWINDOW_H */
