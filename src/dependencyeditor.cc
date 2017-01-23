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

#include "dependencyeditor.h"

#include <sstream>
#include <string>

#include <assert.h>

namespace gdfm {

DependencyEditor::DependencyEditor(
    Gtk::Window& parent, DependencyAction* action)
    : Gtk::Dialog("Edit Dependency Action", parent, true), action(action)
{
    assert(action != nullptr);

    dependenciesLabel.set_text("Dependencies:");
    /* get_content_area()->add(dependenciesLabel); */
    get_content_area()->pack_start(dependenciesLabel, false, false);

    get_content_area()->pack_start(scrolledWindow);

    dependenciesBuffer = dependenciesView.get_buffer();
    std::ostringstream outputStream;
    for (const auto& dependency : action->getDependencies())
        outputStream << dependency << std::endl;
    dependenciesBuffer->set_text(outputStream.str());
    /* get_content_area()->add(dependenciesView); */
    scrolledWindow.add(dependenciesView);

    show_all_children();

    add_button("Ok", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(
        sigc::mem_fun(*this, &DependencyEditor::onResponse));
}

void
DependencyEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;
    std::string text = dependenciesBuffer->get_text();

    std::istringstream inputStream(text);
    std::string line;
    while (std::getline(inputStream, line)) {
        action->addDependency(line);
    }
}
} /* namespace gdfm */
