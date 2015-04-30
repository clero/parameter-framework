/*
 * Copyright (c) 2011-2014, Intel Corporation
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
#include "ParameterFrameworkConfiguration.h"
#include "XmlFileDocSource.h"
#include "XmlMemoryDocSink.h"
#include "XmlElement.h"

CParameterFrameworkConfiguration::CParameterFrameworkConfiguration(const std::string &configurationFile)
    : _bTuningAllowed(false), _uiServerPort(0), _configurationFile(configurationFile)
{
}

bool CParameterFrameworkConfiguration::init(std::string &error)
{
    // TODO: take bool for validate
    std::string::size_type lastSlashPos = _configurationFile.rfind('/', -1);
    if(lastSlashPos == std::string::npos) {
        // Configuration folder is the current folder
        _configurationFolder = '.';
    } else {
        _configurationFolder = _configurationFile.substr(0, lastSlashPos);
    }
    _schemasLocation = _configurationFolder + "/Schemas";

    CXmlSerializingContext context(error);
    CXmlFileDocSource source(_configurationFile,
                             _schemasLocation + "/ParameterFrameworkConfiguration.xsd",
                             "ParameterFrameworkConfiguration",
                             false);

    CXmlMemoryDocSink sink(this);
    return sink.process(source, context);
}

// System class name
const std::string& CParameterFrameworkConfiguration::getSystemClassName() const
{
    return _strSystemClassName;
}

// Tuning allowed
bool CParameterFrameworkConfiguration::isTuningAllowed() const
{
    return _bTuningAllowed;
}

// Server port
uint16_t CParameterFrameworkConfiguration::getServerPort() const
{
    return _uiServerPort;
}

bool CParameterFrameworkConfiguration::retrieveSettingsConfiguration(const CXmlElement &settingsNode,
                                                                     CXmlSerializingContext& context)
{
    if (settingsNode.getNbChildElements() > 2) {
        context.setError("too much settings node");
        return false;
    }
    bool settingsChildFound = false;
    CXmlElement childNode;
    CXmlElement::CChildIterator childIterator(settingsNode);
    while (childIterator.next(childNode)) {
        if (childNode.getType() == "ConfigurableDomainsFileLocation") {
            if(!retrievePathAttribute(childNode, _settingsFile, context)) {
                return false;
            }
            settingsChildFound = true;
        } else if (childNode.getType() == "BinarySettingsFileLocation") {
            if (!retrievePathAttribute(childNode, _binarySettingsFile, context)) {
                return false;
            }
        } else {
            context.setError("Unknown child");
            return false;
        }
    }
    if (!settingsChildFound) {
        context.setError("No ConfigurableDomainsFileLocation element found"
                         " for SystemClass " + _strSystemClassName);
        return false;
    }
    return true;
}

bool CParameterFrameworkConfiguration::retrievePathAttribute(const CXmlElement &xmlElement,
                                                             std::string &path,
                                                             CXmlSerializingContext& context)
{
    path = xmlElement.getAttributeString("Path");
    if (path.empty()) {
        context.setError("Empty Path attribute in element " + xmlElement.getPath());
        return false;
    }
    if (path[0] != '/') {
        // Path is relative
        path = _configurationFolder + "/" + path;
    }
    return true;
}

bool CParameterFrameworkConfiguration::retrievePluginsConfiguration(const CXmlElement &pluginNode,
                                                                    CXmlSerializingContext& context)
{
    CXmlElement::CChildIterator childLocation(pluginNode);
    CXmlElement xmlPluginLocation;
    while (childLocation.next(xmlPluginLocation)) {
        if (!(xmlPluginLocation.getType() == "Location")) {
            context.setError("pas de location");
            return false;
        }
        // Retrieve folder
        std::string pluginFolder = pluginNode.getAttributeString("Folder");
        if (!pluginFolder.empty()) {
            pluginFolder += "/";
        }

        // Get Info from children
        CXmlElement::CChildIterator childIterator(xmlPluginLocation);
        CXmlElement xmlPluginElement;
        while (childIterator.next(xmlPluginElement)) {
            if (!(xmlPluginElement.getType() == "Plugin")) {
                context.setError("pas de plugin");
                return false;
            }

            // Fill Plugin List
            _plugins.push_back(pluginFolder + xmlPluginElement.getAttributeString("Name"));
        }
    }
    return true;
}

// From IXmlSink
bool CParameterFrameworkConfiguration::fromXml(const CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
{
    // System class name
    _strSystemClassName = xmlElement.getAttributeString("SystemClassName");

    // Tuning allowed
    _bTuningAllowed = xmlElement.getAttributeBoolean("TuningAllowed");

    // Server port
    _uiServerPort = (uint16_t)xmlElement.getAttributeInteger("ServerPort");

    CXmlElement childNode;
    CXmlElement::CChildIterator childIterator(xmlElement);

    bool structureChildFound = false;
    bool subsystemPluginsChildFound = false;
    while (childIterator.next(childNode)) {
        if (childNode.getType() == "SubsystemPlugins") {
            if(!retrievePluginsConfiguration(childNode, serializingContext)) {
                return false;
            }
            subsystemPluginsChildFound = true;
        } else if (childNode.getType() == "StructureDescriptionFileLocation") {
            if(!retrievePathAttribute(childNode, _structureFile, serializingContext)) {
                return false;
            }
            structureChildFound = true;
        } else if (childNode.getType() == "SettingsConfiguration") {
            if (!retrieveSettingsConfiguration(childNode, serializingContext)) {
                return false;
            }
        } else {
            serializingContext.setError("Unknown child");
            return false;
        }
    }
    if (!structureChildFound) {
        serializingContext.setError("No StructureDescriptionFileLocation element"
                                    " found for SystemClass " + _strSystemClassName);
    }
    if (!subsystemPluginsChildFound) {
        serializingContext.setError("Parameter Framework Configuration: couldn't "
                                    "find SubsystemPlugins element");
    }
    return true;
}

