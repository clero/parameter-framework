/*
 * Copyright (c) 2011-2015, Intel Corporation
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
#include "DomainConfiguration.h"
#include "AreaConfiguration.h"
#include "ConfigurableElement.h"
#include "CompoundRule.h"
#include "Subsystem.h"
#include "ConfigurationAccessContext.h"
#include <assert.h>
#include "RuleParser.h"

using std::string;

CDomainConfiguration::CDomainConfiguration(const string& strName) : _name(strName), _rule(nullptr)
{
}

CDomainConfiguration::CDomainConfiguration() : CDomainConfiguration("")
{
}

CDomainConfiguration::~CDomainConfiguration()
{
}

// Class kind
string CDomainConfiguration::getKind() const
{
    return "Configuration";
}

// XML configuration settings parsing
//bool CDomainConfiguration::parseSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext)
//{
//    // Actual XML context
//    CXmlDomainImportContext& xmlDomainImportContext = static_cast<CXmlDomainImportContext&>(serializingContext);
//
//    // Take care of configurable elements / area configurations ranks
//    std::list<CAreaConfiguration*> areaConfigurationList;
//
//    // Parse configurable element's configuration settings
//    CXmlElement::CChildIterator it(xmlConfigurationSettingsElement);
//
//    CXmlElement xmlConfigurableElementSettingsElement;
//
//    while (it.next(xmlConfigurableElementSettingsElement)) {
//
//        // Retrieve area configuration
//        string strConfigurableElementPath;
//        xmlConfigurableElementSettingsElement.getAttribute("Path", strConfigurableElementPath);
//
//        CAreaConfiguration* pAreaConfiguration = findAreaConfiguration(strConfigurableElementPath);
//
//        if (!pAreaConfiguration) {
//
//            xmlDomainImportContext.setError("Configurable Element " + strConfigurableElementPath  + " referred to by Configuration " + getPath() + " not associated to Domain");
//
//            return false;
//        }
//        // Ranks
//        areaConfigurationList.push_back(pAreaConfiguration);
//
//        // Parse
//        if (!serializeConfigurableElementSettings(pAreaConfiguration, xmlConfigurableElementSettingsElement, xmlDomainImportContext, false)) {
//
//            return false;
//        }
//    }
//
//    // Reorder area configurations according to XML content
//    reorderAreaConfigurations(areaConfigurationList);
//
//    return true;
//}
//
//// XML configuration settings composing
//void CDomainConfiguration::composeSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext) const
//{
//    // Go through all are configurations
//    AreaConfigurationListIterator it;
//
//    for (it = _orderedAreaConfigurationList.begin(); it != _orderedAreaConfigurationList.end(); ++it) {
//
//        const CAreaConfiguration* pAreaConfiguration = *it;
//
//        // Retrieve configurable element
//        const CConfigurableElement* pConfigurableElement = pAreaConfiguration->getConfigurableElement();
//
//        // Create configurable element child element
//        CXmlElement xmlConfigurableElementSettingsElement;
//
//        xmlConfigurationSettingsElement.createChild(xmlConfigurableElementSettingsElement, "ConfigurableElement");
//
//        // Set Path attribute
//        xmlConfigurableElementSettingsElement.setAttribute("Path", pConfigurableElement->getPath());
//
//        // Delegate composing to area configuration
//        ((CDomainConfiguration&)(*this)).serializeConfigurableElementSettings((CAreaConfiguration*)pAreaConfiguration, xmlConfigurableElementSettingsElement, serializingContext, true);
//    }
//}
//
// Serialize one configuration for one configurable element
//bool CDomainConfiguration::serializeConfigurableElementSettings(CAreaConfiguration* pAreaConfiguration, CXmlElement& xmlConfigurableElementSettingsElement, CXmlSerializingContext& serializingContext, bool bSerializeOut)
//{
//    // Actual XML context
//    CXmlDomainExportContext& xmlDomainExportContext =
//        static_cast<CXmlDomainExportContext&>(serializingContext);
//
//    // Configurable Element
//    const CConfigurableElement* pConfigurableElement = pAreaConfiguration->getConfigurableElement();
//
//    // Element content
//    CXmlElement xmlConfigurableElementSettingsElementContent;
//
//    // Deal with element itself
//    if (!bSerializeOut) {
//
//        // Check structure
//        if (xmlConfigurableElementSettingsElement.getNbChildElements() != 1) {
//
//            // Structure error
//            serializingContext.setError("Struture error encountered while parsing settings of " + pConfigurableElement->getKind() + " " + pConfigurableElement->getName() + " in Configuration " + getPath());
//
//            return false;
//        }
//
//        // Check name and kind
//        if (!xmlConfigurableElementSettingsElement.getChildElement(pConfigurableElement->getKind(), pConfigurableElement->getName(), xmlConfigurableElementSettingsElementContent)) {
//
//            serializingContext.setError("Couldn't find settings for " + pConfigurableElement->getKind() + " " + pConfigurableElement->getName() + " for Configuration " + getPath());
//
//            return false;
//        }
//    } else {
//
//        // Create child XML element
//        xmlConfigurableElementSettingsElement.createChild(xmlConfigurableElementSettingsElementContent, pConfigurableElement->getKind());
//
//        // Set Name
//        xmlConfigurableElementSettingsElementContent.setNameAttribute(pConfigurableElement->getName());
//    }
//
//    // Change context type to parameter settings access
//    string strError;
//
//    // Create configuration access context
//    CConfigurationAccessContext configurationAccessContext(strError, bSerializeOut);
//
//    // Provide current value space
//    configurationAccessContext.setValueSpaceRaw(xmlDomainExportContext.valueSpaceIsRaw());
//
//    // Provide current output raw format
//    configurationAccessContext.setOutputRawFormat(xmlDomainExportContext.outputRawFormatIsHex());
//
//    // Get subsystem
//    const CSubsystem* pSubsystem = pConfigurableElement->getBelongingSubsystem();
//
//    if (pSubsystem && pSubsystem != pConfigurableElement) {
//
//        // Element is a descendant of subsystem
//
//        // Deal with Endianness
//        configurationAccessContext.setBigEndianSubsystem(pSubsystem->isBigEndian());
//    }
//
//    // Have domain configuration parse settings for configurable element
//    if (!pAreaConfiguration->serializeXmlSettings(xmlConfigurableElementSettingsElementContent, configurationAccessContext)) {
//
//        // Forward error
//        xmlDomainExportContext.setError(strError);
//
//        return false;
//    }
//    return true;
//}

// Configurable Elements association
void CDomainConfiguration::addConfigurableElement(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet)
{
    AreaWrapper area{pConfigurableElement->createAreaConfiguration(pSyncerSet)};

    _areaConfigurationList.emplace_back(area);
    _orderedAreaConfigurationList.emplace_back(area);
}

void CDomainConfiguration::removeConfigurableElement(const CConfigurableElement* pConfigurableElement)
{
    AreaWrapper areaToRemove = getAreaConfiguration(pConfigurableElement);

    _areaConfigurationList.remove(areaToRemove);
    _orderedAreaConfigurationList.remove(areaToRemove);
}

// Sequence management
bool CDomainConfiguration::setElementSequence(const std::vector<string>& astrNewElementSequence, string& strError)
{
    // Build a new list of AreaConfiguration objects
    std::list<AreaWrapper> areaConfigurationList;

    uint32_t uiConfigurableElement;

    for (uiConfigurableElement = 0; uiConfigurableElement < astrNewElementSequence.size(); uiConfigurableElement++) {

        string strConfigurableElementPath = astrNewElementSequence[uiConfigurableElement];

        AreaWrapper pAreaConfiguration = findAreaConfiguration(strConfigurableElementPath);

        if (!pAreaConfiguration) {

            strError = "Element " + strConfigurableElementPath + " not found in domain";

            return false;
        }
        // Check not already present in the list
        if (findAreaConfiguration(strConfigurableElementPath, areaConfigurationList)) {

            strError = "Element " + strConfigurableElementPath + " provided more than once";

            return false;
        }

        // Store new ordered area configuration
        areaConfigurationList.push_back(pAreaConfiguration);
    }

    // Reorder area configurations according to given path list
    reorderAreaConfigurations(areaConfigurationList);

    return true;
}

void CDomainConfiguration::getElementSequence(string& strResult) const
{
    strResult = "\n";

    // List configurable element paths out of ordered area configuration list
    for (auto &area : _orderedAreaConfigurationList) {
        strResult += area->getConfigurableElement()->getPath() + "\n";
    }
}

// Application rule
bool CDomainConfiguration::setApplicationRule(const string& strApplicationRule,
                                              const core::criterion::internal::Criteria& criteria,
                                              string& strError)
{
    // Parser
    CRuleParser ruleParser(strApplicationRule, criteria);

    // Attempt to parse it
    if (!ruleParser.parse(NULL, strError)) {

        return false;
    }
    // Replace compound rule
    CCompoundRule* rule = ruleParser.grabRootRule();
    if (rule != nullptr) {
        _rule = std::shared_ptr<CCompoundRule>(rule);
    }

    return true;
}

void CDomainConfiguration::clearApplicationRule()
{
    _rule = nullptr;
}

void CDomainConfiguration::getApplicationRule(string& strResult) const
{
    if (_rule != nullptr) {
        // Start clear
        strResult.clear();

        // Dump rule
        _rule->dump(strResult);

    } else {

        strResult = "<none>";
    }
}

/**
 * Get the Configuration Blackboard.
 *
 * Fetch the Configuration Blackboard related to the ConfigurableElement given in parameter. This
 * Element is used to retrieve the correct AreaConfiguration where the Blackboard is stored.
 *
 * @param[in] pConfigurableElement      A pointer to a ConfigurableElement that is part of the
 *                                      Domain. This must have been checked previously, as an
 *                                      assertion is performed.
 *
 * return Pointer to the Blackboard of the Configuration.
 */
