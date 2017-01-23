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

#include "removeactioneditor.h"

#include <assert.h>

namespace gdfm {

RemoveActionEditor::RemoveActionEditor(
    Gtk::Window& parent, RemoveAction* action)
    : Gtk::Dialog("Edit Remove Action", parent, true), action(action)
{
    assert(action != nullptr);

    pathLabel.set_text("Path:");
    get_content_area()->add(pathLabel);

    pathEntry.set_placeholder_text("Path");
    pathEntry.set_text(action->getFilePath());
    get_content_area()->add(pathEntry);

    show_all_children();

    add_button("Ok", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(
        sigc::mem_fun(*this, &RemoveActionEditor::onResponse));
}

void
RemoveActionEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;

    std::string filePath = pathEntry.get_text();
    if (filePath.length() > 0)
        action->setFilePath(filePath);
}
} /* namespace gdfm */
