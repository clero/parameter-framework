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

#include "XmlStringDocSink.h"
#include "XmlMemoryDocSource.h"
#include "XmlSource.h"

#include <string>
#include <stdexcept>

namespace core
{
namespace xml
{
namespace serialization
{

class Serializer : IXmlSource
{
public:
    Serializer(std::string &xmlFile, binding::Node rootNode)
        : mCurrentNode(rootNode)
    {
        std::string error;
        CXmlSerializingContext context(error);
        CXmlMemoryDocSource memorySource(this, rootNode.first, "", "parameter-framework",
                                         "", false);

        CXmlStringDocSink stringSink(error);
        if (!stringSink.process(memorySource, context)) {
            throw std::runtime_error(error);
        }
        xmlFile = error;
    }

private:
    Serializer(binding::Node node)
        : mCurrentNode(node)
    {
    }

    virtual void toXml(CXmlElement& xmlElement,
                       CXmlSerializingContext& context) const override
    {
        for(auto &attribute : mCurrentNode.second.attributes) {
            xmlElement.setAttribute(attribute.getName(), attribute.get());
        }

        for(auto &child : mCurrentNode.second.childs) {
            // Create corresponding child element
            CXmlElement xmlChildElement;
            xmlElement.createChild(xmlChildElement, child.first);

            // Continue serialization
            Serializer childSerializer(child);
            childSerializer.toXml(xmlChildElement, context);
        }
    }

    binding::Node mCurrentNode;
};

} /** serialization namespace */
} /** xml namespace */
} /** core namespace */
