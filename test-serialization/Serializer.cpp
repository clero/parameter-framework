
#include <map>
#include <list>
#include <string>
#include <tuple>
#include <functional>

//template <template <class> class Wrapper, class... TypeList>
//inline auto make_wrapped_tuple(Wrapper<TypeList>&&... wrapped)
//    -> decltype(std::make_tuple(wrapped...))
//{
//    return std::make_tuple(wrapped...);
//}
/** Data member setter to use during deserialization
 *
 * @tparam the type of the data to retrieve
 */
template <typename T> using Setter = std::function<void(T)>;

/** Xml Attribute representation.
 * Allows to retrieve the data in an xml file and to set it in the right place
 *
 * @tparam the type of the data to retrieve
 */
template <typename T>
struct Attribute
{
    const std::string name;
    const Setter<T> setter;
};

//template <class... TypeList> using Attributes = std::tuple<Attribute<TypeList>...>;

/** Attributes are list of attribute of differrent Setter type */

template <class T>
inline Attribute<T> make_attribute(const std::string &&name, const Setter<T> &setter)
{
    return {name, setter};
}

template <class... TypeList>
inline auto make_attributes(TypeList&&... attribute)
    -> decltype(std::make_tuple(attribute...))
{
    return std::make_tuple(attribute...);
}


template <class... TypeList>
inline auto make_node(const std::string&&, TypeList&&... node)
    -> decltype(std::make_tuple(attribute...))
{
    return std::make_tuple(attribute...);
}
//template <class... TypeNull> struct Nodes {};
//template <class Head, class... Tail>
//struct Nodes<Head, Tail...> : Nodes<Tail...>
//{
//    Nodes(Node<Head> head, Node<Tail>... tail) :
//        Nodes<Tail...>(tail...), front(head)
//    {
//    }
//
//    Node<Head> front;
//};

//template <class... TypeList> struct Node;
////template <class... TypeList> using Nodes = std::list<Node<TypeList...> >;
//
///** Xml tag representation */
//template <class... TypeNull> struct Node {};
//template <template <class... SubTypeList> class Head, class... Tail>
//struct Node<Head<SubTypeList>, Tail...> : Node<Tail...>
//{
//    Node(Head<SubTypeList> head, Node<Tail>... childs) :
//        Node<Tail...>(childs...), childs(head)
//    {
//    }
//
//    std::string tag;
//    //Head attributes;
//    Node<Tail...> childs;
//};

//template <template <class... TypeList> class AttributesTemplate, class... ChildTypeList> struct Node;
//template <template <class... TypeList> class AttributesTemplate, class... ChildTypeList>
//struct Node
//{
//
//    const std::string name;
//    AttributesTemplate<TypeList> attributes;
//    std::list<Node<ChildTypeList...> > childs;
//}


template <class... TypeList>
class Serializer
{
    //Serializer(const std::string &xmlFile, Node<TypeList...> rootNode)
    //{
        //CXmlSerializingContext context(error);
        //CXmlFileDocSource source(_configurationFile,
        //                         "",//_schemasLocation + "/ParameterFrameworkConfiguration.xsd",
        //                     "ParameterFrameworkConfiguration",
        //                     false);

        //// TODO: throw in case of error
        //source.populate(serializingContext);

        //CXmlElement root;
        //source.getRootElement(root);

        //// TODO: test that rootTag has the right type

        //const int size = sizeof...(rootTag.node.tags);

        //for(auto &attribute : rootTag.node.tags) {

        //for(auto &childTags : rootTag.node.tags) {
        //    //parse
        //}




        // Open file
        // Read root tag
        // call good setters
        // call an other serializer on the next tag
    //}

    //using XmlNode = std::string; // temporary

    //Serializer(XmlNode node, Tag rootTag)
    //{
    //    // Read root tag
    //    // call good setters
    //    // call an other serializer on the next tag
    //}
};

class FillMe
{
public:
    int getA() { return mA; };
    std::string getB() { return mB; };
    bool getC() { return mC; };

    Setter<int> sA = [this](int a){ mA = a; };
    Setter<std::string> sB = [this](std::string b){ mB = b; };
    Setter<bool> sC = [this](bool c){ mC = c; };

private:
    int mA = 0;
    std::string mB = "";
    bool mC = false;
};

int main()
{
    FillMe toFill;
    auto attr = make_attribute("A", toFill.sA);

    auto attributes = make_attributes(
                        make_attribute("A", toFill.sA),
                        make_attribute("B", toFill.sB),
                        make_attribute("C", toFill.sC)
                        );

    //FillMe pleaseFill;
    //Nodes<int, std::string, bool> nodes{
    //    {
    //        "FillMe",
    //        {
    //            {"A", [&ill](int a){ toFill.setA(a); }},
    //            {"B", [&toFill](std::string b){ toFill.setB(b); }},
    //            {"C", [&toFill](bool c){ toFill.setC(c); } }
    //        },
    //        {} //empty child tag list
    //    },
    //    {
    //        "FillIt",
    //        {
    //            {"A", [&pleaseFill](int a){ pleaseFill.setA(a); }},
    //            {"B", [&pleaseFill](std::string b){ pleaseFill.setB(b); }},
    //            {"C", [&pleaseFill](bool c){ pleaseFill.setC(c); } }
    //        },
    //        {} //empty child tag list
    //    }
    //};


    // ok
    //Attribute<std::string> test{"B", [&toFill](std::string b){ toFill.setB(b); }};
    //Attributes<int, std::string, bool> testList{
    //    {"A", [&toFill](int a){ toFill.setA(a); }},
    //    {"B", [&toFill](std::string b){ toFill.setB(b); }},
    //    {"C", [&toFill](bool c){ toFill.setC(c); }}
    //};
    //Node<int, std::string, bool> tag{
    //    "FillMe",
    //    {
    //        {"A", [&toFill](int a){ toFill.setA(a); }},
    //        {"B", [&toFill](std::string b){ toFill.setB(b); }},
    //        {"C", [&toFill](bool c){ toFill.setC(c); } }
    //    },
    //    {} //empty child tag list
    //};
    return 0;
}


