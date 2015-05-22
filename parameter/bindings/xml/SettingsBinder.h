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

#include <xmlserializer/Node.h>

#include <string>

namespace core
{
namespace bindings
{
namespace xml
{

class SettingsBinder
{
public:

    core::xml::binding::Node getBindings()
    {
        /** Conf */
        Node compoundRule {
            "SelectionCriterionRule",
            Body {
                Attributes {
                    { "SelectionCriterion", makeAccessor(/**mDomains.name*/) },
                    { "MatchesWhen", makeAccessor(/**mDomains.name*/) },
                    { "Value", makeAccessor(/**mDomains.name*/) },
                },
                Nodes {}
            }
        };
        Node compoundRule {
            "CompoundRule",
            Body {
                Attributes { { "Type", makeAccessor(/**mDomains.name*/) } },
                Nodes { compoundRule, criterionRule }
            }
        };
        Node configuration = makeConfiguration(compoundRule);
        Node configurations = makeContainer("Configurations", configuration);

        /** Elements */
        Node configurableElement {
            "ConfigurableElement",
            Body { Attributes { { "Path", makeAccessor(/**mDomains.name*/) } }, Nodes {} }
        };
        Node configurableElements = makeContainer("ConfigurableElements", configurableElement);

        /** Settings */
        /** imake each parameters */
        Node componentType {
            "Subsystem",
            Body {
                Attributes { { "Path", makeAccessor(/**mDomains.name*/) } },
                Nodes { bitParameter, componentType }
            }
        };
        Node bitParameter /** TODO */{
            "BitParameter",
            Body {
                Attributes { { "Path", makeAccessor(/**mDomains.name*/) } },
                Nodes { bitParameter, subsystem }
            }
        };
        Node configurableElementSettings {
            "ConfigurableElement",
            Body {
                Attributes { { "Path", makeAccessor(/**mDomains.name*/) } },
                Nodes { bitParameter, componentType }
            }
        };
        Node configurationSettings = makeConfiguration(configurableElementSetting);
        Node settings = makeContainer("Settings", configurationSettings);

        /** Domains */
        Node configurableDomain {
            "ConfigurableDomain",
            Body {
                /** need func to create child */
                Attributes {
                    { "Name", makeAccessor(/**mDomains.name*/) },
                    { "SequenceAware", makeAccessor(/**mDomains.name*/) },
                },
                Nodes { configurations, configurableElements, settings }
            }
        };
        Node configurableDomains {
            "ConfigurableDomains",
            Body {
                Attributes {
                    { "SystemClassName", makeAccessor(mDomains.name) },
                },
                Nodes { configurableDomain }
            }
        };
    }

private:

    xml::bindings::Node makeContainer(const std::string &name, Node &contained)
    {
        return { name, Body { Attributes {}, Nodes { contained } } };
    }
    xml::bindings::Node makeConfiguration(Nodes &contained)
    {
        return {
            "Configuration",
            Body { Attributes { { "Name", makeAccessor(/**mDomains.name*/) } }, contained }
        };
    }

    CConfigurableDomains &mDomains;

}
