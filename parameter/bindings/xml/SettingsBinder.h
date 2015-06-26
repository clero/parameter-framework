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

#include "RulesBinder.h"
#include <xmlserializer/Node.h>
#include "ConfigurationAccessContext.h"
#include "AreaConfiguration.h"

#include <string>
#include <list>
#include <functional>

namespace core
{
namespace bindings
{
namespace xml
{

class SettingsBinder
{
public:
    SettingsBinder(CConfigurableDomains &domains,
                   core::criterion::internal::Criteria &criteria,
                   CSystemClass &systemClass) :
        mDomains(domains), mRawDomains{}, mCriteria{criteria},
        mSystemClass(systemClass), mRulesBinder{mCriteria}
    {
    }

    void handleConfigurableElement()
    {
        CPathNavigator pathNavigator(mTmpConfigurableElementPath);
        std::string error;

        // Is there an element and does it match system class name?
        if (!pathNavigator.navigateThrough(mSystemClass.getName(), error)) {
            throw std::runtime_error(
                    "Could not find configurable element of path " + mTmpConfigurableElementPath +
                    " from ConfigurableDomain description " + mRawDomains.back().getName() +
                    " (" + error + ")");
        }
        // Browse system class for configurable element
        CConfigurableElement* configurableElement =
            static_cast<CConfigurableElement*>(mSystemClass.findDescendant(pathNavigator));

        if (!configurableElement) {
            throw std::runtime_error(
                    "Could not find configurable element of path " + mTmpConfigurableElementPath +
                    " from ConfigurableDomain description " + mRawDomains.back().getName());

        }

        core::Results infos;
        if (!mRawDomains.back().addConfigurableElement(configurableElement, NULL, infos)) {
            CUtility::asString(infos, error);
            throw std::runtime_error(error);
        }
    }

    CDomainConfiguration &getCurrentConfiguration()
    {
        try {
            return mRawDomains.back()._configurations[mTmpConfigurableElementPath];
        } catch (std::out_of_range) {
            throw std::runtime_error("Could not find domain configuration '" +
                                     mTmpConfigurableElementPath + "' referred to by configurable "
                                     "domain '" + mRawDomains.back().getName() + "'.");
        }
    }

    void handleSettings()
    {
        // Take care of configurable elements / area configurations ranks
        AreaWrapper area = getCurrentConfiguration().findAreaConfiguration(mTmpConfigurableElementPath);

        if (area == nullptr) {
           throw std::runtime_error(
                   "Configurable Element " + mTmpConfigurableElementPath  +
                   " referred to by Configuration " + mTmpSettingsConfName +
                   " not associated to Domain");
        }
        // Ranks
        mOrderedArea.push_back(area);

        std::string error;
        // Create configuration access context
        CConfigurationAccessContext context(error, /**bSerializeOut*/false);

        // Provide current value space
        context.setValueSpaceRaw(false);

        // Provide current output raw format
        context.setOutputRawFormat(false);

        // Get subsystem
        const CSubsystem* subsystem = area->getConfigurableElement()->getBelongingSubsystem();

        if (subsystem && subsystem != area->getConfigurableElement()) {

            // Element is a descendant of subsystem

            // Deal with Endianness
            context.setBigEndianSubsystem(subsystem->isBigEndian());
        }
        // Assign blackboard to configuration context
        context.setParameterBlackboard(&(area->_blackboard));

        // Assign base offset to configuration context
        context.setBaseOffset(area->getConfigurableElement()->getOffset());

        //area->getConfigurableElement()->serializeXmlSettings();
    }

