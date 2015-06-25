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

#include "xmlserializer/Attribute.h"

#include <map>
#include <string>
#include <functional>

namespace core
{
namespace xml
{
namespace binding
{

/** An xml tag is represented through a simple string */
using Tag = std::string;

class Body;

/** Nodes should be retrieved easily thanks to their tag */
using Nodes = std::map<Tag, Body>;

/** Type of functor which will be called just before Node creation */
using Routine = std::function<void()>;

using Node = std::pair<Tag, Body>;

namespace details
{

/** Xml Node representation */
struct Body
{
    Body(Routine r, Attributes a, Nodes c, Routine e) :
        startRoutine(r), attributes(a), childs(c), endRoutine(e), textContent("") {}
    Body(Attributes a, Nodes c) : Body(Routine{[](){}}, a, c, Routine{[](){}}) {}
    Body(Routine r, Attributes a, Nodes c) : Body(r, a, c, Routine{[](){}}) {}
    Body(Attributes a, Nodes c, Routine e) : Body(Routine{[](){}}, a, c, e) {}

    Body(const std::string &t, Routine r, Attributes a, Routine e) :
        startRoutine(r), attributes(a), childs{}, endRoutine(e), textContent(t) {}

    Routine startRoutine;
    Attributes attributes;
    Nodes childs;
    Routine endRoutine;
    std::string textContent;
};

} /** details namespace */

class Body
{
public:
    Body(const Body &body) = default;
    Body(Routine r, Attributes a, Nodes c, Routine e) :
        mBody{new details::Body(r, a, c, e)} {}
    Body(Attributes a, Nodes c) :
        mBody{new details::Body(a, c)} {}
    Body(Routine r, Attributes a, Nodes c) :
        mBody{new details::Body(r, a, c)} {}
    Body(Attributes a, Nodes c, Routine e) :
        mBody{new details::Body(a, c, e)} {}

    Body(const std::string &t, Attributes a, Routine e) :
        mBody{new details::Body(t, Routine{[](){}}, a, e)} {}
    Body(const std::string &t, Routine r, Attributes a) :
        mBody{new details::Body(t, r, a, Routine{[](){}})} {}
    Body(const std::string &t, Routine r, Attributes a, Routine e) :
        mBody{new details::Body(t, r, a, e)} {}
    Body(const std::string &t, Attributes a) :
        mBody{new details::Body(t, Routine{[](){}}, a, Routine{[](){}})} {}

    void startRoutine()
    {
        mBody->startRoutine();
    }
    void endRoutine()
    {
        mBody->endRoutine();
    }
    Attributes &getAttributes() const
    {
        return mBody->attributes;
    }
    Nodes &getChilds() const
    {
        return mBody->childs;
    }

private:
    std::shared_ptr<details::Body> mBody;
};

} /** binding namespace */
} /** xml namespace */
} /** core namespace */