CParameterBlackboard* CDomainConfiguration::getBlackboard(const CConfigurableElement* pConfigurableElement) const
{
    for (auto &area : _areaConfigurationList) {

        // Check if the Element is associated with the Domain
        if (area->getConfigurableElement() == pConfigurableElement) {

            return &area->getBlackboard();
        }
    }

    assert(0);
    return NULL;
}

// Save data from current
void CDomainConfiguration::save(const CParameterBlackboard* pMainBlackboard)
{
    // Just propagate to areas
    for (auto &area : _areaConfigurationList) {
        area->save(pMainBlackboard);
    }
}

// Apply data to current
bool CDomainConfiguration::restore(CParameterBlackboard* pMainBlackboard,
                                   bool bSync,
                                   core::Results* errors) const
{
    bool bSuccess = true;

    // Just propagate to areas
    for (auto &area : _orderedAreaConfigurationList) {
        bSuccess &= area->restore(pMainBlackboard, bSync, errors);
    }

    return bSuccess;
}

// Ensure validity for configurable element area configuration
void CDomainConfiguration::validate(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard)
{
    AreaWrapper pAreaConfigurationToValidate = getAreaConfiguration(pConfigurableElement);

    // Delegate
    pAreaConfigurationToValidate->validate(pMainBlackboard);
}

