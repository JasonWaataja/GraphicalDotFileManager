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

#include "createmoduledialog.h"

#include <err.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

#include "filecheckaction.h"
#include "installaction.h"
#include "removeaction.h"
#include "util.h"

namespace gdfm {

CreateModuleDialog::CreateModuleDialog(Gtk::Window& parent)
    : Gtk::Dialog("Create Module", parent, true)
{
    set_default_size(400, 300);

    contentBox = Gtk::manage(new Gtk::VBox());
    get_content_area()->add(*contentBox);

    nameBox = Gtk::manage(new Gtk::HBox());
    /* contentBox->pack_start(*nameBox, true, true); */
    get_content_area()->pack_start(*nameBox, false, false);
    nameLabel = Gtk::manage(new Gtk::Label());
    nameLabel->set_text("Name:");
    nameBox->pack_start(*nameLabel, false, false);
    nameEntry = Gtk::manage(new Gtk::Entry());
    nameEntry->set_placeholder_text("Module Name");
    nameBox->pack_start(*nameEntry, true, true);
    filesBox = Gtk::manage(new Gtk::HBox());
    get_content_area()->pack_start(*filesBox, true, true);
    scrollWindow = Gtk::manage(new Gtk::ScrolledWindow());
    filesBox->pack_start(*scrollWindow, true, true);
    filesView = Gtk::manage(new Gtk::TreeView());
    scrollWindow->add(*filesView);
    actionBox = Gtk::manage(new Gtk::VBox());
    filesBox->pack_start(*actionBox, false, false);
    filenameEntry = Gtk::manage(new Gtk::Entry);
    filenameEntry->set_placeholder_text("Filename");
    actionBox->pack_start(*filenameEntry, false, false);
    destinationEntry = Gtk::manage(new Gtk::Entry());
    destinationEntry->set_placeholder_text("~ (Destiatnion)");
    actionBox->pack_start(*destinationEntry, false, false);
    addFileButton = Gtk::manage(new Gtk::Button());
    addFileButton->set_label("Add File");
    actionBox->pack_start(*addFileButton, false, false);
    removeFileButton = Gtk::manage(new Gtk::Button());
    removeFileButton->set_label("Remove");
    actionBox->pack_start(*removeFileButton, false, false);

    addFileButton->signal_clicked().connect(
        sigc::mem_fun(*this, &CreateModuleDialog::onAddFileButtonClicked));

    filesViewSelection = filesView->get_selection();
    filesViewSelection->set_mode(Gtk::SELECTION_MULTIPLE);
    filesViewSelection->signal_changed().connect(sigc::mem_fun(
        *this, &CreateModuleDialog::onFilesViewSelectionChanged));

    removeFileButton->signal_clicked().connect(
        sigc::mem_fun(*this, &CreateModuleDialog::onRemoveFileButtonClicked));

    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    /*
     * It's easier and cleaner to show them all and then hiding specific ones
     * than writing the code to show each on individually.
     */
    show_all_children();
    removeFileButton->hide();

    record.add(filenameColumn);
    record.add(destinationColumn);
    filesList = Gtk::ListStore::create(record);
    filesView->set_model(filesList);
    filesView->append_column("Filename", filenameColumn);
    filesView->append_column("Destination", destinationColumn);
}

CreateModuleDialog::~CreateModuleDialog()
{
}

std::shared_ptr<Module>
CreateModuleDialog::getModule()
{
    std::string name = nameEntry->get_text();
    if (name.length() == 0) {
        Gtk::MessageDialog dialog(*this, "You must enter a name.", false,
            Gtk::MESSAGE_ERROR, Gtk::BUTTONS_OK, true);
        dialog.run();
        return std::shared_ptr<Module>();
    }
    std::shared_ptr<Module> module(new Module(name));
    for (auto i = filesList->children().begin();
         i != filesList->children().end(); i++) {
        Gtk::TreeRow row = *i;
        Glib::ustring filename = row[filenameColumn];
        Glib::ustring destinationDirectory = row[destinationColumn];
        module->addFile(filename, destinationDirectory);
    }
    return module;
}

void
CreateModuleDialog::onAddFileButtonClicked()
{
    std::string filename = filenameEntry->get_text();
    if (filename.length() == 0) {
        Gtk::MessageDialog dialog(*this, "You must enter a filename.", false,
            Gtk::MESSAGE_INFO, Gtk::BUTTONS_OK, true);
        dialog.run();
        return;
    }
    std::string destination = destinationEntry->get_text();
    if (destination.length() == 0) {
        destination = "~";
    }
    Gtk::TreeIter iter = filesList->append();
    Gtk::TreeRow row = *iter;
    row[filenameColumn] = filename;
    row[destinationColumn] = destination;
    filenameEntry->set_text("");
    destinationEntry->set_text("");
}

void
CreateModuleDialog::onFilesViewSelectionChanged()
{
    std::vector<Gtk::TreeModel::Path> selectedFiles =
        filesViewSelection->get_selected_rows();
    if (selectedFiles.size() > 0)
        removeFileButton->show();
    else
        removeFileButton->hide();
}

void
CreateModuleDialog::onRemoveFileButtonClicked()
{
    /*
     * I'm not sure I have to through all this trouble but I think it makes
     * more sense this way. the selected rows are returned as path, and as soon
     * as one is erased, the paths may point to incorrect places. Hence, they
     * are converted to references first which I think move with the item they
     * originally point to.
     */
    std::vector<Gtk::TreePath> selectedFiles =
        filesViewSelection->get_selected_rows();
    if (selectedFiles.size() == 0)
        return;
    std::vector<Gtk::TreeRowReference> rowReferences(selectedFiles.size());
    for (std::vector<Gtk::TreeModel::Path>::size_type i = 0;
         i < selectedFiles.size(); i++)
        rowReferences[i] = Gtk::TreeRowReference(filesList, selectedFiles[i]);

    for (const auto& reference : rowReferences) {
        Gtk::TreePath rowPath = reference.get_path();
        Gtk::TreeIter rowIter = filesList->get_iter(rowPath);
        filesList->erase(rowIter);
    }
}
} /* namespace gdfm */
