#include <Attribute.hpp>
#include "XmlFileDocSource.h"
#include "IXmlSource.h"

#include <list>
#include <map>
#include <string>
#include <memory>
#include <functional>

using Tag = std::string;
class NodeBody;
using Nodes = std::map<Tag, NodeBody>;

/** Xml tag representation */
class NodeBody
{
//public:
//    Node(const std::string &tag, Attributes &attributes, Nodes &childs) :
//        mTag(tag), mAttributes(attributes), mChilds(childs)
//    {
//    }

public:
    Attributes mAttributes;
    Nodes mChilds;
};

using Node = std::pair<Tag, NodeBody>;

class Deserializer
{
    Deserializer(/**const std::string &xmlFile, Node rootNode*/)
    {
        CXmlSerializingContext context(error);
        CXmlFileDocSource source(xmlFile,
                                 "",//_schemasLocation + "/ParameterFrameworkConfiguration.xsd",
                                 rootNode.mTag,
                                 false);

        // TODO: throw in case of error
        source.populate(serializingContext);



      // Open file
      // Read root tag
      // call good setters
      // call an other serializer on the next tag
    }

    using XmlNode = std::string; // temporary


    void deserialize()
    {
        CXmlElement root;
        source.getRootElement(root);

        // TODO: test that rootTag has the right type
        for(auto &attribute : rootTag.second.mAttributes) {

        }

        /**
         * Nodes:
         *  go through all child nodes read from xml
         *  for each node execute deserialization routine (ex push_back etc)
         *  call deserialize on it
         *
         */



       // for(auto &childTags : rootTag.nodes) {

       //     //parse
       // }
    }


    //Serializer(XmlNode node, Tag rootTag)
    //{
    //    // Read root tag
    //    // call good setters
    //    // call an other serializer on the next tag
    //}
};

class Serialize : IXmlSource
{
    Serialize(const std::string &xmlFile, Node rootNode)
    {
        CXmlSerializingContext context(error);
        CXmlMemoryDocSource memorySource(this, rootNode.first, "", "parameter-framework",
                                         "", false);

        // TODO: throw in case of error
        memorySource.populate(serializingContext);



      // Open file
      // Read root tag
      // call good setters
      // call an other serializer on the next tag
    }

    void toXml(CXmlElement& xmlElement, CXmlSerializingContext& serializingContext)
    {
        CXmlElement root;
        source.getRootElement(root);

        // TODO: test that rootTag has the right type
        for(auto &attribute : rootTag.second.mAttributes) {
            
        }

        /**
         * Nodes:
         *  go through all child nodes read from xml
         *  for each node execute deserialization routine (ex push_back etc)
         *  call deserialize on it
         *
         */

        // Create corresponding child element
        CXmlElement xmlChildElement;

        xmlElement.createChild(xmlChildElement, pChild->getKind());

        // Propagate
        pChild->toXml(xmlChildElement, serializingContext);


       // for(auto &childTags : rootTag.nodes) {

       //     //parse
       // }
    }


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

    FillMe(int a, const std::string& b, bool c) :
        mA(a), mB(b), mC(c)
    {

    }

    int getA() { return mA; }
    std::string getB() { return mB; }
    bool getC() { return mC; }

    void setA(int a){ mA = a; }
    void setB(std::string b){ mB = b; }
    void setC(bool c){ mC = c; }

    std::string to_string()
    {
        return "A: " + std::to_string(mA) +
               ", B: " + mB + ", C: " + (mC ? "true" : "false");
    }

private:
    int mA = 0;
    std::string mB = "";
    bool mC = false;
};

int main()
{

    FillMe toFill{666, "huhu", true};

    Nodes nodes {
        { "FillMe",
            { { { "A",
                  Type<int>{},
                  [&toFill](){ return toFill.getA(); },
                  [&toFill](int a){ toFill.setA(a); } } },
                {
                 { "FillIt",
                     { { { "B",
                         Type<std::string>{},
                         [&toFill](){ return toFill.getB(); },
                         [&toFill](std::string b){ toFill.setB(b); } },
                       { "C",
                         Type<bool>{},
                         [&toFill](){ return toFill.getC(); },
                         [&toFill](bool c){ toFill.setC(c); } } },
                     {} /** empty child tag list */ } }
                } } } };


    return 0;
}


