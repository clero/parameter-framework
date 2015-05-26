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
#include <cassert>
#include "ConfigurableDomains.h"
#include "ConfigurableElement.h"
#include "BinaryStream.h"

#define DELEGATE_DOMAIN_WITH_RESULT(domainName, error, delegatedMethod, ...) \
    delegateToDomain<bool, decltype(&CConfigurableDomain::delegatedMethod), \
                     &CConfigurableDomain::delegatedMethod>(domainName, error, __VA_ARGS__)

#define DELEGATE_DOMAIN(domainName, error, delegatedMethod, ...) \
    delegateToDomain<decltype(&CConfigurableDomain::delegatedMethod), \
                     &CConfigurableDomain::delegatedMethod>(domainName, error, __VA_ARGS__)

using std::string;

CConfigurableDomains::CConfigurableDomains() : _domains()
{
}

// Ensure validity on whole domains from main blackboard
void CConfigurableDomains::validate(const CParameterBlackboard* pMainBlackboard)
{
    for (auto &domain : _domains) {
        domain.second.validate(pMainBlackboard);
    }
}

// Configuration application if required
void CConfigurableDomains::apply(CParameterBlackboard* pParameterBlackboard,
                                 CSyncerSet& syncerSet,
                                 bool bForce,
                                 core::Results& infos) const
{
    for (auto &domain : _domains) {
        std::string info;
        // Apply and collect syncers when relevant
        domain.second.apply(pParameterBlackboard, &syncerSet, bForce, info);

        if (!info.empty()) {
            infos.push_back(info);
        }
    }
    // Synchronize those collected syncers
    syncerSet.sync(*pParameterBlackboard, false, NULL);

    // Then deal with domains that need to synchronize along apply
    for (auto &domain : _domains) {
        std::string info;
        // Apply and synchronize when relevant
        domain.second.apply(pParameterBlackboard, NULL, bForce, info);
        if (!info.empty()) {
            infos.push_back(info);
        }
    }
}

// Configuration/Domains handling
/// Domains
bool CConfigurableDomains::createDomain(const string& strName, string& strError)
{
    if (_domains.find(strName) != _domains.end()) {

        strError = "Already existing configurable domain";

        return false;
    }
    _domains.emplace(strName, CConfigurableDomain(strName));
    return true;
}

bool CConfigurableDomains::addDomain(CConfigurableDomain& domain, bool bOverwrite,
                                     string& strError)
{
    if (!bOverwrite && (_domains.find(domain.getName()) != _domains.end())) {
        strError = "Can't add domain \"" + domain.getName() +
            "\" because it already exists and overwrite was not requested.";
        return false;
    }
    // TODO: is copy a problem?
    _domains[domain.getName()] = domain;
    return true;
}

bool CConfigurableDomains::deleteDomain(const string& name)
{
    return _domains.erase(name) == 1;
}

void CConfigurableDomains::deleteAllDomains()
{
    _domains.clear();
}

bool CConfigurableDomains::renameDomain(const string& strName, const string& strNewName, string& strError)
{
    if (_domains.find(strNewName) != _domains.end()) {
        strError = "Already existing domain '" + strNewName  + "'";
        return false;
    }
    auto domainIt = _domains.find(strName);
    if (domainIt == _domains.end()) {
        return notFoundDomainError(strName, strError);
    }
    //domainIt->second.rename(strName, strError);
    //std::swap(_domains[strName], *domainIt);
    //_domains.erase(domainIt);
    return true;
}

bool CConfigurableDomains::setSequenceAwareness(const string& strDomain, bool bSequenceAware, string& strError)
{
    return DELEGATE_DOMAIN(strDomain, strError, setSequenceAwareness, bSequenceAware);
}

bool CConfigurableDomains::getSequenceAwareness(const string& strDomain, bool& bSequenceAware, string& strError) const
{
    try {
        bSequenceAware = _domains.at(strDomain).getSequenceAwareness();
        return true;
    } catch (std::out_of_range &e) {
        return notFoundDomainError(strDomain, strError);
    }
}

