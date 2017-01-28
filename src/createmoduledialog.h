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

#ifndef CREATE_MODULE_DIALOG_H
#define CREATE_MODULE_DIALOG_H

#include <memory>

#include <gtkmm.h>

#include "module.h"

namespace gdfm {

class CreateModuleDialog : public Gtk::Dialog {
public:
    CreateModuleDialog(Gtk::Window& parent);
    virtual ~CreateModuleDialog();

    std::shared_ptr<Module> getModule();

private:
    Gtk::VBox* contentBox;
    Gtk::HBox* nameBox;
    Gtk::Label* nameLabel;
    Gtk::Entry* nameEntry;
    Gtk::HBox* filesBox;
    Gtk::ScrolledWindow* scrollWindow;
    Gtk::TreeView* filesView;
    Gtk::VBox* actionBox;
    Gtk::Entry* filenameEntry;
    Gtk::Entry* destinationEntry;
    Gtk::Button* addFileButton;
    Gtk::Button* removeFileButton;

    Glib::RefPtr<Gtk::TreeSelection> filesViewSelection;
    Glib::RefPtr<Gtk::ListStore> filesList;
    Gtk::TreeModelColumnRecord record;
    Gtk::TreeModelColumn<Glib::ustring> filenameColumn;
    Gtk::TreeModelColumn<Glib::ustring> destinationColumn;

    /* Signal handlers. */
    void onAddFileButtonClicked();
    void onFilesViewSelectionChanged();
    void onRemoveFileButtonClicked();
};
} /* namespace gdfm */

#endif /* CREATE_MODULE_DIALOG_H */
