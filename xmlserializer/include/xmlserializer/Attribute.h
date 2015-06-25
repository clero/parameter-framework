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

#include "xmlserializer/DataHolder.h"

#include <list>
#include <string>

namespace core
{
namespace xml
{
namespace binding
{

/** Xml Attribute representation.
 * Allows to retrieve the data in an xml file and to set it in the right place
 * This is a general facade which allows to store any type of attributes in the same container
 */
class Attribute : public DataHolder
{
public:
    template<class type, typename GetterType, typename SetterType>
    Attribute(const std::string &name, Type<type> &&t, GetterType &&getter, SetterType &&setter) :
        DataHolder(std::move(t), getter, setter), mName(name)
    {
    }
    template<class T>
    Attribute(const std::string &name, Binder<T> &&binder) :
        DataHolder(Type<T>{}, Getter<T>(binder.first), Setter<T>(binder.second)), mName(name)
    {
    }

    /** Retrieve the attribute name
     *
     * @return a string containing the name of the attribute
     */
    virtual const std::string &getName() const
    {
        return mName;
    }

private:
    /** XML attribute name */
    const std::string mName;
};

/** Renaming commonly used list of attributes types */
using Attributes = std::list<Attribute>;

} /** binding namespace */
} /** xml namespace */
} /** core namespace */