/// Configurations
bool CConfigurableDomains::listConfigurations(const string& strDomain, string& strResult) const
{
    return DELEGATE_DOMAIN(strDomain, strResult, listConfigurations, strResult);
}

bool CConfigurableDomains::createConfiguration(const string& strDomain, const string& strConfiguration, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, createConfiguration,
                                       strConfiguration, pMainBlackboard, strError);
}

bool CConfigurableDomains::deleteConfiguration(const string& strDomain, const string& strConfiguration, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, deleteConfiguration,
                                       strConfiguration, strError);
}

bool CConfigurableDomains::renameConfiguration(const string& strDomain, const string& strConfigurationName, const string& strNewConfigurationName, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, renameConfiguration,
                                       strConfigurationName, strNewConfigurationName, strError);
}

bool CConfigurableDomains::listDomainElements(const string& strDomain, string& strResult) const
{
    return DELEGATE_DOMAIN(strDomain, strResult, listAssociatedToElements, strResult);
}

bool CConfigurableDomains::split(const string& domainName,
                                 CConfigurableElement* element,
                                 core::Results& infos)
{
    string error;
    bool success = DELEGATE_DOMAIN_WITH_RESULT(domainName, error, split, element, infos);
    if (!success) { infos.push_back(error); };
    return success;
}

void CConfigurableDomains::listAssociatedElements(string& strResult) const
{
    strResult = "\n";

    std::set<const CConfigurableElement*> configurableElementSet;

    // Get all owned configurable elements
    gatherAllOwnedConfigurableElements(configurableElementSet);

    // Fill result
    std::set<const CConfigurableElement*>::const_iterator it;

    for (it = configurableElementSet.begin(); it != configurableElementSet.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        string strAssociatedDomainList;

        pConfigurableElement->listAssociatedDomains(strAssociatedDomainList, false);

        strResult += pConfigurableElement->getPath() + " [" + strAssociatedDomainList + "]\n";
    }
}

void CConfigurableDomains::listConflictingElements(string& strResult) const
{
    strResult = "\n";

    std::set<const CConfigurableElement*> configurableElementSet;

    // Get all owned configurable elements
    gatherAllOwnedConfigurableElements(configurableElementSet);

    // Fill result
    std::set<const CConfigurableElement*>::const_iterator it;

    for (it = configurableElementSet.begin(); it != configurableElementSet.end(); ++it) {

        const CConfigurableElement* pConfigurableElement = *it;

        if (pConfigurableElement->getBelongingDomainCount() > 1) {

            string strBelongingDomainList;

            pConfigurableElement->listBelongingDomains(strBelongingDomainList, false);

            strResult += pConfigurableElement->getPath() + " contained in multiple domains: " + strBelongingDomainList + "\n";
        }
    }
}

void CConfigurableDomains::listDomains(string& strResult) const
{
    for (auto &domain : _domains) {
        strResult += "\n" + domain.second.getName() +
                     (domain.second.getSequenceAwareness() ? " [sequence aware]" : "");
    }
}

// Gather configurable elements owned by any domain
void CConfigurableDomains::gatherAllOwnedConfigurableElements(std::set<const CConfigurableElement*>& configurableElementSet) const
{
    for (auto &domain : _domains) {
        domain.second.gatherConfigurableElements(configurableElementSet);
    }
}

// Config restore
bool CConfigurableDomains::restoreConfiguration(const string& domainName,
                                                const string& configurationName,
                                                CParameterBlackboard* mainBlackboard,
                                                bool autoSync,
                                                core::Results& errors) const
{
    string error;
    bool success = DELEGATE_DOMAIN_WITH_RESULT(domainName, error, restoreConfiguration,
                                   configurationName, mainBlackboard, autoSync, errors);
    if (!success) { errors.push_back(error); };
    return success;
}

// Config save
bool CConfigurableDomains::saveConfiguration(const string& strDomain, const string& strConfiguration, const CParameterBlackboard* pMainBlackboard, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, saveConfiguration,
                                       strConfiguration, pMainBlackboard, strError);
}

