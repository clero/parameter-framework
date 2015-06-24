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
#pragma once

#include "CompoundRule.h"
#include "Results.h"
#include <criterion/Criteria.h>

#include <list>
#include <string>
#include <vector>
#include <memory>

class CConfigurableElement;
class CAreaConfiguration;
class CParameterBlackboard;
class CConfigurationAccessContext;
class CSyncerSet;

class CDomainConfiguration
{
public:
    CDomainConfiguration();
    CDomainConfiguration(const std::string& strName);
    virtual ~CDomainConfiguration();

    // Configurable Elements association
    void addConfigurableElement(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet);
    void removeConfigurableElement(const CConfigurableElement* pConfigurableElement);

    // Sequence management
    bool setElementSequence(const std::vector<std::string>& astrNewElementSequence, std::string& strError);
    void getElementSequence(std::string& strResult) const;

    /** Set the new application rule for this configuration
     *
     * @param[in] strApplicationRule the rule to parse
     * @param[in] criteria application criteria
     * @param[out] error, error encountered during restoration
     * @result true is success false otherwise
     */
    bool setApplicationRule(const std::string& strApplicationRule,
                            const core::criterion::internal::Criteria& criteria,
                            std::string& strError);

    void clearApplicationRule();
    void getApplicationRule(std::string& strResult) const;

    // Get Blackboard for an element of the domain
    CParameterBlackboard* getBlackboard(const CConfigurableElement* pConfigurableElement) const;

    // Save data from current
    void save(const CParameterBlackboard* pMainBlackboard);

    /** Restore the configuration
     *
     * @param[in] pMainBlackboard the application main blackboard
     * @param[in] bSync indicates if a synchronisation has to be done
     * @param[out] errors, errors encountered during restoration
     * @return true if success false otherwise
     */
    bool restore(CParameterBlackboard* pMainBlackboard,
                 bool bSync,
                 core::Results* errors = NULL) const;

    // Ensure validity for configurable element area configuration
    void validate(const CConfigurableElement* pConfigurableElement, const CParameterBlackboard* pMainBlackboard);
    // Ensure validity of all area configurations
    void validate(const CParameterBlackboard* pMainBlackboard);
    // Return configuration validity for given configurable element
    bool isValid(const CConfigurableElement* pConfigurableElement) const;
    // Ensure validity of configurable element's area configuration by copying in from a valid one
    void validateAgainst(const CDomainConfiguration* pValidDomainConfiguration, const CConfigurableElement* pConfigurableElement);
    // Ensure validity of all configurable element's area configuration by copying in from a valid ones
    void validateAgainst(const CDomainConfiguration* pValidDomainConfiguration);
    // Applicability checking
    bool isApplicable() const;
    // Merge existing configurations to given configurable element ones
    void merge(CConfigurableElement* pToConfigurableElement, CConfigurableElement* pFromConfigurableElement);
    // Domain splitting
    void split(CConfigurableElement* pFromConfigurableElement);

    // XML configuration settings parsing/composing
    //bool parseSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext);
    //void composeSettings(CXmlElement& xmlConfigurationSettingsElement, CXmlSerializingContext& serializingContext) const;

    //// Serialization
    //virtual void binarySerialize(CBinaryStream& binaryStream);

    // Data size
    virtual size_t getDataSize() const;

    // Class kind
    virtual std::string getKind() const;

    std::string getName() const
    {
        return _name;
    }
    void setName(const std::string &name)
    {
        _name = name;
    }
    void setRule(std::shared_ptr<CCompoundRule> rule)
    {
        _rule = rule;
    }

private:
    using AreaWrapper = std::shared_ptr<CAreaConfiguration>;

    // AreaConfiguration retrieval from configurable element
    AreaWrapper getAreaConfiguration(const CConfigurableElement* pConfigurableElement) const;
    // AreaConfiguration retrieval from present area configurations
    AreaWrapper findAreaConfiguration(const std::string& strConfigurableElementPath) const;
    // AreaConfiguration retrieval from given area configuration std::list
    AreaWrapper findAreaConfiguration(const std::string& strConfigurableElementPath, const std::list<AreaWrapper>& areaConfigurationList) const;
    // Area configuration ordering
    void reorderAreaConfigurations(const std::list<AreaWrapper>& areaConfigurationList);

    // Rule
    const CCompoundRule* getRule() const;
    CCompoundRule* getRule();

    // AreaConfigurations
    std::list<AreaWrapper> _areaConfigurationList;
    std::list<AreaWrapper> _orderedAreaConfigurationList;

    /** TODO: remove it if possible */
    std::string _name;

    /** there is a possibility that we have no Rule
     * use ptr until Rule refactor (to have empty rule flag)
     */
    std::shared_ptr<CCompoundRule> _rule;
};
