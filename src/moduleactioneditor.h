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

#ifndef MODULE_ACTION_EDITOR_H
#define MODULE_ACTION_EDITOR_H

#include <memory>

#include <gtkmm.h>

#include "moduleaction.h"

namespace gdfm {

class ModuleActionEditor : public Gtk::Dialog {
public:
    ModuleActionEditor(Gtk::Window& parent);

    /*
     * If the user has successfully created an action, then it returns a
     * pointer to the newly created action.
     *
     * If the user has not completed one, though, or it is invalid, it returns
     * a null smart pointer.
     *
     * Returns a smart pointer to a new ModuleAction created by the user if
     * they did so, a null smart pointer otherwise.
     */
    std::shared_ptr<ModuleAction> getAction();

private:
    Gtk::ComboBoxText typeBox;
    Gtk::Button createActionButton;

    std::shared_ptr<ModuleAction> action;

    void onCreateActionButtonClicked();
};
} /* namespace gdfm */

#endif /* MODULE_ACTION_EDITOR_H */
