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

#include "gdfmwindow.h"

#include <sys/stat.h>

#include <err.h>
#include <stdlib.h>

#include <algorithm>
#include <iostream>

#include "configfilereader.h"
#include "createmoduledialog.h"
#include "util.h"

namespace gdfm {

GdfmWindow::GdfmWindow(
    BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
    : Gtk::ApplicationWindow(cobject), builder(builder)
{
    initChildren();
    addActions();
    initModulesView();
    connectSignals();
}

GdfmWindow::~GdfmWindow()
{
}

void
GdfmWindow::initChildren()
{
    builder->get_widget("add_module_button", addModuleButton);
    builder->get_widget("modules_view", modulesView);
}

void
GdfmWindow::connectSignals()
{
    addModuleButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onAddModuleButtonClicked));
    modulesView->signal_row_activated().connect(
        sigc::mem_fun(*this, &GdfmWindow::onModulesViewRowActivated));
}

void
GdfmWindow::addActions()
{
    this->add_action(
        "open-file", sigc::mem_fun(*this, &GdfmWindow::onActionOpenFile));
    this->add_action("open-directory",
        sigc::mem_fun(*this, &GdfmWindow::onActionOpenDirectory));
    this->add_action("save", sigc::mem_fun(*this, &GdfmWindow::onActionSave));
    this->add_action(
        "save-as", sigc::mem_fun(*this, &GdfmWindow::onActionSaveAs));
    this->add_action("quit", sigc::mem_fun(*this, &GdfmWindow::onActionQuit));
}

void
GdfmWindow::initModulesView()
{
    modulesStore = Gtk::TreeStore::create(columns);
    modulesView->set_model(modulesStore);
    modulesView->append_column("Module", columns.moduleNameColumn);
    modulesView->append_column("Files", columns.fileColumn);
    modulesView->append_column("Actions", columns.actionNameColumn);

    modulesSelection = modulesView->get_selection();
    modulesSelection->set_mode(Gtk::SELECTION_SINGLE);
}

bool
GdfmWindow::loadFile(const std::string& path)
{
    ConfigFileReader reader(path);
    bool success = reader.readModules(std::back_inserter(modules));
    if (!success) {
        Gtk::MessageDialog dialog(*this, "Failed to read modules.", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
        return false;
    }
    currentFilePath = path;
    setModulesViewFromModules();
    return true;
}

bool
GdfmWindow::loadDirectory(const std::string& path)
{
    std::string filePath = path + "/" + "config.dfm";
    struct stat info;
    if (stat(filePath.c_str(), &info) != 0) {
        std::string message = "Failed to find config file " + path + ".";
        Gtk::MessageDialog dialog(*this, message.c_str(), false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
        return false;
    }
    loadFile(filePath);
    return true;
}

void
GdfmWindow::setModulesViewFromModules()
{
    for (const auto& module : modules) {
        appendModule(module);
    }
}

void
GdfmWindow::appendModule(const Module& module)
{
    Gtk::TreeModel::iterator topIter = modulesStore->append();
    Gtk::TreeModel::Row topRow = *topIter;
    topRow[columns.moduleNameColumn] = module.getName();
    topRow[columns.moduleColumn] = std::shared_ptr<Module>(new Module(module));

    for (const auto& file : module.getFiles()) {
        Gtk::TreeIter fileIter = modulesStore->append(topRow.children());
        Gtk::TreeRow fileRow = *fileIter;
        fileRow[columns.fileColumn] = file.getFilename();
    }

    const std::vector<std::shared_ptr<ModuleAction>> installActions =
        module.getInstallActions();
    if (installActions.size() > 0) {
        Gtk::TreeModel::iterator typeIter =
            modulesStore->append(topRow.children());
        Gtk::TreeModel::Row typeRow = *typeIter;
        typeRow[columns.moduleNameColumn] = "Install";
        for (const auto& action : installActions) {
            Gtk::TreeModel::iterator actionIter =
                modulesStore->append(typeRow.children());
            Gtk::TreeModel::Row actionRow = *actionIter;
            actionRow[columns.actionNameColumn] = action->getName();
            actionRow[columns.actionColumn] = action;
        }
    }
    const std::vector<std::shared_ptr<ModuleAction>> uninstallActions =
        module.getUninstallActions();
    if (uninstallActions.size() > 0) {
        Gtk::TreeModel::iterator typeIter =
            modulesStore->append(topRow.children());
        Gtk::TreeModel::Row typeRow = *typeIter;
        typeRow[columns.moduleNameColumn] = "Uninstall";
        for (const auto& action : uninstallActions) {
            Gtk::TreeModel::iterator actionIter =
                modulesStore->append(typeRow.children());
            Gtk::TreeModel::Row actionRow = *actionIter;
            actionRow[columns.actionNameColumn] = action->getName();
            actionRow[columns.actionColumn] = action;
        }
    }
    const std::vector<std::shared_ptr<ModuleAction>> updateActions =
        module.getUpdateActions();
    if (updateActions.size() > 0) {
        Gtk::TreeModel::iterator typeIter =
            modulesStore->append(topRow.children());
        Gtk::TreeModel::Row typeRow = *typeIter;
        typeRow[columns.moduleNameColumn] = "Update";
        for (const auto& action : updateActions) {
            Gtk::TreeModel::iterator actionIter =
                modulesStore->append(typeRow.children());
            Gtk::TreeModel::Row actionRow = *actionIter;
            actionRow[columns.actionNameColumn] = action->getName();
            actionRow[columns.actionColumn] = action;
        }
    }
}

void
GdfmWindow::onAddModuleButtonClicked()
{
    CreateModuleDialog dialog(*this);
    int response = dialog.run();
    if (response == Gtk::RESPONSE_OK) {
        std::shared_ptr<Module> module = dialog.getModule();
        if (module)
            appendModule(*module);
    }
}

void
GdfmWindow::onActionOpenFile()
{
    Gtk::FileChooserDialog dialog(
        *this, "Choose a File", Gtk::FILE_CHOOSER_ACTION_OPEN);
    dialog.set_modal(true);
    dialog.set_select_multiple(false);

    dialog.add_button("Select", Gtk::RESPONSE_OK);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

    dialog.set_current_folder(getHomeDirectory());

    int response = dialog.run();
    if (response == Gtk::RESPONSE_OK)
        loadFile(dialog.get_filename());
}

void
GdfmWindow::onActionOpenDirectory()
{
    Gtk::FileChooserDialog dialog(
        *this, "Select Folder", Gtk::FILE_CHOOSER_ACTION_SELECT_FOLDER);
    dialog.set_modal(true);
    dialog.set_select_multiple(false);

    dialog.add_button("Select", Gtk::RESPONSE_OK);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);

    dialog.set_current_folder(getHomeDirectory());

    int response = dialog.run();
    if (response == Gtk::RESPONSE_OK)
        loadDirectory(dialog.get_filename());
}

void
GdfmWindow::onActionSave()
{
}

void
GdfmWindow::onActionSaveAs()
{
}

void
GdfmWindow::onActionQuit()
{
    close();
}

void
GdfmWindow::onModulesViewRowActivated(
    const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
    Gtk::MessageDialog dialog(*this, "Selection changed", false,
        Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
    dialog.run();
}

std::shared_ptr<Module>
GdfmWindow::createModuleDialog()
{
    CreateModuleDialog dialog(*this);
    dialog.run();
    return std::shared_ptr<Module>();
}
} /* namespace gdfm */
