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

#include "messageeditor.h"

#include <assert.h>

namespace gdfm {

MessageEditor::MessageEditor(Gtk::Window& parent, MessageAction* action)
    : Gtk::Dialog("Edit Message", parent, true), action(action)
{
    assert(action != nullptr);

    Gtk::Grid* grid = Gtk::manage(new Gtk::Grid());
    get_content_area()->add(*grid);
    Gtk::Label* nameLabel = Gtk::manage(new Gtk::Label());
    nameLabel->set_text("Name:");
    grid->attach(*nameLabel, 0, 0, 1, 1);
    nameEntry = Gtk::manage(new Gtk::Entry());
    nameEntry->set_placeholder_text("Name");
    nameEntry->set_text(action->getName());
    grid->attach(*nameEntry, 1, 0, 1, 1);
    Gtk::Label* messageLabel = Gtk::manage(new Gtk::Label());
    messageLabel->set_text("Message:");
    grid->attach(*messageLabel, 0, 1, 1, 1);
    messageEntry = Gtk::manage(new Gtk::Entry());
    messageEntry->set_placeholder_text("Message");
    messageEntry->set_text(action->getMessage());
    grid->attach(*messageEntry, 1, 1, 1, 1);

    show_all_children();

    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(
        sigc::mem_fun(*this, &MessageEditor::onResponse));
}

void
MessageEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;
    std::string name = nameEntry->get_text();
    std::string message = messageEntry->get_text();
    if (name.length() == 0 || message.length() == 0)
        return;
    action->setName(name);
    action->setMessage(message);
}
} /* namespace gdfm */
