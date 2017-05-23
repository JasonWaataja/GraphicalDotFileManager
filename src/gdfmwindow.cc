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
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include <algorithm>
#include <iostream>

#include "configfilereader.h"
#include "configfilewriter.h"
#include "createmoduledialog.h"
#include "moduleactioneditor.h"
#include "modulefileeditor.h"
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
    updateVisibleButtons();
}

GdfmWindow::~GdfmWindow()
{
}

void
GdfmWindow::initChildren()
{
    builder->get_widget("add_module_button", addModuleButton);
    builder->get_widget("modules_view", modulesView);
    builder->get_widget("install_all_button", installAllModulesButton);
    builder->get_widget("uninstall_all_button", uninstallAllModulesButton);
    builder->get_widget("update_all_button", updateAllModuleButton);
    builder->get_widget("move_up_button", moveUpButton);
    builder->get_widget("move_down_button", moveDownButton);
}

void
GdfmWindow::connectSignals()
{
    addModuleButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onAddModuleButtonClicked));
    modulesView->signal_row_activated().connect(
        sigc::mem_fun(*this, &GdfmWindow::onModulesViewRowActivated));
    modulesView->signal_button_press_event().connect_notify(
        sigc::mem_fun(*this, &GdfmWindow::onModulesViewButtonPressEvent));
    installAllModulesButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onInstallAllModulesButtonClicked));
    uninstallAllModulesButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onUninstallAllModulesButtonClicked));
    updateAllModuleButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onUpdateAllModulesButtonClicked));
    moveUpButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onMoveUpButtonClicked));
    moveDownButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onMoveDownButtonClicked));
    modulesSelection->signal_changed().connect(
        sigc::mem_fun(*this, &GdfmWindow::onModulesSelectionChanged));
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
    this->add_action(
        "about", sigc::mem_fun(*this, &GdfmWindow::onActionAbout));
}

void
GdfmWindow::initModulesView()
{
    columns.add(moduleNameColumn);
    columns.add(actionNameColumn);
    columns.add(fileColumn);
    columns.add(rowTypeColumn);
    columns.add(moduleColumn);
    columns.add(moduleFileColumn);
    columns.add(actionColumn);
    modulesStore = Gtk::TreeStore::create(columns);
    modulesView->set_model(modulesStore);
    modulesView->append_column("Module", moduleNameColumn);
    modulesView->append_column("Files", fileColumn);
    modulesView->append_column("Actions", actionNameColumn);

    modulesSelection = modulesView->get_selection();
    modulesSelection->set_mode(Gtk::SELECTION_SINGLE);
}

std::vector<Module>
GdfmWindow::createModulesFromView() const
{
    std::vector<Module> newModules;
    for (const auto moduleRow : modulesStore->children())
        newModules.push_back(createModuleForRow(moduleRow));
    return newModules;
}

Module
GdfmWindow::createModuleForRow(const Gtk::TreeRow& row) const
{
    std::shared_ptr<Module> storedModule = row[moduleColumn];
    Module newModule;
    newModule.setName(storedModule->getName());
    Gtk::TreeIter childIter = row.children().begin();
    if (childIter == row.children().end())
        return newModule;
    Gtk::TreeRow childRow = *childIter;
    while (childRow[rowTypeColumn] != MODULE_TYPE_ROW) {
        std::shared_ptr<ModuleFile> file = childRow[moduleFileColumn];
        newModule.addFile(file->getFilename(), file->getDestinationDirectory(),
            file->getDestinationFilename());
        childIter++;
        childRow = *childIter;
        if (childIter == row.children().end())
            return newModule;
    }
    /*
     * At this point it the code, the iterator is guaranteed to be a module
     * type row.
     */
    if (childRow[moduleNameColumn] == "Install") {
        for (const auto& actionIter : childRow.children()) {
            Gtk::TreeRow actionRow = *actionIter;
            newModule.addInstallAction(actionRow[actionColumn]);
        }
        childIter++;
        childRow = *childIter;
    }
    if (childRow[moduleNameColumn] == "Uninstall") {
        for (const auto& actionIter : childRow.children()) {
            Gtk::TreeRow actionRow = *actionIter;
            newModule.addUninstallAction(actionRow[actionColumn]);
        }
        childIter++;
        childRow = *childIter;
    }
    if (childRow[moduleNameColumn] == "Update") {
        for (const auto& actionIter : childRow.children()) {
            Gtk::TreeRow actionRow = *actionIter;
            newModule.addUpdateAction(actionRow[actionColumn]);
        }
    }
    return newModule;
}

