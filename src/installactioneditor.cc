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

#include "installactioneditor.h"

#include <assert.h>

#include "util.h"

namespace gdfm {

InstallActionEditor::InstallActionEditor(
    Gtk::Window& window, InstallAction* action)
    : Gtk::Dialog("Edit Install Action", window, true), action(action)
{
    assert(action != nullptr);

    get_content_area()->add(filenameBox);

    filenameLabel.set_text("Filename:");
    filenameBox.pack_start(filenameLabel, false, false);

    filenameEntry.set_placeholder_text("Filename");
    filenameEntry.set_text(action->getFilename());
    filenameBox.pack_start(filenameEntry, true, true);

    get_content_area()->add(destinationBox);

    destinationLabel.set_text("Destination Directory:");
    destinationBox.pack_start(destinationLabel, false, false);

    destinationEntry.set_placeholder_text("~");
    destinationEntry.set_text(action->getDestinationDirectory());
    destinationBox.pack_start(destinationEntry, true, true);

    get_content_area()->add(installFilenameBox);

    installFilenameLabel.set_text("Install Filename:");
    installFilenameBox.pack_start(installFilenameLabel, false, false);

    installFilenameEntry.set_placeholder_text("Install Filename");
    installFilenameEntry.set_text(action->getInstallFilename());
    installFilenameBox.pack_start(installFilenameEntry, true, true);

    add_button("Ok", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(
        sigc::mem_fun(*this, &InstallActionEditor::onResponse));
}

void
InstallActionEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_CANCEL)
        return;

    std::string filename = filenameEntry.get_text();
    if (filename.length() == 0)
        return;
    action->setFilename(filename);

    std::string destinationDirectory = destinationEntry.get_text();
    if (destinationDirectory.length() > 0)
        action->setDestinationDirectory(destinationDirectory);
    else
        action->setDestinationDirectory(getHomeDirectory());

    std::string installFilename = installFilenameEntry.get_text();
    if (installFilename.length() > 0)
        action->setInstallFilename(installFilename);
    else
        action->setInstallFilename(action->getFilename());
}
} /* namespace gdfm */
