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

#include "filecheckeditor.h"

#include <assert.h>
#include <err.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>

namespace gdfm {

FileCheckEditor::FileCheckEditor(Gtk::Window& parent, FileCheckAction* action)
    : Gtk::Dialog("Edit File Check Action", parent, true), action(action)
{
    assert(action != nullptr);

    get_content_area()->add(grid);

    sourcePathLabel.set_text("Source Path:");
    grid.attach(sourcePathLabel, 0, 0, 1, 1);

    sourcePathEntry.set_placeholder_text("Source Path");
    sourcePathEntry.set_text(action->getSourcePath());
    grid.attach(sourcePathEntry, 1, 0, 1, 1);

    destinationPathLabel.set_text("Destination Path:");
    grid.attach(destinationPathLabel, 0, 1, 1, 1);

    destinationPathEntry.set_placeholder_text("Destination Path");
    destinationPathEntry.set_text(action->getDestinationPath());
    grid.attach(destinationPathEntry, 1, 1, 1, 1);

    show_all_children();

    add_button("Ok", Gtk::RESPONSE_OK);
    add_button("Cancel", Gtk::RESPONSE_CANCEL);

    signal_response().connect(
        sigc::mem_fun(*this, &FileCheckEditor::onResponse));
}

void
FileCheckEditor::onResponse(int responseId)
{
    if (responseId != Gtk::RESPONSE_OK)
        return;

    std::string sourcePath = sourcePathEntry.get_text();
    if (sourcePath.length() == 0)
        return;
    action->setSourcePath(sourcePath);

    std::string destinationPath = destinationPathEntry.get_text();
    if (destinationPath.length() > 0) {
        action->setDestinationPath(destinationPath);
        return;
    }

    /*
     * If there was no destination, then take the filename in the source path
     * and append it to the home directory.
     */
    char* sourcePathCopy = strdup(sourcePath.c_str());
    if (sourcePathCopy == nullptr)
        err(EXIT_FAILURE, nullptr);
    char* sourcePathBasename = basename(sourcePathCopy);
    destinationPath = "~/";
    destinationPath += sourcePathBasename;
    action->setDestinationPath(destinationPath);
    free(sourcePathCopy);
}
} /* namespace gdfm */
