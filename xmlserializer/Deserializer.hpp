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

#include "XmlSink.h"
#include "XmlFileDocSource.h"
#include "XmlMemoryDocSink.h"

namespace core
{
namespace xml
{
namespace serialization
{

class Deserializer : IXmlSink
{
public:
    Deserializer(const std::string &xmlFile, binding::Node rootNode)
        : mCurrentNode(rootNode)
    {
        std::string error;
        CXmlSerializingContext context(error);
        CXmlFileDocSource source(xmlFile,
                                 "",//_schemasLocation + "/ParameterFrameworkConfiguration.xsd",
                                 rootNode.first,
                                 false);
        CXmlMemoryDocSink sink(this);

        if (!sink.process(source, context)) {
            throw std::runtime_error(error);
        }
    }

private:
    Deserializer(binding::Node node)
        : mCurrentNode(node)
    {
    }

    virtual bool fromXml(const CXmlElement& xmlElement,
                         CXmlSerializingContext& context) override
    {
        if (xmlElement.getType() != mCurrentNode.first) {
            std::string error = "Tag mismatch";
            context.setError(error);
            return false;
        }

        // No check here, schema are here for that purpose
        for(auto &attribute : mCurrentNode.second.attributes) {
            std::string rawAttribute;
            if (xmlElement.hasAttribute(attribute.getName())) {
                xmlElement.getAttribute(attribute.getName(), rawAttribute);

                try {
                    attribute.set(rawAttribute);
                } catch (std::invalid_argument &e) {
                    std::string error = "Attribute error '" + attribute.getName() + "': " + e.what();
                    context.setError(error);
                    return false;
                }
            }
        }

        /**
         * Nodes:
         *  go through all child nodes read from xml
         *  for each node execute deserialization routine (ex push_back etc)
         *  call deserialize on it
         *
         */
        if (xmlElement.getNbChildElements() != 0) {
            CXmlElement::CChildIterator childIterator(xmlElement);
            CXmlElement childNode;
            while ( childIterator.next(childNode)) {

                /**
                 * Move all in the other constructor ?
                 */
                // retrieve node and propagate Create new serializer
                binding::Node childNodeStruct =
                    {childNode.getType(), mCurrentNode.second.childs.at(childNode.getType())};

                // Propagate
                Deserializer childSerializer(childNodeStruct);
                if (!childSerializer.fromXml(childNode, context)) {
                    return false;
                }
            }
        }
        return true;
    }

    binding::Node mCurrentNode;
};

} /** serialization namespace */
} /** xml namespace */
} /** core namespace */