// Ensure validity of all area configurations
void CDomainConfiguration::validate(const CParameterBlackboard* pMainBlackboard)
{
    for (auto &area : _areaConfigurationList) {
        area->validate(pMainBlackboard);
    }
}

// Return configuration validity for given configurable element
bool CDomainConfiguration::isValid(const CConfigurableElement* pConfigurableElement) const
{
    // Get child configurable elemnt's area configuration
    AreaWrapper pAreaConfiguration = getAreaConfiguration(pConfigurableElement);

    assert(pAreaConfiguration);

    return pAreaConfiguration->isValid();
}

// Ensure validity of configurable element's area configuration by copying in from a valid one
void CDomainConfiguration::validateAgainst(const CDomainConfiguration* pValidDomainConfiguration, const CConfigurableElement* pConfigurableElement)
{
    // Retrieve related area configurations
    AreaWrapper pAreaConfigurationToValidate = getAreaConfiguration(pConfigurableElement);
    const AreaWrapper pAreaConfigurationToValidateAgainst = pValidDomainConfiguration->getAreaConfiguration(pConfigurableElement);

    // Delegate to area
    pAreaConfigurationToValidate->validateAgainst(pAreaConfigurationToValidateAgainst.get());
}

// Ensure validity of all configurable element's area configuration by copying in from a valid ones
void CDomainConfiguration::validateAgainst(const CDomainConfiguration* pValidDomainConfiguration)
{
    // Copy in configuration data from against domain
    auto it = _areaConfigurationList.begin();
    auto itAgainst = pValidDomainConfiguration->_areaConfigurationList.begin();

    for (it = _areaConfigurationList.begin(), itAgainst = pValidDomainConfiguration->_areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it, ++itAgainst) {

        AreaWrapper pAreaConfigurationToValidate = *it;
        const AreaWrapper pAreaConfigurationToValidateAgainst = *itAgainst;

        // Delegate to area
        pAreaConfigurationToValidate->validateAgainst(pAreaConfigurationToValidateAgainst.get());
    }
}

// Dynamic data application
bool CDomainConfiguration::isApplicable() const
{
    return _rule != nullptr && _rule->matches();
}

// Merge existing configurations to given configurable element ones
void CDomainConfiguration::merge(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement)
{
    // Retrieve related area configurations
    AreaWrapper pAreaConfigurationToMergeTo = getAreaConfiguration(pToConfigurableElement);
    const AreaWrapper pAreaConfigurationToMergeFrom = getAreaConfiguration(pFromConfigurableElement);

    // Do the merge
    pAreaConfigurationToMergeFrom->copyToOuter(pAreaConfigurationToMergeTo.get());
}

