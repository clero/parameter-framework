/*
 * Copyright (c) 2015, Intel Corporation
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#pragma once

#include "Node.hpp"
#include "ParameterFrameworkConfiguration.h"

#include <string>
#include <stdexcept>

namespace core
{
namespace xml
{
namespace bindings
{

class ParameterFrameworkConfigurationXmlBinding
{
public:
    /** @param[in] configuration the reference on the configuration to fill */
    ParameterFrameworkConfigurationXmlBinding(CParameterFrameworkConfiguration &configuration)
        : mConfiguration(configuration), mPluginFolder("")
    {
    }


    /** Generate Xml bindings for a ParameterFrameworkConfiguration object
     *
     * return root Node of generated bindings
     */
    core::xml::binding::Node getBindings()
    {
        using namespace core::xml::binding;
        Node plugin {
            "Plugin",
            Body {
                Attributes {
                    { "Name",
                      Type<std::string>{},
                      [] () { throw std::runtime_error("Unimplemented serialization behavior of"
                                                       " ParameterFrameworkConfiguration");
                              return ""; },
                      [this] (std::string name) {
                          mConfiguration._plugins.push_back(
                                  mPluginFolder + (mPluginFolder.empty() ? "" : "/") + name);
                      }
                    }
                },
                Nodes {}
            }
        };
        Node location {
            "Location",
            Body {
                Attributes { { "Folder", makeAccessor(mPluginFolder) } },
                Nodes { plugin }
            }
        };
        Node subsystemPlugins { "SubsystemPlugins", Body { Attributes {}, Nodes { location } } };
        Node structure {
            "StructureDescriptionFileLocation",
            Body { Attributes { makePathAttribute(mConfiguration._structureFile) }, Nodes {} }
        };
        Node configurableDomains {
            "ConfigurableDomainsFileLocation",
            Body { Attributes { makePathAttribute(mConfiguration._settingsFile) }, Nodes {} }
        };
        Node settingsBinary {
            "BinarySettingsFileLocation",
            Body { Attributes { makePathAttribute(mConfiguration._binarySettingsFile) }, Nodes {} }
        };
        Node settings {
            "SettingsConfiguration",
            Body { Attributes {}, Nodes { configurableDomains, settingsBinary } }
        };
        Node configuration {
            "ParameterFrameworkConfiguration",
            Body {
                Attributes {
                    { "SystemClassName", makeAccessor(mConfiguration._strSystemClassName) },
                    { "ServerPort", makeAccessor(mConfiguration._uiServerPort) },
                    { "TuningAllowed", makeAccessor(mConfiguration._bTuningAllowed) }
                },
                Nodes { subsystemPlugins, structure, settings }
            }
        };

        return configuration;
    }

private:
    /** Helper for Path attribute binding generation
     * Prepare, in the setter, the path by adding the configuration folder to any
     * relative path.
     *
     * @return an Attribute which can hold a path
     */
    core::xml::binding::Attribute makePathAttribute(std::string &pathSink)
    {
        using namespace core::xml::binding;
        return { "Path", Type<std::string>{},
                 [&pathSink] () { return pathSink; },
                 [&pathSink, this] (std::string path) {
                    if (path[0] != '/') {
                        // Path is relative
                        pathSink = mConfiguration._configurationFolder + "/" + path;
                    }
                 }
        };
    }
    /** Reference on the Configuration object to fill */
    CParameterFrameworkConfiguration &mConfiguration;

    /** Plugin Folder temporary variable */
    std::string mPluginFolder;
};

} /** bindings namespace */
} /** xml namespace */
} /** core namespace */