    core::xml::binding::Node getBindings()
    {
        using namespace core::xml::binding;

        Node stringParameter {
            "StringParameter",
            Body {
                /** text Content */
                makeBinder(mTmpParameterValue),
                Attributes{ {"Name", makeBinder(mTmpParameter)} },
                Routine { [this] { handleSettings(); } }
            }
        };
        Node settingsConfigurableElement {
            "ConfigurableElement",
            Body {
                Attributes{ {"Path", makeBinder(mTmpConfigurableElementPath)} },
                Nodes{ stringParameter },
            }
        };
        Node settingsConfiguration {
            "Configuration",
            Body {
                Attributes{ {"Name", makeBinder(mTmpSettingsConfName)} },
                Nodes{ settingsConfigurableElement },
                Routine {
                    [this] {
                        getCurrentConfiguration().reorderAreaConfigurations(mOrderedArea);
                    }
                }
            }
        };
        Node settings {
            "Settings",
            Body {
                Attributes{}, Nodes{ settingsConfiguration },
            }
        };
        Node configurableElement {
            "ConfigurableElement",
            Body {
                Attributes{ {"Path", makeBinder(mTmpConfigurableElementPath)} }, Nodes{},
                Routine { [this] { handleConfigurableElement(); } }
            }
        };
        Node configurableElements {
            "ConfigurableElements",
            Body { Attributes{}, Nodes{ configurableElement } }
        };
        Node configuration {
            "Configuration",
            Body {
                Routine { [this] () {
                    mRawConfigurations.emplace_back();
                } },
                Attributes {
                    {
                        "Name",
                        Type<std::string>{},
                        [this](){ return ""; }, //FIXME
                        [this](std::string name){ mRawConfigurations.back().setName(name); }
                    }
                },
                Nodes { mRulesBinder.getBindings() },
                Routine { [this] () {
                        mRawConfigurations.back().setRule(mRulesBinder.getRule());
                    }
                },
            }
        };
        Node configurations {
            "Configurations",
            Body {
                Attributes {},
                Nodes { configuration },
                Routine {
                    [this] () {
                        for (auto &conf : mRawConfigurations) {
                            mRawDomains.back()._configurations.emplace(conf.getName(), conf);
                        }
                        mRawConfigurations.clear();
                    }
                }
            }
        };

        /** Domains */
        Node configurableDomain {
            "ConfigurableDomain",
            Body {
                Routine { [this] () { mRawDomains.emplace_back(); } },
                Attributes {
                    {
                        "Name",
                        Type<std::string>{},
                        [this](){ return ""; },
                        [this](std::string name){ mRawDomains.back()._name = name; }
                    },
                    {
                        "SequenceAware",
                        Type<bool>{},
                        [this](){ return false; },
                        [this](bool sequenceAware){ mRawDomains.back()._bSequenceAware = sequenceAware; }
                    }
                },
                Nodes { configurations, configurableElements, settings },
            }
        };
        Node configurableDomains {
            "ConfigurableDomains",
            Body {
                Attributes {},
                Nodes {configurableDomain},
                Routine {
                    [this] () {
                        for (auto &domain : mRawDomains) {
                            std::string error;
                            if (!mDomains.addDomain(domain, false, error)) {
                                throw std::invalid_argument(error);
                            }
                        }
                        mRawDomains.clear();
                    }
                }
            }
        };
        return configurableDomains;
    }

private:

    CConfigurableDomains &mDomains;
    std::list<CConfigurableDomain> mRawDomains;
    std::list<CDomainConfiguration> mRawConfigurations;

    // Settings
    std::string mTmpConfigurableElementPath;
    std::string mTmpSettingsConfName;
    using AreaWrapper = std::shared_ptr<CAreaConfiguration>;
    std::list<AreaWrapper> mAreaWrapper;
    std::list<AreaWrapper> mOrderedArea;
    std::string mTmpParameter;
    /** FIXME: should be of any types */
    std::string mTmpParameterValue;

    core::criterion::internal::Criteria &mCriteria;
    CSystemClass &mSystemClass;

    RulesBinder mRulesBinder;
};

//class DomainBinder
//{
//
//    core::xml::binding::Node getBindings()
//    {
//        using namespace core::xml::binding;
//        /** Domains */
//        Node configurableDomain {
//            "ConfigurableDomain",
//            Body {
//                /** need func to create child */
//                //Routine {
//                //    [this] (const std::string& name) {
//                //        std::string error;
//                //        if (!mDomains.createDomain(name, error)) {
//                //            throw std::invalid_argument(error);
//                //        }
//                //    }
//                //},
//                Attributes {
//                    { "Name", makeAccessor(mDomains._domains.at(name)._name) },
//                    { "SequenceAware", makeAccessor(mDomains._domains.at(name)._bSequenceAware) },
//                },
//                Nodes { configurations, configurableElements, settings }
//            }
//        };
//}

} /** xml namespace */
} /** bindings namespace */
} /** core namespace */