// Domain splitting
void CDomainConfiguration::split(CConfigurableElement* pFromConfigurableElement)
{
    // Retrieve related area configuration
    const AreaWrapper pAreaConfigurationToSplitFrom = getAreaConfiguration(pFromConfigurableElement);

    // Go through children areas to copy configuration data to them
    size_t uiNbConfigurableElementChildren = pFromConfigurableElement->getNbChildren();
    size_t uiChild;

    for (uiChild = 0; uiChild < uiNbConfigurableElementChildren; uiChild++) {

        CConfigurableElement* pToChildConfigurableElement = static_cast<CConfigurableElement*>(pFromConfigurableElement->getChild(uiChild));

        // Get child configurable elemnt's area configuration
        AreaWrapper pChildAreaConfiguration = getAreaConfiguration(pToChildConfigurableElement);

        // Do the copy
        pChildAreaConfiguration->copyFromOuter(pAreaConfigurationToSplitFrom.get());
    }
}

// AreaConfiguration retrieval from configurable element
CDomainConfiguration::AreaWrapper CDomainConfiguration::getAreaConfiguration(const CConfigurableElement* pConfigurableElement) const
{
    for (auto &area :  _areaConfigurationList) {
        if (area->getConfigurableElement() == pConfigurableElement) {
            return area;
        }
    }
    // Not found?
    assert(0);

    return NULL;
}

// AreaConfiguration retrieval from present area configurations
CDomainConfiguration::AreaWrapper CDomainConfiguration::findAreaConfiguration(const string& strConfigurableElementPath) const
{
    return findAreaConfiguration(strConfigurableElementPath, _areaConfigurationList);
}

// AreaConfiguration retrieval from given area configuration list
CDomainConfiguration::AreaWrapper CDomainConfiguration::findAreaConfiguration(const string& strConfigurableElementPath, const std::list<AreaWrapper>& areaConfigurationList) const
{
    for (auto &area :  areaConfigurationList) {
        if (area->getConfigurableElement()->getPath() == strConfigurableElementPath) {
            return area;
        }
    }

    // Not found
    return NULL;
}

// Area configuration ordering
void CDomainConfiguration::reorderAreaConfigurations(const std::list<AreaWrapper>& areaConfigurationList)
{
    // Ensure elements in provided list appear first and ordered the same way in internal one

    // Remove all elements present in the provided list from the internal one
    for (auto &area :  _areaConfigurationList) {
        _orderedAreaConfigurationList.remove(area);
    }

    // Prepended provided elements into internal list
    _orderedAreaConfigurationList.insert(_orderedAreaConfigurationList.begin(), areaConfigurationList.begin(), areaConfigurationList.end());
}

// Serialization
//void CDomainConfiguration::binarySerialize(CBinaryStream& binaryStream)
//{
//    AreaConfigurationListIterator it;
//
//    // Area configurations order
//    if (binaryStream.isOut()) {
//
//        for (it = _orderedAreaConfigurationList.begin(); it != _orderedAreaConfigurationList.end(); ++it) {
//
//            // Get rank
//            uint32_t uiAreaConfigurationRank = getAreaConfigurationRank(*it);
//
//            // Store it
//            binaryStream.write((const uint8_t*)&uiAreaConfigurationRank, sizeof(uiAreaConfigurationRank));
//        }
//    } else {
//
//        // Empty ordered list first
//        _orderedAreaConfigurationList.resize(0);
//
//        uint32_t uiAreaConfiguration;
//
//        for (uiAreaConfiguration = 0; uiAreaConfiguration < _areaConfigurationList.size(); uiAreaConfiguration++) {
//
//            // Get rank
//            uint32_t uiAreaConfigurationRank;
//
//            binaryStream.read((uint8_t*)&uiAreaConfigurationRank, sizeof(uiAreaConfigurationRank));
//
//            _orderedAreaConfigurationList.push_back(getAreaConfiguration(uiAreaConfigurationRank));
//        }
//    }
//
//    // Propagate to areas
//    for (it = _areaConfigurationList.begin(); it != _areaConfigurationList.end(); ++it) {
//
//        CAreaConfiguration* pAreaConfiguration = *it;
//
//        pAreaConfiguration->serialize(binaryStream);
//    }
//}

// Data size
size_t CDomainConfiguration::getDataSize() const
{
    size_t uiDataSize;

    // Add necessary size to store area configurations order
    uiDataSize = _areaConfigurationList.size() * sizeof(uint32_t);

    // Propagate request to areas
    for (auto &area : _areaConfigurationList) {
        uiDataSize += area->getSize();
    }
    return uiDataSize;
}
