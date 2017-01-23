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

#include <sstream>
#include <string>

namespace gdfm {

ShellEditor::ShellEditor(Gtk::Window& parent, ShellAction* action)
    : Gtk::Dialog("Edit Shell Commands", parent, true), action(action)
{
    assert(action != nullptr);

    commandsLabel.set_text("Commands:");
    get_content_area()->pack_start(commandsLabel, false, false);

    get_content_area()->pack_start(scrolledWindow, true, true);

    commandsBuffer = commandsView.get_buffer();
    std::ostringstream outputStream;
    for (const auto& command : action->getShellCommands())
        outputStream << command << std::endl;
    commandsBuffer->set_text(outputStream.str());
    scrolledWindow.add(commandsView);

    show_all_children();

    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(sigc::mem_fun(*this, &ShellEditor::onResponse));
}

void
ShellEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;

    std::vector<std::string> commands;
    std::istringstream reader(commandsBuffer->get_text());
    std::string line;
    while (std::getline(reader, line))
        commands.push_back(line);
    action->setShellCommands(commands);
}
} /* namespace gdfm */
