/* <auto_header>
 * <FILENAME>
 * 
 * INTEL CONFIDENTIAL
 * Copyright © 2011 Intel 
 * Corporation All Rights Reserved.
 * 
 * The source code contained or described herein and all documents related to
 * the source code ("Material") are owned by Intel Corporation or its suppliers
 * or licensors. Title to the Material remains with Intel Corporation or its
 * suppliers and licensors. The Material contains trade secrets and proprietary
 * and confidential information of Intel or its suppliers and licensors. The
 * Material is protected by worldwide copyright and trade secret laws and
 * treaty provisions. No part of the Material may be used, copied, reproduced,
 * modified, published, uploaded, posted, transmitted, distributed, or
 * disclosed in any way without Intel’s prior express written permission.
 * 
 * No license under any patent, copyright, trade secret or other intellectual
 * property right is granted to or conferred upon you by disclosure or delivery
 * of the Materials, either expressly, by implication, inducement, estoppel or
 * otherwise. Any license under such intellectual property rights must be
 * express and approved by Intel in writing.
 * 
 *  AUTHOR: Patrick Benavoli (patrickx.benavoli@intel.com)
 * CREATED: 2011-06-01
 * UPDATED: 2011-07-27
 * 
 * 
 * </auto_header>
 */
#include "AreaConfiguration.h"
#include "ConfigurableElement.h"
#include "ConfigurationAccessContext.h"
#include "BinaryStream.h"
#include <assert.h>

CAreaConfiguration::CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet)
    : _pConfigurableElement(pConfigurableElement), _pSyncerSet(pSyncerSet), _bValid(false)
{
    // Size blackboard
    _blackboard.setSize(_pConfigurableElement->getFootPrint());
}

CAreaConfiguration::CAreaConfiguration(const CConfigurableElement* pConfigurableElement, const CSyncerSet* pSyncerSet, uint32_t uiSize)
    : _pConfigurableElement(pConfigurableElement), _pSyncerSet(pSyncerSet), _bValid(false)
{
    // Size blackboard
    _blackboard.setSize(uiSize);
}

// Save data from current
void CAreaConfiguration::save(const CParameterBlackboard* pMainBlackboard)
{
    copyFrom(pMainBlackboard, _pConfigurableElement->getOffset());
}

// Apply data to current
bool CAreaConfiguration::restore(CParameterBlackboard* pMainBlackboard, bool bSync, string& strError) const
{
    assert(_bValid);

    copyTo(pMainBlackboard, _pConfigurableElement->getOffset());

    // Synchronize if required
    if (bSync) {

        if (!_pSyncerSet->sync(*pMainBlackboard, false, strError)) {

            return false;
        }
    }

    return true;
}

// Ensure validity
void CAreaConfiguration::validate(const CParameterBlackboard* pMainBlackboard)
{
    if (!_bValid) {

        // Saving from blackboard make area configuration valid
        save(pMainBlackboard);

        _bValid = true;
    }
}

// Return validity
bool CAreaConfiguration::isValid() const
{
    return _bValid;
}

// Ensure validity against given valid area configuration
void CAreaConfiguration::validateAgainst(const CAreaConfiguration* pValidAreaConfiguration)
{
    // Should be called on purpose
    assert(!_bValid);

    // Check proper against area given
    assert(pValidAreaConfiguration->isValid());

    // Check compatibility
    assert(_pConfigurableElement == pValidAreaConfiguration->_pConfigurableElement);

    // Copy
    _blackboard.restoreFrom(&pValidAreaConfiguration->_blackboard, 0);

    // Set as valid
    _bValid = true;
}

// XML configuration settings parsing
bool CAreaConfiguration::serializeXmlSettings(CXmlElement& xmlConfigurableElementSettingsElementContent, CConfigurationAccessContext& configurationAccessContext)
{
    // Assign blackboard to configuration context
    configurationAccessContext.setParameterBlackboard(&_blackboard);

    // Assign base offset to configuration context
    configurationAccessContext.setBaseOffset(_pConfigurableElement->getOffset());

    // Parse configuration settings (element contents)
    if (_pConfigurableElement->serializeXmlSettings(xmlConfigurableElementSettingsElementContent, configurationAccessContext)) {

        if (!configurationAccessContext.serializeOut()) {

            // Serialized-in areas are valid
            _bValid = true;
        }
        return true;
    }
    return false;
}

// Compound handling
const CConfigurableElement* CAreaConfiguration::getConfigurableElement() const
{
    return _pConfigurableElement;
}

void CAreaConfiguration::copyToOuter(CAreaConfiguration* pToAreaConfiguration) const
{
    assert(_pConfigurableElement->isDescendantOf(pToAreaConfiguration->getConfigurableElement()));

    copyTo(&pToAreaConfiguration->_blackboard, _pConfigurableElement->getOffset() - pToAreaConfiguration->getConfigurableElement()->getOffset());
}

void CAreaConfiguration::copyFromOuter(const CAreaConfiguration* pFromAreaConfiguration)
{
    assert(_pConfigurableElement->isDescendantOf(pFromAreaConfiguration->getConfigurableElement()));

    copyFrom(&pFromAreaConfiguration->_blackboard, _pConfigurableElement->getOffset() - pFromAreaConfiguration->getConfigurableElement()->getOffset());

    // Inner becomes valid
    setValid(true);
}

// Serialization
void CAreaConfiguration::serialize(CBinaryStream& binaryStream)
{
    // Delegate to blackboard
    _blackboard.serialize(binaryStream);

    if (!binaryStream.isOut()) {

        // Serialized in areas are valid
        _bValid = true;
    }
}

// Data size
uint32_t CAreaConfiguration::getSize() const
{
    return _blackboard.getSize();
}

// Store validity
void CAreaConfiguration::setValid(bool bValid)
{
    _bValid = bValid;
}

// Blackboard copies
void CAreaConfiguration::copyTo(CParameterBlackboard* pToBlackboard, uint32_t uiOffset) const
{
    pToBlackboard->restoreFrom(&_blackboard, uiOffset);
}

void CAreaConfiguration::copyFrom(const CParameterBlackboard* pFromBlackboard, uint32_t uiOffset)
{
   pFromBlackboard->saveTo(&_blackboard, uiOffset);
}

