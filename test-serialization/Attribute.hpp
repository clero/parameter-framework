#pragma once

#include <convert.hpp>

#include <list>
#include <string>
#include <sstream>
#include <memory>
#include <functional>

/** Data member setter to use during deserialization
 *
 * @tparam the type of the data to retrieve
 */
template <typename T> using Setter = std::function<void(T)>;
template <typename T> using Getter = std::function<T()>;

template <typename T>
class AttributeImpl;

/** Xml Attribute representation.
 * Allows to retrieve the data in an xml file and to set it in the right place
 *
 * @tparam the type of the data to retrieve
 */
class Attribute
{
public:

    template<class Type, typename GetterType, typename SetterType>
    Attribute(const std::string &name, Type &&/**type*/, GetterType &&getter, SetterType &&setter) :
        mAttributeImpl{new AttributeImpl<typename Type::type>(name,
                                            Getter<typename Type::type>(getter),
                                            Setter<typename Type::type>(setter))}
    {
    }

    virtual ~Attribute() = default;

    virtual const std::string &getName() const
    {
        return mAttributeImpl->getName();
    }

    virtual std::string get() const
    {
        return mAttributeImpl->get();
    }

    virtual void set(const std::string& value)
    {
        mAttributeImpl->set(value);
    }

protected:

    Attribute() = default;

private:

    /**
     * shared_ptr is required because unique_ptr can't be copied
     * as Attribute are going to be in a list, there will be a copy
     * when initializing through an initializer_list (which always copy)
     */
    const std::shared_ptr<Attribute> mAttributeImpl = nullptr;
};

template <typename T>
class AttributeImpl : public Attribute
{
public:

    AttributeImpl(const std::string &name, Getter<T> getter, Setter<T> setter) :
        mName(name), mGetter(getter), mSetter(setter)
    {
    }

    virtual const std::string &getName() const override
    {
        return mName;
    }

    virtual std::string get() const override
    {
        std::ostringstream output;
        output << mGetter();
        return output.str();
    }

    virtual void set(const std::string &rawValue) override
    {
        // use convertTo

        T value;
        if (!convertTo(rawValue, value))
        {
            //throw
        }
        mSetter(value);
    }

private:

    const std::string mName;

    const Getter<T> mGetter;
    const Setter<T> mSetter;
};

using Attributes = std::list<Attribute>;

template <typename T> struct Type {};

template<> struct Type<int> { using type = int; };
template<> struct Type<long> { using type = long; };
template<> struct Type<long long> { using type = long long; };
template<> struct Type<unsigned> { using type = unsigned; };
template<> struct Type<unsigned long> { using type = unsigned long; };
template<> struct Type<unsigned long long> { using type = unsigned long long; };
template<> struct Type<float> { using type = float; };
template<> struct Type<double> { using type = double; };
template<> struct Type<long double> { using type = long double; };
template<> struct Type<bool> { using type = bool; };
template<> struct Type<std::string> { using type = std::string; };
