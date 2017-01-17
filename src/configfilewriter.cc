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

#include "configfilewriter.h"

#include <err.h>

namespace gdfm {

ConfigFileWriter::ConfigFileWriter(
    const std::string& path, const std::vector<Module> modules)
    : path(path), modules(modules)
{
    writer.open(path);
}

ConfigFileWriter::~ConfigFileWriter()
{
    if (isOpen())
        close();
}

bool
ConfigFileWriter::writeModules()
{
    if (!isOpen()) {
        warnx("Attempting to write to non-open writer.");
        return false;
    }
    for (const auto& module : modules) {
        for (const auto& line : module.createConfigLines())
            writer << line << std::endl;
        writer << std::endl;
    }
    return true;
}

bool
ConfigFileWriter::isOpen() const
{
    return writer.is_open();
}

void
ConfigFileWriter::close()
{
    writer.close();
}

const std::string&
ConfigFileWriter::getPath() const
{
    return path;
}

void
ConfigFileWriter::setPath(const std::string& path)
{
    this->path = path;
    writer.close();
    writer.open(path);
}

const std::vector<Module>&
ConfigFileWriter::getModules() const
{
    return modules;
}

void
ConfigFileWriter::setModules(const std::vector<Module>& modules)
{
    this->modules = modules;
}
} /* namespace gdfm */
