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

#ifndef INSTALL_ACTION_EDITOR_H
#define INSTALL_ACTION_EDITOR_H

#include <gtkmm.h>

#include "installaction.h"

namespace gdfm {

class InstallActionEditor : public Gtk::Dialog {
public:
    InstallActionEditor(Gtk::Window& window, InstallAction* action);

private:
    InstallAction* action;

    Gtk::Grid grid;
    Gtk::Label filenameLabel;
    Gtk::Entry filenameEntry;
    Gtk::Label destinationLabel;
    Gtk::Entry destinationEntry;
    Gtk::Label installFilenameLabel;
    Gtk::Entry installFilenameEntry;

    void onResponse(int responseId);
};
} /* namespace 2017 */

#endif /* INSTALL_ACTION_EDITOR_H */
