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

#include "modulefileeditor.h"

#include <assert.h>

namespace gdfm {

ModuleFileEditor::ModuleFileEditor(Gtk::Window& parent, ModuleFile* file)
    : Gtk::Dialog("Edit File", parent, true), file(file)
{
    assert(file != nullptr);

    get_content_area()->add(filenameBox);

    filenameLabel.set_text("Filename:");
    filenameBox.pack_start(filenameLabel, false, false);

    filenameEntry.set_placeholder_text("Filename");
    filenameEntry.set_text(file->getFilename());
    filenameBox.pack_start(filenameEntry, true, true);

    get_content_area()->add(destinationDirectoryBox);

    destinationDirectoryLabel.set_text("Destination Directory:");
    destinationDirectoryBox.pack_start(
        destinationDirectoryLabel, false, false);

    destinationDirectoryEntry.set_placeholder_text(
        "~ (Destination Directory)");
    destinationDirectoryEntry.set_text(file->getDestinationDirectory());
    destinationDirectoryBox.pack_start(destinationDirectoryEntry, true, true);

    get_content_area()->add(destinationFilenameBox);

    destinationFilenameLabel.set_text("Destination Filename:");
    destinationFilenameBox.pack_start(destinationFilenameLabel, false, false);

    destinationFilenameEntry.set_placeholder_text("Destination Filename");
    destinationFilenameEntry.set_text(file->getFilename());
    destinationFilenameBox.pack_start(destinationFilenameEntry, true, true);

    show_all_children();

    add_button("OK", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(
        sigc::mem_fun(*this, &ModuleFileEditor::onResponse));
}

void
ModuleFileEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;
    std::string filename = filenameEntry.get_text();
    if (filename.length() == 0)
        return;
    std::string destinationDirectory = destinationDirectoryEntry.get_text();
    if (destinationDirectory.length() == 0)
        destinationDirectory = "~";
    std::string destinationFilename = destinationFilenameEntry.get_text();
    if (destinationFilename.length() == 0)
        destinationFilename = filename;
    file->setFilename(filename);
    file->setDestinationDirectory(destinationDirectory);
    file->setDestinationFilename(destinationFilename);
}
} /* namespace gdfm */
