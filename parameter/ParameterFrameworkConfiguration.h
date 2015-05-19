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
#pragma once

#include <string>
#include <list>

class CParameterFrameworkConfiguration
{
public:
    CParameterFrameworkConfiguration(const std::string &configurationFile);

    bool init(std::string &error);

    // System class name
    const std::string& getSystemClassName() const;

    // Tuning allowed
    bool isTuningAllowed() const;

    // Server port
    uint16_t getServerPort() const;

    std::list<std::string> getPlugins() const
    {
        return _plugins;
    }

    const std::string &getBinarySettingsFile() const
    {
        return _binarySettingsFile;
    }

    const std::string &getSettingsFile() const
    {
        return _settingsFile;
    }

    const std::string &getStructureFile() const
    {
        return _structureFile;
    }

    const std::string &getSchemasLocation() const
    {
        return _schemasLocation;
    }

    void setSchemasLocation(const std::string &location)
    {
        _schemasLocation = location;
    }

    // System class name
    std::string _strSystemClassName;
    // Tuning allowed
    bool _bTuningAllowed;
    // Server port
    uint16_t _uiServerPort;
    std::string _configurationFile;
    std::string _configurationFolder;
    //static
    std::string _schemasLocation;
    std::string _structureFile;

    std::string _settingsFile;
    std::string _binarySettingsFile;

    std::list<std::string> _plugins;
};