bool CConfigurableDomains::setElementSequence(const string& strDomain, const string& strConfiguration, const std::vector<string>& astrNewElementSequence, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, setElementSequence,
                                       strConfiguration, astrNewElementSequence, strError);
}

bool CConfigurableDomains::getElementSequence(const string& strDomain, const string& strConfiguration, string& strResult) const
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strResult, getElementSequence,
                                       strConfiguration, strResult);
}

bool CConfigurableDomains::setApplicationRule(const string& strDomain,
                                              const string& strConfiguration,
                                              const string& strApplicationRule,
                                              const core::criterion::internal::Criteria& criteria,
                                              string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, setApplicationRule, strConfiguration,
                                       strApplicationRule, criteria, strError);
}

bool CConfigurableDomains::clearApplicationRule(const string& strDomain, const string& strConfiguration, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, clearApplicationRule,
                                       strConfiguration, strError);
}

bool CConfigurableDomains::getApplicationRule(const string& strDomain, const string& strConfiguration, string& strResult) const
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strResult, getApplicationRule,
                                       strConfiguration, strResult);
}

// Last applied configurations
void CConfigurableDomains::listLastAppliedConfigurations(string& strResult) const
{
    for (auto &domain : _domains) {

        strResult += domain.second.getName() + ": " +
                     domain.second.getLastAppliedConfigurationName() +
                     " [" + domain.second.getPendingConfigurationName() + "]\n";
    }
}

// Configurable element - domain association
bool
CConfigurableDomains::addConfigurableElementToDomain(const string& domainName,
                                                     CConfigurableElement* element,
                                                     const CParameterBlackboard* mainBlackboard,
                                                     core::Results& infos)
{
    string error;
    bool success = DELEGATE_DOMAIN_WITH_RESULT(domainName, error,
                                   addConfigurableElement, element, mainBlackboard, infos);
    if (!success) { infos.push_back(error); };
    return success;
}

bool CConfigurableDomains::removeConfigurableElementFromDomain(const string& strDomain, CConfigurableElement* pConfigurableElement, string& strError)
{
    return DELEGATE_DOMAIN_WITH_RESULT(strDomain, strError, removeConfigurableElement, pConfigurableElement, strError);
}

CParameterBlackboard* CConfigurableDomains::findConfigurationBlackboard(const string& strDomain,
                                                       const string& strConfiguration,
                                                       const CConfigurableElement* pConfigurableElement,
                                                       uint32_t& uiBaseOffset,
                                                       bool& bIsLastApplied,
                                                       string& strError) const
{
    try {
        const CConfigurableDomain &domain = _domains.at(strDomain);
        if (!pConfigurableElement->belongsTo(&domain)) {

            strError = "Element \"" + pConfigurableElement->getPath() +
                       "\" does not belong to domain \"" + strDomain + "\"";
            return NULL;
        }

        // Find Configuration Blackboard and Base Offset
        return domain.findConfigurationBlackboard(strConfiguration, pConfigurableElement,
                                                  uiBaseOffset, bIsLastApplied, strError);
    } catch (std::out_of_range &e) {
        notFoundDomainError(strDomain, strError);
        return nullptr;
    }

}

// Binary settings load/store
//bool CConfigurableDomains::serializeSettings(const string& strBinarySettingsFilePath, bool bOut, uint8_t uiStructureChecksum, string& strError)
//{
//    // Instantiate byte stream
//    CBinaryStream binarySettingsStream(strBinarySettingsFilePath, bOut, getDataSize(), uiStructureChecksum);
//
//    // Open file
//    if (!binarySettingsStream.open(strError)) {
//
//        strError = "Unable to open binary settings file " + strBinarySettingsFilePath + ": " + strError;
//
//        return false;
//    }
//
//    // Serialize
//    binarySerialize(binarySettingsStream);
//
//    // Close stream
//    binarySettingsStream.close();
//
//    return true;
//}

inline bool CConfigurableDomains::notFoundDomainError(const std::string &name,
                                                      std::string &error) const
{
   error = "Configurable domain " + name + " not found";
   return false;
}
