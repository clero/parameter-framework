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

#include <convert.hpp>

#include <list>
#include <string>
#include <sstream>
#include <memory>
#include <functional>
#include <stdexcept>
#include <utility>

namespace core
{
namespace xml
{
namespace binding
{

/** Data member setter to use during deserialization
 *
 * @tparam the type of the data to retrieve
 */
template <typename T> using Setter = std::function<void(T)>;
template <typename T> using Getter = std::function<T()>;
template <typename T> using Accessor = std::pair<Getter<T>, Setter<T> >;

template <typename T>
Accessor<T> makeAccessor(T &attribute)
{
    return make_pair(Getter<T>([&attribute](){return attribute;}),
                     Setter<T>([&attribute](T value){attribute = value;}));
}


/** Type deduction helper structure TODO: describe*/
template <typename T> struct Type {};

// @{
/** Available type List */
template<> struct Type<int> { using type = int; };
template<> struct Type<long> { using type = long; };
template<> struct Type<long long> { using type = long long; };
template<> struct Type<unsigned> { using type = unsigned; };
template<> struct Type<short unsigned> { using type = short unsigned; };
template<> struct Type<unsigned long> { using type = unsigned long; };
template<> struct Type<unsigned long long> { using type = unsigned long long; };
template<> struct Type<float> { using type = float; };
template<> struct Type<double> { using type = double; };
template<> struct Type<long double> { using type = long double; };
template<> struct Type<bool> { using type = bool; };
template<> struct Type<std::string> { using type = std::string; };
// @}

namespace details
{
/** Attribute Implementation forward declaration */
template <typename T>
class AttributeImpl;
} /** details namespace */

/** Xml Attribute representation.
 * Allows to retrieve the data in an xml file and to set it in the right place
 * This is a general facade which allows to store any type of attributes in the same container
 *
 * @tparam the type of the data to retrieve
 */
class Attribute
{
public:

    /** Attribute constructor
     * Allows to define a complete attributes. An attribute should contains methods
     * to store and retrieves data of an Object. This is done using Getter and Setter object
     *
     * TODO: param doc
     *
     */
    template<class Type, typename GetterType, typename SetterType>
    Attribute(const std::string &name, Type &&/**type*/, GetterType &&getter, SetterType &&setter) :
        mAttributeImpl{new details::AttributeImpl<typename Type::type>(name,
                                                              Getter<typename Type::type>(getter),
                                                              Setter<typename Type::type>(setter))}
    {
    }

    template<class T>
    Attribute(const std::string &name, Accessor<T> &&accessor) :
        Attribute(name, Type<T>{}, Getter<T>(accessor.first), Setter<T>(accessor.second))
    {
    }


    virtual ~Attribute() = default;

    /** Retrieve the attribute name
     *
     * @return a string containing the name of the attribute
     */
    virtual const std::string &getName() const
    {
        return mAttributeImpl->getName();
    }

    /** Retrieve the raw attribute value
     *
     * @return a string the raw attribute value
     */
    virtual std::string get() const
    {
        return mAttributeImpl->get();
    }

    /** Set the raw attribute value
     *
     * @param[in] value the raw attribute value to set
     */
    virtual void set(const std::string& value)
    {
        mAttributeImpl->set(value);
    }

protected:

    /** Default constructor needed by implementenation subclass */
    Attribute() = default;

private:

    /** Implementation older
     *
     * shared_ptr is required because unique_ptr can't be copied.
     * As Attribute are going to be in lists, there will be a copy
     * when initializing through an initializer_list (which always copy)
     */
    const std::shared_ptr<Attribute> mAttributeImpl = nullptr;
};

namespace details
{

/** Effective Attribute implementation class
 * Provide a real handling of a particular type of Attribute
 *
 * @tparam the type of the data contained in the attribute
 */
template <typename T>
class AttributeImpl : public Attribute
{
public:

    AttributeImpl(const std::string &name, Getter<T> getter, Setter<T> setter) :
        mName(name), mGetter(getter), mSetter(setter)
    {
    }

    virtual const std::string &getName() const override final
    {
        return mName;
    }

    virtual std::string get() const override final
    {
        std::ostringstream output;
        output << mGetter();
        return output.str();
    }

    virtual void set(const std::string &rawValue) override final
    {
        T value;
        if (!convertTo(rawValue, value))
        {
            throw std::invalid_argument("Invalid type conversion during '" +
                                        mName + "' attribute set");
        }
        mSetter(value);
    }

private:

    const std::string mName;

    const Getter<T> mGetter;
    const Setter<T> mSetter;
};

template <>
void AttributeImpl<std::string>::set(const std::string &rawValue)
{
    mSetter(rawValue);
}

} /** details namespace */

/** Renaming commonly used list of attributes types */
using Attributes = std::list<Attribute>;

} /** binding namespace */
} /** xml namespace */
} /** core namespace */
