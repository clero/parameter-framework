
#include <map>
#include <list>
#include <string>
#include <functional>

template <typename T> using Setter = std::function<void(T)>;
template <typename T>
struct Attribute
{
    const std::string name;
    const Setter<T> setter;
};

template <class... TypeNull> struct Attributes {};
template <class Head, class... Tail>
struct Attributes<Head, Tail...> : Attributes<Tail...>
{
    Attributes(Attribute<Head> head, Attribute<Tail>... tail) :
        Attributes<Tail...>(tail...), front(head)
    {
    }

    Attribute<Head> front;
};

template <class... TypeList>
struct Node;

template <class... TypeList>
struct Tag
{
    std::string tag;
    Node<TypeList...> node;
};

template <class... TypeList> using Tags = std::list<Tag<TypeList...> >;

template <class... TypeList>
struct Node
{
    Attributes<TypeList...> attributes;
    Tags<TypeList...> tags;
};

template <class... TypeList>
class Serializer
{
    Serializer(const std::string &xmlFile, Tag<TypeList...> rootTag)
    {
        CXmlSerializingContext context(error);
        CXmlFileDocSource source(_configurationFile,
                                 "",//_schemasLocation + "/ParameterFrameworkConfiguration.xsd",
                             "ParameterFrameworkConfiguration",
                             false);

        // TODO: throw in case of error
        source.populate(serializingContext);

        CXmlElement root;
        source.getRootElement(root);

        // TODO: test that rootTag has the right type

        const int size = sizeof...(rootTag.node.tags);

        for(auto &attribute : rootTag.node.tags) {

        for(auto &childTags : rootTag.node.tags) {
            //parse
        }




        // Open file
        // Read root tag
        // call good setters
        // call an other serializer on the next tag
    }

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

    void setA(int a) { mA = a; };
    void setB(std::string b) { mB = b; };
    void setC(bool c) { mC = c; };

private:
    int mA = 0;
    std::string mB = "";
    bool mC = false;
};

int main()
{
    FillMe toFill;
    Tags<int, std::string, bool> tags{
        {
            "FillMe",
            {
                {
                    {"A", [&toFill](int a){ toFill.setA(a); }},
                    {"B", [&toFill](std::string b){ toFill.setB(b); }},
                    {"C", [&toFill](bool c){ toFill.setC(c); } }
                },
                {} //empty child tag list
            }
        }
    };


    // ok
    //Attribute<std::string> test{"B", [&toFill](std::string b){ toFill.setB(b); }};
    //Attributes<int, std::string, bool> testList{
    //    {"A", [&toFill](int a){ toFill.setA(a); }},
    //    {"B", [&toFill](std::string b){ toFill.setB(b); }},
    //    {"C", [&toFill](bool c){ toFill.setC(c); }}
    //};
    //Tag<int, std::string, bool> tag{
    //    "FillMe",
    //    {
    //        {
    //            {"A", [&toFill](int a){ toFill.setA(a); }},
    //            {"B", [&toFill](std::string b){ toFill.setB(b); }},
    //            {"C", [&toFill](bool c){ toFill.setC(c); } }
    //        },
    //        {} //empty child tag list
    //    }
    //};
    return 0;
}