bool
GdfmWindow::loadFile(const std::string& path)
{
    std::vector<Module> modules;
    ConfigFileReader reader(path);
    bool success = reader.readModules(std::back_inserter(modules));
    if (!success) {
        Gtk::MessageDialog dialog(*this, "Failed to read modules.", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
        return false;
    }
    for (auto &module : modules)
        module.setParent(this);
    currentFilePath = path;
    setModulesViewFromModules(modules);
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
GdfmWindow::setModulesViewFromModules(const std::vector<Module>& modules)
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
    topRow[moduleNameColumn] = module.getName();
    topRow[moduleColumn] = std::shared_ptr<Module>(new Module(module));
    topRow[rowTypeColumn] = MODULE_ROW;

    for (const auto& file : module.getFiles()) {
        Gtk::TreeIter fileIter = modulesStore->append(topRow.children());
        Gtk::TreeRow fileRow = *fileIter;
        fileRow[fileColumn] = file.getFilename();
        fileRow[moduleFileColumn] =
            std::shared_ptr<ModuleFile>(new ModuleFile(file));
        fileRow[rowTypeColumn] = MODULE_FILE_ROW;
    }

    const std::vector<std::shared_ptr<ModuleAction>> installActions =
        module.getInstallActions();
    if (installActions.size() > 0) {
        Gtk::TreeModel::iterator typeIter =
            modulesStore->append(topRow.children());
        Gtk::TreeModel::Row typeRow = *typeIter;
        typeRow[moduleNameColumn] = "Install";
        typeRow[rowTypeColumn] = MODULE_TYPE_ROW;
        for (const auto& action : installActions) {
            Gtk::TreeModel::iterator actionIter =
                modulesStore->append(typeRow.children());
            Gtk::TreeModel::Row actionRow = *actionIter;
            actionRow[actionNameColumn] = action->getName();
            actionRow[actionColumn] = action;
            actionRow[rowTypeColumn] = MODULE_ACTION_ROW;
        }
    }
    const std::vector<std::shared_ptr<ModuleAction>> uninstallActions =
        module.getUninstallActions();
    if (uninstallActions.size() > 0) {
        Gtk::TreeModel::iterator typeIter =
            modulesStore->append(topRow.children());
        Gtk::TreeModel::Row typeRow = *typeIter;
        typeRow[moduleNameColumn] = "Uninstall";
        typeRow[rowTypeColumn] = MODULE_TYPE_ROW;
        for (const auto& action : uninstallActions) {
            Gtk::TreeModel::iterator actionIter =
                modulesStore->append(typeRow.children());
            Gtk::TreeModel::Row actionRow = *actionIter;
            actionRow[actionNameColumn] = action->getName();
            actionRow[actionColumn] = action;
            actionRow[rowTypeColumn] = MODULE_ACTION_ROW;
        }
    }
    const std::vector<std::shared_ptr<ModuleAction>> updateActions =
        module.getUpdateActions();
    if (updateActions.size() > 0) {
        Gtk::TreeModel::iterator typeIter =
            modulesStore->append(topRow.children());
        Gtk::TreeModel::Row typeRow = *typeIter;
        typeRow[moduleNameColumn] = "Update";
        typeRow[rowTypeColumn] = MODULE_TYPE_ROW;
        for (const auto& action : updateActions) {
            Gtk::TreeModel::iterator actionIter =
                modulesStore->append(typeRow.children());
            Gtk::TreeModel::Row actionRow = *actionIter;
            actionRow[actionNameColumn] = action->getName();
            actionRow[actionColumn] = action;
            actionRow[rowTypeColumn] = MODULE_ACTION_ROW;
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
    std::vector<Module> modules = createModulesFromView();
    std::string outputFile = currentFilePath;
    if (outputFile.length() == 0) {
        Gtk::FileChooserDialog dialog(
            *this, "Save", Gtk::FILE_CHOOSER_ACTION_SAVE);
        dialog.set_select_multiple(false);
        dialog.set_current_folder(getHomeDirectory());
        dialog.set_filename("config.dfm");
        dialog.add_button("Select", Gtk::RESPONSE_OK);
        dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
        int response = dialog.run();
        if (response == Gtk::RESPONSE_OK)
            outputFile = dialog.get_filename();
        else
            return;
    }
    ConfigFileWriter writer(outputFile, modules);
    if (!writer.isOpen()) {
        Gtk::MessageDialog dialog(*this,
            "Failed to open file " + outputFile + ".", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
    bool status = writer.writeModules();
    if (!status) {
        Gtk::MessageDialog dialog(*this,
            "Failed to write to file " + outputFile + ".", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
}

void
GdfmWindow::onActionSaveAs()
{
    std::vector<Module> modules = createModulesFromView();
    Gtk::FileChooserDialog dialog(
        *this, "Save As", Gtk::FILE_CHOOSER_ACTION_SAVE);
    dialog.set_select_multiple(false);
    dialog.set_current_folder(getHomeDirectory());
    dialog.set_filename("config.dfm");
    dialog.add_button("Select", Gtk::RESPONSE_OK);
    dialog.add_button("Cancel", Gtk::RESPONSE_CANCEL);
    int response = dialog.run();
    if (response != Gtk::RESPONSE_OK)
        return;
    std::string outputFile = dialog.get_filename();
    ConfigFileWriter writer(outputFile, modules);
    if (!writer.isOpen()) {
        Gtk::MessageDialog dialog(*this,
            "Failed to open file " + outputFile + ".", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
    bool status = writer.writeModules();
    if (!status) {
        Gtk::MessageDialog dialog(*this,
            "Failed to write to file " + outputFile + ".", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
}

void
GdfmWindow::onActionQuit()
{
    close();
}

void
GdfmWindow::onActionAbout()
{
    Gtk::AboutDialog dialog;
    dialog.set_parent(*this);
    dialog.set_program_name("GraphicalDotFileManager");
    dialog.set_version("Version 0.1.5");
    dialog.set_copyright("Copyright 2017 Jason Waataja");
    dialog.set_license_type(Gtk::LICENSE_MIT_X11);
    dialog.set_website(
        "http://github.com/JasonWaataja/GraphicalDotFileManager");
    dialog.set_website_label("website");
    dialog.set_authors(std::vector<Glib::ustring>{
        "Jason Waataja <jasonswaataja@gmail.com>" });
    dialog.run();
}

void
GdfmWindow::onModulesViewRowActivated(
    const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow row = *iter;
    RowType type = row[rowTypeColumn];
    /* I could use a switch statement here, but changing it wasn't worth it. */
    if (type == MODULE_ACTION_ROW) {
        std::shared_ptr<ModuleAction> action = row[actionColumn];
        action->graphicalEdit(*this);
        /* This is just in case the action name changed. */
        row[actionNameColumn] = action->getName();
    } else if (type == MODULE_FILE_ROW) {
        std::shared_ptr<ModuleFile> file = row[moduleFileColumn];
        file->graphicalEdit(*this);
        /* This is also just in case the name changed. */
        row[fileColumn] = file->getFilename();
    }
}

std::shared_ptr<Module>
GdfmWindow::createModuleDialog()
{
    CreateModuleDialog dialog(*this);
    dialog.run();
    return std::shared_ptr<Module>();
}

void
GdfmWindow::onModulesViewButtonPressEvent(GdkEventButton* button)
{
    if (button->type != GDK_BUTTON_PRESS
        || button->button != GDK_BUTTON_SECONDARY)
        return;

    Gtk::TreePath selectedPath;

    bool isOnRow =
        modulesView->get_path_at_pos(button->x, button->y, selectedPath);

    Gtk::Menu* menu = Gtk::manage(new Gtk::Menu());
    /*
     * I'm attaching it here so that if the function ends up returning it will
     * still be deleted because it is attached to the window I think.
     */
    menu->accelerate(*this);
    if (!isOnRow) {
        Gtk::MenuItem* addModuleItem =
            Gtk::manage(new Gtk::MenuItem("Add Module"));
        addModuleItem->signal_activate().connect(
            sigc::mem_fun(*this, &GdfmWindow::onAddModuleItemActivated));
        menu->append(*addModuleItem);
    } else {
        Gtk::TreeIter selectedIter = modulesStore->get_iter(selectedPath);
        Gtk::TreeRow selectedRow = *selectedIter;
        Gtk::TreeRowReference selectedRowReference(modulesStore, selectedPath);
        RowType type = selectedRow[rowTypeColumn];
        auto addMenuItem = [this, &menu, &selectedRowReference](
            const std::string& name,
            const sigc::slot<void, Gtk::TreeRowReference>& slot) {
            Gtk::MenuItem* item = Gtk::manage(new Gtk::MenuItem(name));
            item->signal_activate().connect(
                sigc::bind<Gtk::TreeRowReference>(slot, selectedRowReference));
            menu->append(*item);
        };
        if (type == MODULE_ROW) {
            addMenuItem("Edit",
                sigc::mem_fun(*this, &GdfmWindow::onModuleEditItemActivated));
            addMenuItem(
                "Remove", sigc::mem_fun(*this,
                              &GdfmWindow::onModuleRemoveItemActivated));
            addMenuItem(
                "Add File", sigc::mem_fun(*this,
                                &GdfmWindow::onModuleAddFileItemActivated));
            addMenuItem("Add Install Action",
                sigc::mem_fun(*this,
                    &GdfmWindow::onModuleAddInstallActionItemActivated));
            addMenuItem("Add Uninstall Action",
                sigc::mem_fun(*this,
                    &GdfmWindow::onModuleAddUninstallActionItemActivated));
            addMenuItem("Add Update Action",
                sigc::mem_fun(
                    *this, &GdfmWindow::onModuleAddUpdateActionItemActivated));
            addMenuItem(
                "Install", sigc::mem_fun(*this,
                               &GdfmWindow::onModuleInstallItemActivated));
            addMenuItem(
                "Uninstall", sigc::mem_fun(*this,
                                 &GdfmWindow::onModuleUninstallItemActivated));
            addMenuItem(
                "Update", sigc::mem_fun(*this,
                              &GdfmWindow::onModuleUpdateItemActivated));
        } else if (type == MODULE_FILE_ROW) {
            addMenuItem(
                "Edit", sigc::mem_fun(*this,
                            &GdfmWindow::onModuleFileEditItemActivated));
            addMenuItem(
                "Remove", sigc::mem_fun(*this,
                              &GdfmWindow::onModuleFileRemoveItemActivated));
        } else if (type == MODULE_ACTION_ROW) {
            addMenuItem(
                "Edit", sigc::mem_fun(*this,
                            &GdfmWindow::onModuleActionEditItemActivated));
            addMenuItem(
                "Remove", sigc::mem_fun(*this,
                              &GdfmWindow::onModuleActionRemoveItemActivated));
        } else
            return;
    }
    menu->show_all();
    menu->popup(button->button, button->time);
}

void
GdfmWindow::onAddModuleItemActivated()
{
    CreateModuleDialog dialog(*this);
    int response = dialog.run();
    if (response != Gtk::RESPONSE_OK)
        return;
    std::shared_ptr<Module> module = dialog.getModule();
    if (module)
        appendModule(*module);
}

void
GdfmWindow::onModuleEditItemActivated(Gtk::TreeRowReference row)
{
}

void
GdfmWindow::onModuleRemoveItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    modulesStore->erase(iter);
}

void
GdfmWindow::onModuleAddFileItemActivated(Gtk::TreeRowReference row)
{
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow referencedRow = *iter;

    ModuleFile* file = new ModuleFile();
    ModuleFileEditor editor(*this, file);
    int response = editor.run();
    if (response != Gtk::RESPONSE_OK) {
        delete file;
        return;
    }
    for (auto childIter = referencedRow.children().begin();
         childIter != referencedRow.children().end(); childIter++) {
        Gtk::TreeRow childRow = *childIter;
        if (childRow[rowTypeColumn] == MODULE_TYPE_ROW) {
            Gtk::TreeIter newIter = modulesStore->insert(childIter);
            Gtk::TreeRow newRow = *newIter;
            newRow[fileColumn] = file->getFilename();
            newRow[moduleFileColumn] = std::shared_ptr<ModuleFile>(file);
            newRow[rowTypeColumn] = MODULE_FILE_ROW;
            return;
        }
    }
    Gtk::TreeIter newIter = modulesStore->append(referencedRow.children());
    Gtk::TreeRow newRow = *newIter;
    newRow[fileColumn] = file->getFilename();
    newRow[moduleFileColumn] = std::shared_ptr<ModuleFile>(file);
    newRow[rowTypeColumn] = MODULE_FILE_ROW;
}

void
GdfmWindow::onModuleAddInstallActionItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow referencedRow = *iter;

    ModuleActionEditor editor(*this);
    int response = editor.run();
    if (response != Gtk::RESPONSE_OK)
        return;
    std::shared_ptr<ModuleAction> action = editor.getAction();
    if (!action)
        return;
    Gtk::TreeRowReference installRow = getInstallRow(referencedRow);
    Gtk::TreePath installPath = installRow.get_path();
    Gtk::TreeIter installIter = modulesStore->get_iter(installPath);
    Gtk::TreeIter newIter = modulesStore->append(installIter->children());
    Gtk::TreeRow newRow = *newIter;
    newRow[actionNameColumn] = action->getName();
    newRow[actionColumn] = action;
    newRow[rowTypeColumn] = MODULE_ACTION_ROW;
}

void
GdfmWindow::onModuleAddUninstallActionItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow referencedRow = *iter;

    ModuleActionEditor editor(*this);
    int response = editor.run();
    if (response != Gtk::RESPONSE_OK)
        return;
    std::shared_ptr<ModuleAction> action = editor.getAction();
    if (!action)
        return;
    Gtk::TreeRowReference uninstallRow = getUninstallRow(referencedRow);
    Gtk::TreePath uninstallPath = uninstallRow.get_path();
    Gtk::TreeIter uninstallIter = modulesStore->get_iter(uninstallPath);
    Gtk::TreeIter newIter = modulesStore->append(uninstallIter->children());
    Gtk::TreeRow newRow = *newIter;
    newRow[actionNameColumn] = action->getName();
    newRow[actionColumn] = action;
    newRow[rowTypeColumn] = MODULE_ACTION_ROW;
}

void
GdfmWindow::onModuleAddUpdateActionItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow referencedRow = *iter;

    ModuleActionEditor editor(*this);
    int response = editor.run();
    if (response != Gtk::RESPONSE_OK)
        return;
    std::shared_ptr<ModuleAction> action = editor.getAction();
    if (!action)
        return;
    Gtk::TreeRowReference updateRow = getUpdateRow(referencedRow);
    Gtk::TreePath updatePath = updateRow.get_path();
    Gtk::TreeIter updateIter = modulesStore->get_iter(updatePath);
    Gtk::TreeIter newIter = modulesStore->append(updateIter->children());
    Gtk::TreeRow newRow = *newIter;
    newRow[actionNameColumn] = action->getName();
    newRow[actionColumn] = action;
    newRow[rowTypeColumn] = MODULE_ACTION_ROW;
}

void
GdfmWindow::onModuleFileEditItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow selectedRow = *iter;
    std::shared_ptr<ModuleFile> file = selectedRow[moduleFileColumn];
    if (file)
        file->graphicalEdit(*this);
}

void
GdfmWindow::onModuleFileRemoveItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    modulesStore->erase(iter);
}

void
GdfmWindow::onModuleActionEditItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreeRow selectedRow = *iter;
    std::shared_ptr<ModuleAction> action = selectedRow[actionColumn];
    if (action)
        action->graphicalEdit(*this);
}

void
GdfmWindow::onModuleActionRemoveItemActivated(Gtk::TreeRowReference row)
{
    if (!row.is_valid())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Gtk::TreePath parentPath = modulesStore->get_path(iter->parent());
    modulesStore->erase(iter);
    Gtk::TreeIter parentIter = modulesStore->get_iter(parentPath);
    if (parentIter->children().size() == 0)
        modulesStore->erase(parentIter);
}

Gtk::TreeRowReference
GdfmWindow::getInstallRow(const Gtk::TreeRow& moduleRow)
{
    if (moduleRow[rowTypeColumn] != MODULE_ROW)
        return Gtk::TreeRowReference();
    for (auto iter = moduleRow.children().begin();
         iter != moduleRow.children().end(); iter++) {
        Gtk::TreeRow childRow = *iter;
        if (childRow[rowTypeColumn] == MODULE_TYPE_ROW
            && childRow[moduleNameColumn] == "Install") {
            return Gtk::TreeRowReference(
                modulesStore, modulesStore->get_path(iter));
        }
        if (childRow[rowTypeColumn] == MODULE_TYPE_ROW
            && childRow[moduleNameColumn] != "Uninstall") {
            Gtk::TreeIter newIter = modulesStore->insert(iter);
            Gtk::TreeRow newRow = *newIter;
            newRow[moduleNameColumn] = "Install";
            newRow[rowTypeColumn] = MODULE_TYPE_ROW;
            return Gtk::TreeRowReference(
                modulesStore, modulesStore->get_path(newIter));
        }
    }
    Gtk::TreeIter newIter = modulesStore->append(moduleRow.children());
    Gtk::TreeRow newRow = *newIter;
    newRow[moduleNameColumn] = "Install";
    newRow[rowTypeColumn] = MODULE_TYPE_ROW;
    return Gtk::TreeRowReference(
        modulesStore, modulesStore->get_path(newIter));
}

Gtk::TreeRowReference
GdfmWindow::getUninstallRow(const Gtk::TreeRow& moduleRow)
{
    if (moduleRow[rowTypeColumn] != MODULE_ROW)
        return Gtk::TreeRowReference();
    for (auto iter = moduleRow.children().begin();
         iter != moduleRow.children().end(); iter++) {
        Gtk::TreeRow childRow = *iter;
        if (childRow[rowTypeColumn] == MODULE_TYPE_ROW
            && childRow[moduleNameColumn] == "Uninstall") {
            return Gtk::TreeRowReference(
                modulesStore, modulesStore->get_path(iter));
        }
        if (childRow[rowTypeColumn] == MODULE_TYPE_ROW
            && childRow[moduleNameColumn] == "Update") {
            Gtk::TreeIter newIter = modulesStore->insert(iter);
            Gtk::TreeRow newRow = *newIter;
            newRow[moduleNameColumn] = "Uninstall";
            newRow[rowTypeColumn] = MODULE_TYPE_ROW;
            return Gtk::TreeRowReference(
                modulesStore, modulesStore->get_path(newIter));
        }
    }
    Gtk::TreeIter newIter = modulesStore->append(moduleRow.children());
    Gtk::TreeRow newRow = *newIter;
    newRow[moduleNameColumn] = "Uninstall";
    newRow[rowTypeColumn] = MODULE_TYPE_ROW;
    return Gtk::TreeRowReference(
        modulesStore, modulesStore->get_path(newIter));
}

Gtk::TreeRowReference
GdfmWindow::getUpdateRow(const Gtk::TreeRow& moduleRow)
{
    if (moduleRow[rowTypeColumn] != MODULE_ROW)
        return Gtk::TreeRowReference();
    for (auto iter = moduleRow.children().begin();
         iter != moduleRow.children().end(); iter++) {
        Gtk::TreeRow childRow = *iter;
        if (childRow[rowTypeColumn] == MODULE_TYPE_ROW
            && childRow[moduleNameColumn] == "Update") {
            return Gtk::TreeRowReference(
                modulesStore, modulesStore->get_path(iter));
        }
    }
    Gtk::TreeIter newIter = modulesStore->append(moduleRow.children());
    Gtk::TreeRow newRow = *newIter;
    newRow[moduleNameColumn] = "Update";
    newRow[rowTypeColumn] = MODULE_TYPE_ROW;
    return Gtk::TreeRowReference(
        modulesStore, modulesStore->get_path(newIter));
}

void
GdfmWindow::onInstallAllModulesButtonClicked()
{
    if (!promptContinueIfNoDirectory())
        return;
    std::vector<Module> modules = createModulesFromView();
    std::string sourceDirectory = getSourceDirectory();
    for (const auto& module : modules) {
        if (!installModuleWithPopups(module, sourceDirectory))
            return;
    }
}

void
GdfmWindow::onUninstallAllModulesButtonClicked()
{
    if (!promptContinueIfNoDirectory())
        return;
    std::vector<Module> modules = createModulesFromView();
    std::string sourceDirectory = getSourceDirectory();
    for (const auto& module : modules) {
        if (!uninstallModuleWithPopups(module, sourceDirectory))
            return;
    }
}

void
GdfmWindow::onUpdateAllModulesButtonClicked()
{
    if (!promptContinueIfNoDirectory())
        return;
    std::vector<Module> modules = createModulesFromView();
    std::string sourceDirectory = getSourceDirectory();
    for (const auto& module : modules) {
        if (!updateModuleWithPopups(module, sourceDirectory))
            return;
    }
}

std::string
GdfmWindow::getSourceDirectory() const
{
    if (currentFilePath.length() == 0)
        return getHomeDirectory();
    char* filePathCopy = strdup(currentFilePath.c_str());
    if (filePathCopy == nullptr)
        err(EXIT_FAILURE, nullptr);
    std::string sourceDirectory = dirname(filePathCopy);
    free(filePathCopy);
    return sourceDirectory;
}

bool
GdfmWindow::promptContinueIfNoDirectory()
{
    if (currentFilePath.length() > 0)
        return true;
    Gtk::MessageDialog dialog(*this,
        "No directory associated with the current file, use the home directory?",
        false, Gtk::MESSAGE_QUESTION, Gtk::BUTTONS_YES_NO, true);
    int response = dialog.run();
    return response == Gtk::RESPONSE_YES;
}

void
GdfmWindow::onModuleInstallItemActivated(Gtk::TreeRowReference row)
{
    if (!promptContinueIfNoDirectory())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Module module = createModuleForRow(*iter);
    installModuleWithPopups(module, getSourceDirectory());
}

void
GdfmWindow::onModuleUninstallItemActivated(Gtk::TreeRowReference row)
{
    if (!promptContinueIfNoDirectory())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Module module = createModuleForRow(*iter);
    uninstallModuleWithPopups(module, getSourceDirectory());
}

void
GdfmWindow::onModuleUpdateItemActivated(Gtk::TreeRowReference row)
{
    if (!promptContinueIfNoDirectory())
        return;
    Gtk::TreePath path = row.get_path();
    Gtk::TreeIter iter = modulesStore->get_iter(path);
    Module module = createModuleForRow(*iter);
    updateModuleWithPopups(module, getSourceDirectory());
}

bool
GdfmWindow::installModuleWithPopups(
    const Module& module, const std::string& sourceDirectory)
{
    bool status = module.install(sourceDirectory);
    if (!status) {
        Gtk::MessageDialog dialog(*this,
            "Failed to install module " + module.getName(), false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
    return status;
}

bool
GdfmWindow::uninstallModuleWithPopups(
    const Module& module, const std::string& sourceDirectory)
{
    bool status = module.uninstall(sourceDirectory);
    if (!status) {
        Gtk::MessageDialog dialog(*this,
            "Failed to uninstall module " + module.getName(), false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
    return status;
}

bool
GdfmWindow::updateModuleWithPopups(
    const Module& module, const std::string& sourceDirectory)
{
    bool status = module.update(sourceDirectory);
    if (!status) {
        Gtk::MessageDialog dialog(*this,
            "Failed to update module " + module.getName(), false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
    }
    return status;
}

void
GdfmWindow::onModulesSelectionChanged()
{
    updateVisibleButtons();
}

void
GdfmWindow::updateVisibleButtons()
{
    Gtk::TreeIter selectedIter = modulesSelection->get_selected();
    bool visibility = true;
    if (!modulesStore->iter_is_valid(selectedIter))
        visibility = false;
    else {
        Gtk::TreeRow selectedRow = *selectedIter;
        visibility = selectedRow[rowTypeColumn] == MODULE_ACTION_ROW;
    }
    moveUpButton->set_visible(visibility);
    moveDownButton->set_visible(visibility);
}

void
GdfmWindow::onMoveUpButtonClicked()
{
    Gtk::TreeIter selectedIter = modulesSelection->get_selected();
    if (!modulesStore->iter_is_valid(selectedIter))
        return;
    Gtk::TreeRow selectedRow = *selectedIter;
    if (selectedRow[rowTypeColumn] != MODULE_ACTION_ROW)
        return;
    Gtk::TreePath startPath = modulesStore->get_path(selectedIter);
    Gtk::TreePath endPath = startPath;
    if (!endPath.prev())
        return;
    modulesStore->move(
        modulesStore->get_iter(startPath), modulesStore->get_iter(endPath));
}

void
GdfmWindow::onMoveDownButtonClicked()
{
    Gtk::TreeIter selectedIter = modulesSelection->get_selected();
    if (!modulesStore->iter_is_valid(selectedIter))
        return;
    Gtk::TreeRow selectedRow = *selectedIter;
    if (selectedRow[rowTypeColumn] != MODULE_ACTION_ROW)
        return;
    Gtk::TreePath startPath = modulesStore->get_path(selectedIter);
    Gtk::TreePath endPath = startPath;
    endPath.next();
    Gtk::TreeIter endIter = modulesStore->get_iter(endPath);
    if (!modulesStore->iter_is_valid(endIter))
        return;
    /*
     * I wanted to use a move function here, and I have before when using GTK+.
     * However, the C api has a move_after function which I used for move
     * functions like these but I don't think gtkmm has it. Since it's only
     * moving one row, though, a swap works.
     */
    modulesStore->iter_swap(modulesStore->get_iter(startPath), endIter);
}
} /* namespace gdfm */
