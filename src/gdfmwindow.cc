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

#include "gdfmwindow.h"

#include <err.h>
#include <stdlib.h>

#include <iostream>

namespace gdfm {

GdfmWindow::GdfmWindow(
    BaseObjectType* cobject, const Glib::RefPtr<Gtk::Builder>& builder)
    : Gtk::ApplicationWindow(cobject), builder(builder)
{
    initChildren();
    connectSignals();
    addActions();
}

GdfmWindow::~GdfmWindow()
{
}

void
GdfmWindow::initChildren()
{
    builder->get_widget("add_module_button", addModuleButton);
    builder->get_widget("modules_view", modulesView);
}

void
GdfmWindow::connectSignals()
{
    addModuleButton->signal_clicked().connect(
        sigc::mem_fun(*this, &GdfmWindow::onAddModuleButtonClicked));
}

void
GdfmWindow::addActions()
{
    this->add_action("open", sigc::mem_fun(*this, &GdfmWindow::onActionOpen));
    this->add_action("save", sigc::mem_fun(*this, &GdfmWindow::onActionSave));
    this->add_action(
        "save-as", sigc::mem_fun(*this, &GdfmWindow::onActionSaveAs));
    this->add_action("quit", sigc::mem_fun(*this, &GdfmWindow::onActionQuit));
}

void
GdfmWindow::onAddModuleButtonClicked()
{
}

void
GdfmWindow::onActionOpen()
{
}

void
GdfmWindow::onActionSave()
{
}

void
GdfmWindow::onActionSaveAs()
{
}

void
GdfmWindow::onActionQuit()
{
    delete this;
}
} /* namespace gdfm */
