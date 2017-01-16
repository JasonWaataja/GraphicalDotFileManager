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

#include "shelleditor.h"

#include <assert.h>

namespace gdfm {

ShellEditor::ShellEditor(Gtk::Window& parent, ShellAction* action)
    : Gtk::Dialog("Edit Shell Commands", parent, true), action(action)
{
    assert(action != nullptr);

    set_default_size(300, 300);

    Gtk::HBox* nameBox = Gtk::manage(new Gtk::HBox());
    get_content_area()->pack_start(*nameBox, false, false);

    Gtk::Label* nameLabel = Gtk::manage(new Gtk::Label());
    nameLabel->set_text("Name:");
    nameBox->pack_start(*nameLabel, false, false);

    nameEntry = Gtk::manage(new Gtk::Entry());
    nameEntry->set_placeholder_text("Name");
    nameEntry->set_text(action->getName());
    nameBox->pack_start(*nameEntry, true, true);

    Gtk::HBox* commandsBox = Gtk::manage(new Gtk::HBox());
    get_content_area()->pack_start(*commandsBox, true, true);

    commandsView = Gtk::manage(new Gtk::TreeView());
    commandsBox->pack_start(*commandsView, true, true);

    Gtk::VBox* actionsBox = Gtk::manage(new Gtk::VBox());
    commandsBox->pack_start(*actionsBox, false, false);

    commandEntry = Gtk::manage(new Gtk::Entry);
    commandEntry->set_placeholder_text("Command");
    actionsBox->pack_start(*commandEntry, false, false);

    Gtk::Button* addCommandButton = Gtk::manage(new Gtk::Button());
    addCommandButton->set_label("Add Command");
    actionsBox->pack_start(*addCommandButton, false, false);

    removeCommandButton = Gtk::manage(new Gtk::Button());
    removeCommandButton->set_label("Remove Command");
    actionsBox->pack_start(*removeCommandButton, false, false);

    show_all_children();
    removeCommandButton->hide();

    columns.add(commandColumn);
    commandsStore = Gtk::ListStore::create(columns);
    commandsView->set_model(commandsStore);
    commandsView->append_column("Command", commandColumn);
    commandsSelection = commandsView->get_selection();
    commandsSelection->set_mode(Gtk::SELECTION_MULTIPLE);

    for (const auto& command : action->getShellCommands()) {
        Gtk::TreeIter iter = commandsStore->append();
        Gtk::TreeRow row = *iter;
        row[commandColumn] = command;
    }

    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    commandsSelection->signal_changed().connect(
        sigc::mem_fun(*this, &ShellEditor::onCommandsSelectionChanged));
    addCommandButton->signal_clicked().connect(
        sigc::mem_fun(*this, &ShellEditor::onAddCommandButtonClicked));
    removeCommandButton->signal_clicked().connect(
        sigc::mem_fun(*this, &ShellEditor::onRemoveCommandButtonClicked));
    signal_response().connect(sigc::mem_fun(*this, &ShellEditor::onResponse));
}

void
ShellEditor::onCommandsSelectionChanged()
{
    std::vector<Gtk::TreePath> selected =
        commandsSelection->get_selected_rows();
    if (selected.size() > 0)
        removeCommandButton->show();
    else
        removeCommandButton->hide();
}

void
ShellEditor::onRemoveCommandButtonClicked()
{
    std::vector<Gtk::TreePath> selected =
        commandsSelection->get_selected_rows();
    if (selected.size() == 0)
        return;
    std::vector<Gtk::TreeRowReference> selectedReferences(selected.size());
    for (std::vector<Gtk::TreePath>::size_type i = 0; i < selected.size(); i++)
        selectedReferences[i] =
            Gtk::TreeRowReference(commandsStore, selected[i]);

    for (const auto& reference : selectedReferences) {
        Gtk::TreePath rowPath = reference.get_path();
        Gtk::TreeIter rowIter = commandsStore->get_iter(rowPath);
        commandsStore->erase(rowIter);
    }
}

void
ShellEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;

    std::string name = nameEntry->get_text();
    if (name.length() == 0)
        return;
    std::vector<std::string> commands;
    /*
     * I probably shouldn't use auto here, but it makes it much easier for me
     * to read and write this particular section.
     */
    for (auto i = commandsStore->children().begin();
         i != commandsStore->children().end(); i++) {
        Gtk::TreeRow row = *i;
        Glib::ustring command = row[commandColumn];
        commands.push_back(command);
    }
    action->setName(name);
    action->setShellCommands(commands);
}

void
ShellEditor::onAddCommandButtonClicked()
{
    std::string command = commandEntry->get_text();
    if (command.length() == 0)
        return;
    Gtk::TreeIter iter = commandsStore->append();
    Gtk::TreeRow row = *iter;
    row[commandColumn] = command;
    commandEntry->set_text("");
}
} /* namespace gdfm */
