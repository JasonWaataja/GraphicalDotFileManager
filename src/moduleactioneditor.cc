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

#include "moduleactioneditor.h"

#include "dependencyaction.h"
#include "dependencyeditor.h"
#include "filecheckaction.h"
#include "filecheckeditor.h"
#include "installaction.h"
#include "installactioneditor.h"
#include "messageaction.h"
#include "messageeditor.h"
#include "removeaction.h"
#include "removeactioneditor.h"
#include "shellaction.h"
#include "shelleditor.h"

namespace gdfm {

ModuleActionEditor::ModuleActionEditor(Gtk::Window& parent)
    : Gtk::Dialog("Create a Module Action", parent, true)
{
    typeBox.append("Message Action");
    typeBox.append("Shell Action");
    typeBox.append("Install Action");
    typeBox.append("Remove Action");
    typeBox.append("File Check Action");
    typeBox.append("Dependency Action");
    typeBox.set_active(0);
    get_content_area()->add(typeBox);

    createActionButton.set_label("Create Action");
    get_content_area()->add(createActionButton);

    show_all_children();


    createActionButton.signal_clicked().connect(sigc::mem_fun(
        *this, &ModuleActionEditor::onCreateActionButtonClicked));
}

void
ModuleActionEditor::onCreateActionButtonClicked()
{
    std::string selected = typeBox.get_active_text();
    if (selected == "Message Action") {
        std::shared_ptr<MessageAction> action(new MessageAction());
        MessageEditor editor(*this, action.get());
        if (editor.run() == Gtk::RESPONSE_OK) {
            this->action = action;
            response(Gtk::RESPONSE_OK);
        }
    } else if (selected == "Shell Action") {
        std::shared_ptr<ShellAction> action(new ShellAction());
        ShellEditor editor(*this, action.get());
        if (editor.run() == Gtk::RESPONSE_OK) {
            this->action = action;
            response(Gtk::RESPONSE_OK);
        }
    } else if (selected == "Install Action") {
        std::shared_ptr<InstallAction> action(new InstallAction());
        InstallActionEditor editor(*this, action.get());
        if (editor.run() == Gtk::RESPONSE_OK) {
            this->action = action;
            response(Gtk::RESPONSE_OK);
        }
    } else if (selected == "Remove Action") {
        std::shared_ptr<RemoveAction> action(new RemoveAction());
        RemoveActionEditor editor(*this, action.get());
        if (editor.run() == Gtk::RESPONSE_OK) {
            this->action = action;
            response(Gtk::RESPONSE_OK);
        }
    } else if (selected == "File Check Action") {
        std::shared_ptr<FileCheckAction> action(new FileCheckAction());
        FileCheckEditor editor(*this, action.get());
        if (editor.run() == Gtk::RESPONSE_OK) {
            this->action = action;
            response(Gtk::RESPONSE_OK);
        }
    } else if (selected == "Dependency Action") {
        std::shared_ptr<DependencyAction> action(new DependencyAction);
        DependencyEditor editor(*this, action.get());
        if (editor.run() == Gtk::RESPONSE_OK) {
            this->action = action;
            response(Gtk::RESPONSE_OK);
        }
    }
}

std::shared_ptr<ModuleAction>
ModuleActionEditor::getAction()
{
    return action;
}
} /* namespace gdfm  */
