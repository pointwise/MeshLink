/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "MeshAssociativity.h"

#include "MeshLinkParser_xerces.h"
#include "MeshLinkWriter_xerces.h"

#include <iostream>
#include <istream>
#include <iterator>
#include <sstream>

// DOM Parser
#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/Base64.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/util/XMLUni.hpp>

using namespace xercesc;

class DOMNodeTagNameFilter: public DOMNodeFilter {
public:
    DOMNodeTagNameFilter(const char *tag_name)
    {
        tagName_ = XMLString::transcode(tag_name);
    }
    ~DOMNodeTagNameFilter()
    {
        XMLString::release(&tagName_);
    }

    FilterAction 	acceptNode(const DOMNode *node) const
    {
        if (node->getNodeType() == DOMNode::ELEMENT_NODE) {
            const DOMElement *elem = dynamic_cast<const DOMElement *>(node);
            if (nullptr != elem &&
                XMLString::equals(tagName_, elem->getTagName())) {
                return FILTER_ACCEPT;
            }
        }
        return FILTER_REJECT;
    }

private:
    DOMNodeTagNameFilter();
    XMLCh *tagName_;
};




// Replaces xerces XMLString::transcode() method to take care of
// releasing the transcoded result automatically
template <typename T, typename U>
class XMLCopier {
public:
    XMLCopier(const T *input) :
        input_(input)
    {
        val_ = XMLString::transcode(input_);
    }

    ~XMLCopier()
    {
        XMLString::release(&val_);
    }

    operator U *()
    {
        return val_;
    }

    const U *convert() const
    {
        return val_;
    }

    bool empty()
    {
        return !(XMLString::stringLen(val_) > 0);
    }

private:
    const T *input_;
    U *val_;
};


// Very temporary - use in function calls only
#define X(str) (XMLCopier<char, XMLCh>(str)).convert()
#define Char(str) XMLCopier<XMLCh, char>(str).convert()

// Returns true if Element node is a Text Node or CDATASection Node
static bool
parseNode(DOMElement *node, std::vector<std::string> &atts, std::string &value)
{
    bool result = false;
    // Attributes vector contains names, to be replaced with parsed values
    int count; // used if data is base64
    bool base64 = false;
    bool quads = false;
    for (size_t i = 0; i < atts.size(); ++i) {
        XMLCopier<char, XMLCh> name(atts.at(i).c_str());
        XMLCopier<XMLCh, char> attr(node->getAttribute(name));
        if (!attr.empty()) {
            if (std::string("count") == atts.at(i)) {
                count = std::stoul((char*)attr);
            }
            if (std::string("format") == atts.at(i)) {
                if (std::string("base64") == std::string((char*)attr)) {
                    base64 = true;
                }
            }
            if (std::string("etype") == atts.at(i)) {
                if (std::string("Quad4") == std::string((char*)attr)) {
                    quads = true;
                }
            }
            atts[i] = attr;
        }
        else {
            atts[i] = std::string("");
        }
    }

    // Should be empty, to be filled
    DOMNode *child = node->getFirstChild();
    while (nullptr != child) {
        if (DOMNode::TEXT_NODE == child->getNodeType()) {
            XMLCopier<XMLCh, char> contents(child->getNodeValue());
            if (base64) {
                XMLSize_t length;
                XMLByte *data = Base64::decode((const XMLByte*)((char*)contents),
                    &length);
                int faceIndxSize = count * (quads ? 4 : 3);
                int *faceIndices = new int[faceIndxSize];
                memcpy(faceIndices, data, faceIndxSize * sizeof(int));
                for (int i = 0; i < faceIndxSize; ++i) {
                    if (0 < i) {
                        value += " ";
                    }
                    value += std::to_string(faceIndices[i]);
                }
            }
            else {
                value = contents;
            }
            result = true;
            break;
        }
        else if (DOMNode::CDATA_SECTION_NODE == child->getNodeType()) {
            // Get the data
            DOMCDATASection *cdata = dynamic_cast<DOMCDATASection*>(child);

            // Base64 decode
            XMLSize_t length;
            XMLByte *data = Base64::decodeToXMLByte(cdata->getData(), &length);
            value.assign((char*)data, length);
            result = true;
            break;
        }
        child = child->getNextSibling();
    }
    return result;
}


class SAXErrorHandler : public ErrorHandler {
public:
    SAXErrorHandler() {
        resetErrors();
    }
    virtual ~SAXErrorHandler() {}

    virtual void warning(const SAXParseException &ex) {
        ++warningCount_;
        std::cout << "SAX Warning message: " << std::endl;
        printException(ex);
    }

    virtual void error(const SAXParseException &ex) {
        ++errorCount_;
        std::cout << "SAX Error message: " << std::endl;
        printException(ex);
    }

    virtual void fatalError(const SAXParseException &ex) {
        ++fatalErrorCount_;
        std::cout << "SAX Fatal Error message: " << std::endl;
        printException(ex);
    }

    void printException(const SAXParseException &ex) {
        if (nullptr != ex.getMessage()) {
            XMLCopier<XMLCh, char> msg(ex.getMessage());
            std::cout << "\tMessage: " << msg << std::endl;
        }
        if (nullptr != ex.getPublicId()) {
            XMLCopier<XMLCh, char> msg(ex.getPublicId());
            std::cout << "\tPublic Id: " << msg << std::endl;
        }
        if (nullptr != ex.getSystemId()) {
            XMLCopier<XMLCh, char> msg(ex.getSystemId());
            std::cout << "\tSystem Id: " << msg << std::endl;
        }
        std::cout << "\tLine number: " << ex.getLineNumber() << std::endl;
        std::cout << "\tColumn number: " << ex.getColumnNumber() << std::endl;
    }

    void outputCounts() {
        std::cout << "Warnings: " << warningCount_ << std::endl;
        std::cout << "Errors: " << errorCount_ << std::endl;
        std::cout << "Fatal Errors: " << fatalErrorCount_ << std::endl;
    }

    virtual void resetErrors() {
        warningCount_ = 0;
        errorCount_ = 0;
        fatalErrorCount_ = 0;
    }

private:
    int warningCount_;
    int errorCount_;
    int fatalErrorCount_;
};





bool
MeshLinkParserXerces::parseAttributes(xercesc_3_2::DOMElement *root)
{
    if (!meshAssociativity_) { return false; }
    bool status = true;
    meshAssociativity_->clearAttributes();

    DOMNode *attr;
    {
        DOMNodeTagNameFilter attFilter("Attribute");
        DOMDocument *doc = root->getOwnerDocument();
        DOMNode *attNode;
        DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT, 
            &attFilter, false);
        while ((attNode = iter->nextNode()) != nullptr) {
            DOMElement *elem = dynamic_cast<DOMElement *>(attNode);
            if (nullptr != elem) {

                int attid;
                std::string name;
                std::string contents;

                XMLCopier<char, XMLCh> nameName("name");
                XMLCopier<char, XMLCh> attidName("attid");

                DOMNamedNodeMap *attMap = attNode->getAttributes();
                if (nullptr == attMap) {
                    std::cout << "Attribute node not an element" << std::endl;
                    continue;
                }

                // required atts
                attr = attMap->getNamedItem(attidName);
                if (nullptr == attr) {
                    std::cout << "Attribute missing attid attribute" << std::endl;
                    continue;
                }
                attid = XMLString::parseInt(attr->getNodeValue());
                const MeshLinkAttribute *existing =
                    meshAssociativity_->getAttributeByID(attid);
                if (existing) {
                    std::cout << "Attribute reuses existing attid attribute \"" <<
                        attid << "\"" << std::endl;
                    continue;
                }

                // optional atts
                attr = attMap->getNamedItem(nameName);
                if (nullptr != attr) {
                    XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                    name = (char *)xmlStr;
                }

                // contents
                DOMNode *child = attNode->getFirstChild();
                while (nullptr != child) {
                    if (DOMNode::TEXT_NODE == child->getNodeType()) {
                        XMLCopier<XMLCh, char> xmlStr(child->getNodeValue());
                        contents = (char *)xmlStr;
                        break;
                    }
                    child = child->getNextSibling();
                }

                if (contents.empty()) {
                    std::cout << "Attribute missing content" << std::endl;
                    continue;
                }
                else {
                    MeshLinkAttribute mlAtt((MLINT)attid, name, contents, false,
                        *meshAssociativity_);
                    if (mlAtt.isValid()) {
                        meshAssociativity_->addAttribute(mlAtt);
                    }
                }
            }
        }
        iter->release();
    }

    // AttributeGroup elements
    {
        DOMNodeTagNameFilter attFilter("AttributeGroup");
        DOMDocument *doc = root->getOwnerDocument();
        DOMNode *attNode;
        DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        while ((attNode = iter->nextNode()) != nullptr) {
            DOMElement *elem = dynamic_cast<DOMElement *>(attNode);
            if (nullptr != elem) {

                int attid;
                std::string name;
                std::string contents;

                XMLCopier<char, XMLCh> nameName("name");
                XMLCopier<char, XMLCh> attidName("attid");

                DOMNamedNodeMap *attMap = attNode->getAttributes();
                if (nullptr == attMap) {
                    std::cout << "AttributeGroup node not an element" << std::endl;
                    continue;
                }

                // required atts
                attr = attMap->getNamedItem(attidName);
                if (nullptr == attr) {
                    std::cout << "AttributeGroup missing attid attribute" <<
                        std::endl;
                    continue;
                }
                attid = XMLString::parseInt(attr->getNodeValue());
                const MeshLinkAttribute *existing =
                    meshAssociativity_->getAttributeByID(attid);
                if (existing) {
                    std::cout << "AttributeGroup reuses existing attid attribute \"" <<
                        attid << "\"" << std::endl;
                    continue;
                }

                // optional atts
                attr = attMap->getNamedItem(nameName);
                if (nullptr != attr) {
                    XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                    name = (char *)xmlStr;
                }

                // contents
                DOMNode *child = attNode->getFirstChild();
                while (nullptr != child) {
                    if (DOMNode::TEXT_NODE == child->getNodeType()) {
                        XMLCopier<XMLCh, char> xmlStr(child->getNodeValue());
                        contents = (char *)xmlStr;
                        break;
                    }
                    child = child->getNextSibling();
                }

                if (contents.empty()) {
                    std::cout << "AttributeGroup missing content" << std::endl;
                    continue;
                }
                else {
                    MeshLinkAttribute mlAtt((MLINT)attid, name, contents, true,
                        *meshAssociativity_);
                    if (mlAtt.isValid()) {
                        meshAssociativity_->addAttribute(mlAtt);
                    }
                }
            }
        }
        iter->release();
    }

    return status;
}

bool
MeshLinkParserXerces::parsePeriodicInfo(xercesc_3_2::DOMElement *root)
{
    if (!meshAssociativity_) { return false; }
    bool status = true;
    meshAssociativity_->clearTransforms();
    meshAssociativity_->clearMeshElementLinkages();

    DOMNode *attr;
    {
        DOMNodeTagNameFilter attFilter("Transform");
        DOMDocument *doc = root->getOwnerDocument();
        DOMNode *attNode;
        DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        while ((attNode = iter->nextNode()) != nullptr) {
            DOMElement *elem = dynamic_cast<DOMElement *>(attNode);
            if (nullptr != elem) {

                int xid;
                std::string name;
                std::string contents;

                XMLCopier<char, XMLCh> nameName("name");
                XMLCopier<char, XMLCh> xidName("xid");

                DOMNamedNodeMap *attMap = attNode->getAttributes();
                if (nullptr == attMap) {
                    std::cout << "Attribute node not an element" << std::endl;
                    continue;
                }

                // required atts
                attr = attMap->getNamedItem(xidName);
                if (nullptr == attr) {
                    std::cout << "Transform missing xid attribute" << std::endl;
                    continue;
                }
                xid = XMLString::parseInt(attr->getNodeValue());
                const MeshLinkTransform *existing =
                    meshAssociativity_->getTransformByID(xid);
                if (existing) {
                    std::cout << "Transform reuses existing xid attribute \"" <<
                        xid << "\"" << std::endl;
                    continue;
                }

                // optional name attribute
                attr = attMap->getNamedItem(nameName);
                if (nullptr != attr) {
                    XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                    name = (char *)xmlStr;
                }

                // contents
                DOMNode *child = attNode->getFirstChild();
                while (nullptr != child) {
                    if (DOMNode::TEXT_NODE == child->getNodeType()) {
                        XMLCopier<XMLCh, char> xmlStr(child->getNodeValue());
                        contents = (char *)xmlStr;
                        break;
                    }
                    child = child->getNextSibling();
                }

                if (contents.empty()) {
                    std::cout << "Transform missing content" << std::endl;
                    continue;
                }
                else {
                    MeshLinkTransform mlXform((MLINT)xid, name, contents,
                        *meshAssociativity_);
                    if (mlXform.isValid()) {
                        // optional aref attribute
                        DOMNode *arefAttr = attMap->getNamedItem(X("aref"));
                        if (nullptr != arefAttr) {
                            const XMLCh *valStr = arefAttr->getNodeValue();
                            mlXform.setAref(XMLString::parseInt(valStr));
                        }

                        // stores a copy
                        meshAssociativity_->addTransform(mlXform);
                    }
                }
            }
        }
        iter->release();
    }

    {
        DOMNodeTagNameFilter attFilter("MeshElementLinkage");
        DOMDocument *doc = root->getOwnerDocument();
        DOMNode *attNode;
        DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        while ((attNode = iter->nextNode()) != nullptr) {
            DOMElement *elem = dynamic_cast<DOMElement *>(attNode);
            if (nullptr != elem) {

                std::string name;
                std::string srcEntRef;
                std::string tgtEntRef;

                XMLCopier<char, XMLCh> nameName("name");
                XMLCopier<char, XMLCh> srcEntRefName("sourceEntityRef");
                XMLCopier<char, XMLCh> tgtEntRefName("targetEntityRef");

                DOMNamedNodeMap *attMap = attNode->getAttributes();
                if (nullptr == attMap) {
                    std::cout << "Attribute node not an element" << std::endl;
                    continue;
                }

                // required atts
                attr = attMap->getNamedItem(srcEntRefName);
                if (nullptr == attr) {
                    std::cout << "MeshElementLinkage missing sourceEntityRef attribute" << std::endl;
                    continue;
                }
                else {
                    XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                    srcEntRef = (char *)xmlStr;
                }

                attr = attMap->getNamedItem(tgtEntRefName);
                if (nullptr == attr) {
                    std::cout << "MeshElementLinkage missing targetEntityRef attribute" << std::endl;
                    continue;
                }
                else {
                    XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                    tgtEntRef = (char *)xmlStr;
                }


                // optional name attribute
                attr = attMap->getNamedItem(nameName);
                if (nullptr != attr) {
                    XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                    name = (char *)xmlStr;
                }

                
                MeshElementLinkage *mlLink = new MeshElementLinkage(name, srcEntRef,
                    tgtEntRef, *meshAssociativity_);
                if (mlLink->isValid() &&
                    // stores and takes ownership of pointer
                    meshAssociativity_->addMeshElementLinkage(mlLink)
                    ) {
                    // optional xref attribute
                    DOMNode *xrefAttr = attMap->getNamedItem(X("xref"));
                    if (nullptr != xrefAttr) {
                        const XMLCh *valStr = xrefAttr->getNodeValue();
                        if (!mlLink->setXref(XMLString::parseInt(valStr), *meshAssociativity_)) {
                            delete mlLink; mlLink = NULL;
                            std::cout << "MeshElementLinkage missing xref transform" << std::endl;
                            continue;
                        }
                    }
                    // optional aref attribute
                    DOMNode *arefAttr = attMap->getNamedItem(X("aref"));
                    if (nullptr != arefAttr) {
                        const XMLCh *valStr = arefAttr->getNodeValue();
                        mlLink->setAref(XMLString::parseInt(valStr));
                    }
                }
                else {
                    delete mlLink; mlLink = NULL;
                    std::cout << "MeshElementLinkage missing source or target entity " << std::endl;
                    continue;
                }
            }
        }
        iter->release();
    }

    return status;
}



// Valiadate MeshLink file against the schema
bool
MeshLinkParserXerces::validate(const std::string &fileName,
    const std::string &schemaName)
{
    bool result = true;
    // Note: anything from xerces must be created after successful
    // initialization, and must be destroyed before termination - that
    // includes anything on the stack, so, don't use the stack
    // (includes std::shared_ptr)
    try {
        XMLPlatformUtils::Initialize();
        std::cout << "Successfully initialized xerces API." << std::endl;
    }
    catch (const XMLException &) {
        std::cout << "Failed to initialize xerces API." << std::endl;
        return 1;
    }

    // No std::shared_ptr here - gets released after Terminate(),
    // which throws a memory exception
    XercesDOMParser* parser = new XercesDOMParser();
    // std::shared_ptr okay here - parser does NOT take ownership
    SAXErrorHandler* errHandler(new SAXErrorHandler());
    parser->setErrorHandler(errHandler);

    if (!schemaName.empty()) {
        // Use explicitly defined schema file, not the schemaLocation
        // specified in meshlink file
        try {
            XMLCopier<char, XMLCh> sc(schemaName.c_str());
            parser->useCachedGrammarInParse(true);
            Grammar *schema = parser->loadGrammar(sc, Grammar::SchemaGrammarType,
                true);
            if (nullptr == schema) {
                std::cout << "Schema not loaded. Check for file." << std::endl;
                errHandler->outputCounts();
                result = false;
            }
            else {
                std::cout << "Successfully loaded schema." << std::endl;
                XMLCopier<XMLCh, char> targetNS(schema->getTargetNamespace());
                std::cout << "Target namespace: " << targetNS << std::endl;
                errHandler->resetErrors();
            }
        }
        catch (const XMLException &toCatch) {
            XMLCopier<XMLCh, char> msg(toCatch.getMessage());
            std::cout << "Exception message: " << msg << std::endl;
            result = false;
        }
        catch (const SAXException &toCatch) {
            XMLCopier<XMLCh, char> msg(toCatch.getMessage());
            std::cout << "SAX Exception message: " << msg << std::endl;
            result = false;
        }
        catch (const DOMException &toCatch) {
            XMLCopier<XMLCh, char> msg(toCatch.getMessage());
            std::cout << "DOM Exception message: " << msg << std::endl;
            result = false;
        }
    }

    if (result) {
        try {
            if (parser->isUsingCachedGrammarInParse()) {
                std::cout << "Using cached grammer in parse." << std::endl;
            }
            else {
                std::cout << "Not using cached grammer in parse." << std::endl;
            }
            parser->setValidationSchemaFullChecking(true);
            parser->setValidationScheme(XercesDOMParser::Val_Always);
            parser->setDoNamespaces(true);
            parser->setDoSchema(true);
            std::cout << "**************Validating..." << std::endl;
            parser->parse(fileName.c_str());
            std::cout << "**************Validation complete." << std::endl;
        }
        catch (const XMLException &toCatch) {
            XMLCopier<XMLCh, char> msg(toCatch.getMessage());
            std::cout << "Exception message: " << msg << std::endl;
            result = false;
        }
        catch (const DOMException &toCatch) {
            XMLCopier<XMLCh, char> msg(toCatch.getMessage());
            std::cout << "DOM Exception message: " << msg << std::endl;
            result = false;
        }
        errHandler->outputCounts();
    }

    delete parser;
    delete errHandler;

    try {
        XMLPlatformUtils::Terminate();
        std::cout << "Successfully terminated xerces API." << std::endl;
    }
    catch (const XMLException &) {
        std::cout << "Failed to terminate xerces API." << std::endl;
        return false;
    }

    return result;
}


// Parse GeometryReference node
bool
parseGeomRefDOM(DOMNode *geometryReferenceDOM, GeometryGroup &group)
{
    DOMNamedNodeMap *attMap = geometryReferenceDOM->getAttributes();
    if (nullptr == attMap) {
        std::cout << "GeometryReference node not an element" << std::endl;
        return false;
    }
    // geometry id - 'gid' attribute
    DOMNode *gidAttr = attMap->getNamedItem(X("gid"));
    if (nullptr == gidAttr) {
        std::cout << "GeometryReference missing gid attribute" << std::endl;
        return false;
    }
    const XMLCh *gid = gidAttr->getNodeValue();
    group.setID(XMLString::parseInt(gid));

    // entity name - only one per DOMNode - 'ref' attribute
    DOMNode *refAttr = attMap->getNamedItem(X("ref"));
    if (nullptr == refAttr) {
        std::cout << "GeometryReference missing ref attribute" << std::endl;
        return false;
    }
    XMLCopier<XMLCh, char> ref(refAttr->getNodeValue());
    group.addEntityName((char *)ref);

    // always need a name - default to 'ref' value
    group.setName((char *)ref);

    // optional 'name' attribute
    DOMNode *nameAttr = attMap->getNamedItem(X("name"));
    if (nullptr != nameAttr) {
        XMLCopier<XMLCh, char> xmlStr(nameAttr->getNodeValue());
        group.setName((char *)xmlStr);
    }

    // optional aref attribute
    DOMNode *arefAttr = attMap->getNamedItem(X("aref"));
    if (nullptr != arefAttr) {
        const XMLCh *valStr = arefAttr->getNodeValue();
        group.setAref( XMLString::parseInt(valStr) );
    }

    return true;
}



// Parse GeometryGroup node
bool
parseGeomGroupDOM(DOMNode *geometryGroupDOM,
    MeshAssociativity *meshAssociativity,
    GeometryGroup &group)
{
    DOMNamedNodeMap *attMap = geometryGroupDOM->getAttributes();
    if (nullptr == attMap) {
        std::cout << "GeometryGroup node not an element" << std::endl;
        return false;
    }
    // Geometry group id - 'gid' attribute
    DOMNode *gidAttr = attMap->getNamedItem(X("gid"));
    if (nullptr == gidAttr) {
        std::cout << "GeometryGroup missing gid attribute" << std::endl;
        return false;
    }
    const XMLCh *gidStr = gidAttr->getNodeValue();
    int gid = XMLString::parseInt(gidStr);

    // Get content (list of gid integers)
    std::string grefstr;
    DOMNode *child = geometryGroupDOM->getFirstChild();
    while (nullptr != child) {
        if (DOMNode::TEXT_NODE == child->getNodeType()) {
            XMLCopier<XMLCh, char> contents(child->getNodeValue());
            grefstr = contents;
            break;
        }
        child = child->getNextSibling();
    }
    if (grefstr.empty()) {
        std::cout << "GeometryGroup with gid=\"" << gid <<
            "\" missing content" << std::endl;
        return false;
    }

    // Construct a stream from the string
    std::stringstream strstr(grefstr);

    // Use stream iterators to copy the stream to the vector as
    // whitespace separated strings
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    std::vector<std::string> grefs(it, end);

    // Always need a name - default to "geom_group_<gid>"
    std::ostringstream s;  s << gid;
    std::string name = std::string("geom_group_") + s.str();
    group.setName( name.c_str() );
    group.setID(gid);

    size_t i;
    for (i = 0; i < grefs.size(); ++i) {
        std::string &gref = grefs[i];
        int grefID = MESH_TOPO_INVALID_REF;
        try {
            grefID = std::stoi(gref);
        }
        catch (...) {
            std::cout << "GeometryGroup invalid content \"" << gref << "\"" <<
                std::endl;
            return false;
        }
        // Add to group
        group.addGID(grefID);
        GeometryGroup *refgroup = meshAssociativity->getGeometryGroupByID(grefID);
        if (refgroup) {
            refgroup->setGroupID(gid);
            const std::vector<std::string> &ents = refgroup->getEntityNames();
            std::vector<std::string>::const_iterator iter;
            for (iter = ents.cbegin(); iter != ents.cend(); ++iter) {
                group.addEntityName((*iter).c_str());
            }
        }
        else {
            std::cout << "GeometryGroup unknown gid in content \"" << grefID <<
                "\"" << std::endl;
            return false;
        }
    }

    // optional 'name' attribute
    DOMNode *nameAttr = attMap->getNamedItem(X("name"));
    if (nullptr != nameAttr) {
        XMLCopier<XMLCh, char> xmlStr(nameAttr->getNodeValue());
        group.setName((char *)xmlStr);
    }

    // optional aref attribute
    DOMNode *arefAttr = attMap->getNamedItem(X("aref"));
    if (nullptr != arefAttr) {
        const XMLCh *valStr = arefAttr->getNodeValue();
        group.setAref(XMLString::parseInt(valStr));
    }

    return true;
}


// Parse GeometryFile element and child GeometryReference and
// GeometryGroup elements
bool
MeshLinkParserXerces::parseGeometryRefs(DOMElement *root)
{
    if (!meshAssociativity_) {
        return false;
    }
    bool result = true;
    DOMDocument *doc = root->getOwnerDocument();

    // Loop through GeometryFile nodes parsing GeometryReference elements
    {
        DOMNodeTagNameFilter attFilter("GeometryFile");
        DOMNode *node;
        DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        while (result && (node = iter->nextNode()) != nullptr) {
            DOMElement *elem = dynamic_cast<DOMElement *>(node);
            if (nullptr != elem) {

                std::string filename, aref;

                if (elem->hasAttribute(X("filename"))) {
                    XMLCopier<XMLCh, char> xmlStr(elem->getAttribute(X("filename")));
                    filename = (char *)xmlStr;
                }
                else {
                    std::cout << "GeometryFile node: no filename attribute." <<
                        std::endl;
                    return false;
                }
                if (elem->hasAttribute(X("aref"))) {
                    XMLCopier<XMLCh, char> xmlStr(elem->getAttribute(X("aref")));
                    aref = (char *)xmlStr;
                }

                GeometryFile geomFile(filename, aref);

                // Geometry references for this file
                {
                    DOMNodeTagNameFilter attFilter2("GeometryReference");
                    DOMNode *geomRef;
                    DOMNodeIterator *iter2 = doc->createNodeIterator(elem, DOMNodeFilter::SHOW_ELEMENT,
                        &attFilter2, false);
                    while (result && (geomRef = iter2->nextNode()) != nullptr) {
                        GeometryGroup geom_group;
                        if ((result = parseGeomRefDOM(geomRef, geom_group))) {
                            meshAssociativity_->addGeometryGroup(geom_group);
                            geomFile.addGeometryGroupID(geom_group.getID());
                        }
                    }
                    iter2->release();
                }
                // Since we add via value instead of pointer, we better
                // add it after we've made all our changes/additions
                meshAssociativity_->addGeometryFile(geomFile);
            }
        }
        iter->release();
    }

    // GeometryGroup content is list of attids which must match
    // those defined by GeometryReference elements above.
    {
        DOMNodeTagNameFilter attFilter("GeometryGroup");
        DOMNode *geomGroup;
        DOMNodeIterator *iter = doc->createNodeIterator(root, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        while (result && (geomGroup = iter->nextNode()) != nullptr) {
            GeometryGroup geom_group;
            if ((result = parseGeomGroupDOM(geomGroup, meshAssociativity_,
                geom_group))) {
                meshAssociativity_->addGeometryGroup(geom_group);
            }
        }
        iter->release();
    }

    return result;
}


// Parse single MeshFile element and children
bool
MeshLinkParserXerces::parseMeshFile(xercesc_3_2::DOMElement *meshFile)
{
    if (!meshAssociativity_) { return false; }

    std::string filename, aref;

    if (meshFile->hasAttribute(X("filename"))) {
        XMLCopier<XMLCh, char> xmlStr(meshFile->getAttribute(X("filename")));
        filename = (char *)xmlStr;
    }
    else {
        std::cout << "MeshFile node: no filename attribute." << std::endl;
        return false;
    }
    if (meshFile->hasAttribute(X("aref"))) {
        XMLCopier<XMLCh, char> xmlStr(meshFile->getAttribute(X("aref")));
        aref = (char *)xmlStr;
    }

    MeshFile mFile(filename, aref);

    // Model References
    bool result = true;
    int count = 0;
    {
        DOMNodeTagNameFilter attFilter("MeshModelReference");
        DOMNode *node;
        DOMDocument *doc = meshFile->getOwnerDocument();
        DOMNodeIterator *iter = doc->createNodeIterator(meshFile, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        DOMElement *modelRef;
        while (result && (node = iter->nextNode()) != nullptr) {
            ++count;
            if (DOMNode::ELEMENT_NODE == node->getNodeType()) {
                modelRef = dynamic_cast<DOMElement*>(node);
                result = parseMeshRefModel(mFile, modelRef);
            }
        }
        iter->release();
    }

    if (0 == count) {
        std::cout << "MeshFile node: no model references." << std::endl;
        return false;
    }

    meshAssociativity_->addMeshFile(mFile);

    if (!result) {
        std::cout << "MeshFile node: problem parsing mesh reference." <<
            std::endl;
        return false;
    }

    return true;
}


#define ParseMeshContainer(MeshModel, MeshObj)                          \
    {                                                                   \
        DOMNodeTagNameFilter attFilter(#MeshObj);                       \
        DOMNode *node;                                                  \
        DOMDocument *doc = MeshModel->getOwnerDocument();               \
        DOMNodeIterator *iter = doc->createNodeIterator(MeshModel,      \
            DOMNodeFilter::SHOW_ELEMENT, &attFilter, false);            \
        DOMElement *meshObj;                                            \
        XMLSize_t i = 0;                                                \
        while (result && (node = iter->nextNode()) != nullptr) {        \
            ++i;                                                        \
            if (DOMNode::ELEMENT_NODE == node->getNodeType()) {         \
                meshObj = dynamic_cast<DOMElement*>(node);              \
                result = parse##MeshObj(meshModel, meshObj);            \
            }                                                           \
        }                                                               \
        iter->release();                                                \
        if (!result) {                                                  \
            std::cout << "MeshModelReference node: problem "            \
                "parsing " << #MeshObj << " " << i << std::endl;        \
            return false;                                               \
        }                                                               \
    }


#define ParseMeshContainerNoRecurse(MeshModel, MeshObj)                 \
    {                                                                   \
        DOMNodeTagNameFilter attFilter(#MeshObj);                       \
        DOMNode *node;                                                  \
        DOMDocument *doc = MeshModel->getOwnerDocument();               \
        DOMNodeIterator *iter = doc->createNodeIterator(MeshModel,      \
            DOMNodeFilter::SHOW_ELEMENT, &attFilter, false);            \
        DOMElement *meshObj;                                            \
        XMLSize_t i = 0;                                                \
        while (result && (node = iter->nextNode()) != nullptr) {        \
            ++i;                                                        \
            DOMNode *parentNode = node->getParentNode();                \
            if (parentNode && DOMNode::ELEMENT_NODE == parentNode->getNodeType()) { \
                DOMElement *parentElem = dynamic_cast<DOMElement*>(parentNode); \
                if (parentElem != MeshModel) continue;                  \
            }                                                           \
            if (DOMNode::ELEMENT_NODE == node->getNodeType()) {         \
                meshObj = dynamic_cast<DOMElement*>(node);              \
                result = parse##MeshObj(meshModel, meshObj);            \
            }                                                           \
        }                                                               \
        iter->release();                                                \
        if (!result) {                                                  \
            std::cout << "MeshModelReference node: problem "            \
                "parsing " << #MeshObj << " " << i << std::endl;        \
            return false;                                               \
        }                                                               \
    }


bool
MeshLinkParserXerces::parseMeshRefModel(MeshFile &meshFile,
    xercesc_3_2::DOMElement *modelRef)
{
    if (!meshAssociativity_) { return false; }

    bool mapID = false;
    int mid = MESH_TOPO_INVALID_REF;
    std::string name;
    int aref = MESH_TOPO_INVALID_REF;
    int gref = MESH_TOPO_INVALID_REF;
    std::string ref;
    // Required
    if (modelRef->hasAttribute(X("ref"))) {
        XMLCopier<XMLCh, char> xmlStr(modelRef->getAttribute(X("ref")));
        ref = std::string(xmlStr);
    }
    else {
        // Bad model store
        std::cout << "MeshModelReference: missing ref attribute." << std::endl;
        return false;
    }

    if (modelRef->hasAttribute(X("mid"))) {
        mid = XMLString::parseInt(modelRef->getAttribute(X("mid")));

        MeshModel *existingModel = meshAssociativity_->getMeshModelByID(mid);
        if (existingModel) {
            // Can't have multiple mid values
            std::cout << "MeshModelReference: mid identifier already in "
                "use." << std::endl;
            return false;
        }
        mapID = true;
    }
    if (modelRef->hasAttribute(X("aref"))) {
        aref = XMLString::parseInt(modelRef->getAttribute(X("aref")));
    }
    if (modelRef->hasAttribute(X("gref"))) {
        gref = XMLString::parseInt(modelRef->getAttribute(X("gref")));
    }
    if (modelRef->hasAttribute(X("name"))) {
        XMLCopier<XMLCh, char> xmlStr(modelRef->getAttribute(X("name")));
        name = std::string(xmlStr);

        MeshModel *existingModel =
            meshAssociativity_->getMeshModelByName(name.c_str());
        if (existingModel) {
            // Can't have multiple name values
            std::cout << "MeshModelReference: name identifier already in "
                "use." << std::endl;
            return false;
        }
    }

    MeshModel* meshModel = new MeshModel(ref, mid, aref, gref, name);

    bool result = meshAssociativity_->addMeshModel(meshModel, mapID);

    if (!result) {
        // bad model store
        std::cout << "MeshModelReference: error storing model." << std::endl;
        delete meshModel;
        return false;
    }
    else {
        // Add to MeshFile
        meshFile.addModelRef(ref);

        // Parse MeshSheets
        ParseMeshContainer(modelRef, MeshSheet);
        ParseMeshContainer(modelRef, MeshSheetReference);

        // Parse MeshStrings
        ParseMeshContainer(modelRef, MeshString);
        ParseMeshContainer(modelRef, MeshStringReference);

        // Parse MeshPoints

        // Param vertices (do prior to MeshPointRef)
        {
            DOMNodeTagNameFilter attFilter("ParamVertex");
            DOMNode *node;
            DOMDocument *doc = modelRef->getOwnerDocument();
            DOMNodeIterator *iter = doc->createNodeIterator(modelRef, DOMNodeFilter::SHOW_ELEMENT,
                &attFilter, false);
            while (result && (node = iter->nextNode()) != nullptr) {

                DOMNode *parentNode = node->getParentNode();
                if (parentNode && DOMNode::ELEMENT_NODE == parentNode->getNodeType()) {
                    DOMElement *parentElem = dynamic_cast<DOMElement*>(parentNode);
                    if (parentElem != modelRef) {
                        // don't allow recursion to children of MeshModelRef
                        // we want those paramVerts to be handled by the
                        // MeshString and MeshSheet parser
                        continue;
                    }
                }
                DOMElement *vertNode = dynamic_cast<DOMElement*>(node);
                result = parseParamVertex(meshModel, vertNode);
            }
            iter->release();
        }

        if (!result) {
            std::cout << "MeshModel: error parsing vertex." << std::endl;
            return false;
        }

        ParseMeshContainerNoRecurse(modelRef, MeshPointReference);


        if (result && verbose_level_ > 0) {
            DOMAttr *attNode = modelRef->getAttributeNode(X("name"));
            if (attNode) {
                XMLCopier<XMLCh, char> val(attNode->getValue());
                printf("MeshModel %s geometry associations:\n", (char *)val);
            }
            printf("%8" MLINT_FORMAT " mesh edges\n", meshModel->getNumEdges());
            printf("%8" MLINT_FORMAT " mesh faces\n", meshModel->getNumFaces());
        }
    }

    return true;
}


MeshLinkParserXerces::~MeshLinkParserXerces()
{
    //clearMeshObjs();
}


bool
MeshLinkParserXerces::parseMeshObject(MeshModel *model,
    MeshTopo* meshTopo,
    xercesc_3_2::DOMElement *xmlObj,
    const char *objName,
    ParseMeshObjMap &parseMeshObjMap, bool &mapID)
{
    if (NULL == meshTopo) { return false; }
    MeshString *meshString = dynamic_cast<MeshString *> (meshTopo);
    MeshSheet *meshSheet = dynamic_cast<MeshSheet *> (meshTopo);

    mapID = false;
    if (xmlObj->hasAttribute(X("mid"))) {
        meshTopo->setID( XMLString::parseInt(xmlObj->getAttribute(X("mid"))) );

        if (meshString) {
            MeshString *existingString =
                model->getMeshStringByID(meshString->getID());
            if (existingString) {
                // Can't have multiple mid values
                std::cout << "MeshString: mid identifier already in use." <<
                    std::endl;
                return false;
            }
            mapID = true;
        }
        if (meshSheet) {
            MeshSheet *existingSheet = model->getMeshSheetByID(meshSheet->getID());
            if (existingSheet) {
                // Can't have multiple mid values
                std::cout << "MeshSheet: mid identifier already in use." <<
                    std::endl;
                return false;
            }
            mapID = true;
        }
    }
    if (xmlObj->hasAttribute(X("name"))) {
        XMLCopier<XMLCh, char> name(xmlObj->getAttribute(X("name")));
        meshTopo->setName( name );

        if (meshString) {
            MeshString *existingString = model->getMeshStringByName(meshString->getName());
            if (existingString) {
                // Can't have multiple name values
                std::cout << "MeshString: name identifier already in use." <<
                    std::endl;
                return false;
            }
        }
        if (meshSheet) {
            MeshSheet *existingSheet = model->getMeshSheetByName(meshSheet->getName());
            if (existingSheet) {
                // Can't have multiple name values
                std::cout << "MeshSheet: name identifier already in use." <<
                    std::endl;
                return false;
            }
        }
    }
    else {
        if (meshString) {
            meshString->setName(meshString->getNextName().c_str());
        }
        if (meshSheet) {
            meshSheet->setName(meshSheet->getNextName().c_str());
        }
    }
    if (xmlObj->hasAttribute(X("gref"))) {
        meshTopo->setGref(XMLString::parseInt(xmlObj->getAttribute(X("gref"))));
    }
    if (xmlObj->hasAttribute(X("aref"))) {
        meshTopo->setAref(XMLString::parseInt(xmlObj->getAttribute(X("aref"))));
    }


    if (xmlObj->hasAttribute(X("ref"))) {
        XMLCopier<XMLCh, char> ref(xmlObj->getAttribute(X("ref")));
        meshTopo->setRef(ref);

        if (meshString) {
            MeshString *existingString =
                model->getMeshStringByRef(meshString->getRef());
            if (existingString) {
                // Can't have multiple narefme values
                std::cout << "MeshStringReference: ref identifier already in use." <<
                    std::endl;
                return false;
            }
        }
        if (meshSheet) {
            MeshSheet *existingSheet = model->getMeshSheetByRef(meshSheet->getRef());
            if (existingSheet) {
                // Can't have multiple ref values
                std::cout << "MeshSheetReference: ref identifier already in use." <<
                    std::endl;
                return false;
            }
        }
    }

    bool result = true;

    // Param vertices
    {
        DOMNodeTagNameFilter attFilter("ParamVertex");
        DOMNode *node;
        DOMDocument *doc = xmlObj->getOwnerDocument();
        DOMNodeIterator *iter = doc->createNodeIterator(xmlObj, DOMNodeFilter::SHOW_ELEMENT,
            &attFilter, false);
        while (result && (node = iter->nextNode()) != nullptr) {
            DOMElement *vertNode = dynamic_cast<DOMElement*>(node);
            result = parseParamVertex(meshTopo, vertNode);
        }
        iter->release();
    }
    if (!result) {
        std::cout << objName << ": error parsing vertex." << std::endl;
        return false;
    }

    // Child mesh objects
    ParseMeshObjMap::iterator meshObjIter;
    XMLSize_t totalItems = 0;
    for (meshObjIter = parseMeshObjMap.begin();
             meshObjIter != parseMeshObjMap.end(); ++meshObjIter) {
        const char *meshObjName = meshObjIter->first;
        pParseMeshObj parseMeshObj = meshObjIter->second;
        {
            DOMNodeTagNameFilter attFilter(meshObjName);
            DOMNode *node;
            DOMDocument *doc = xmlObj->getOwnerDocument();
            DOMNodeIterator *iter = doc->createNodeIterator(xmlObj, DOMNodeFilter::SHOW_ELEMENT,
                &attFilter, false);
            while (result && (node = iter->nextNode()) != nullptr) {
                ++totalItems;
                DOMElement *itemNode = dynamic_cast<DOMElement*>(node);
                result = (this->*parseMeshObj)(model, meshTopo, itemNode);
            }
            iter->release();
        }
        if (!result) {
            std::cout << objName << ": error parsing " << meshObjName << "." <<
                std::endl;
            return false;
        }
    }

    if (0 == totalItems) {
        std::cout << objName << ": error missing content." <<
            std::endl;
        return false;
    }

    return true;
}

bool
MeshLinkParserXerces::parseMeshStringReference(MeshModel *model,
    xercesc_3_2::DOMElement *meshStringNode)
{
    // Handle as MeshString
    return parseMeshString(model, meshStringNode);
}


bool
MeshLinkParserXerces::parseMeshString(MeshModel *model,
    xercesc_3_2::DOMElement *meshStringNode)
{
    // map tag name -> parsing function pointer
    ParseMeshObjMap parseMap;
    parseMap["MeshEdge"] = &MeshLinkParserXerces::parseMeshEdge;
    parseMap["MeshEdgeReference"] = &MeshLinkParserXerces::parseMeshEdgeReference;

    // Create new MeshString to hold parsed edge data
    // Becomes the responsibility of the MeshModel if
    // parsing is successful.
    MeshString* meshString(new MeshString());
    MeshTopo* meshTopo = meshString;
    bool mapID = false;

    bool result = parseMeshObject(model, meshTopo, meshStringNode, "MeshString",
        parseMap, mapID);

    if (result) {
        result = model->addMeshString(meshString, mapID);
        if (!result) {
            printf("MeshString: error storing\n   %s\n",
                    (char*)meshStringNode->getNodeName());
        }
    }

    if (!result) {
        delete meshString;
        return result;
    }

    if (verbose_level_ > 0) {
        printf("MeshString %s geometry associations:\n", meshString->getName().c_str());
        printf("%8" MLINT_FORMAT " parametric vertices\n", meshString->getNumParamVerts());
        printf("%8" MLINT_FORMAT " mesh edges\n", meshString->getNumEdges());
    }

    return true;
}


bool
MeshLinkParserXerces::parseMeshPointReference(MeshModel *model,
    xercesc_3_2::DOMElement *meshPointNode)
{
    MeshTopo* parentMeshTopo = NULL;
    return parseMeshPointReference(model, parentMeshTopo, meshPointNode);
}

bool
MeshLinkParserXerces::parseMeshPointReference(MeshModel *model,
    MeshTopo* parentMeshTopo,
    xercesc_3_2::DOMElement *meshPointNode)
{
    MeshSheet* meshSheet =
        dynamic_cast <MeshSheet*> (parentMeshTopo);
    MeshString* meshString =
        dynamic_cast <MeshString*> (parentMeshTopo);

    std::vector<std::string> attributeNames;

    // Optional attributes
    attributeNames.push_back("mid");
    attributeNames.push_back("format");
    attributeNames.push_back("count");
    attributeNames.push_back("aref");
    attributeNames.push_back("gref");
    attributeNames.push_back("name");
    std::vector<std::string> attrs = attributeNames;
    std::string value;
    if (parseNode(meshPointNode, attrs, value)) {

        // Optional mid attribute
        int iattr = 0;
        int mid = MESH_TOPO_INVALID_REF;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // Optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format) {
            std::cout << "MeshPointReference: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // Optional count attribute
        ++iattr;
        int count = 1;
        if (attrs[iattr] != "") {
            std::istringstream isCount(attrs[iattr]);
            isCount >> count;
            if (count <= 0 || count > 1e12) {
                std::cout << "MeshPointReference: illegal count value: " <<
                    count << std::endl;
                return false;
            }
        }

        // Optional aref attribute
        ++iattr;
        int aref = MESH_TOPO_INVALID_REF;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // Optional gref attribute
        ++iattr;
        int gref = MESH_TOPO_INVALID_REF;
        if (parentMeshTopo) {
            gref = (int)parentMeshTopo->getGref();  // Default to parent's Gref
        }
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // Optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        if (!name.empty() && count > 1) {
            printf("MeshPointReference: error name att cannot be specified "
                "when count > 1\n   %s\n", name.c_str());
            return false;
        }

        bool result = true;
        if ("" != value && count > 0) {
            std::istringstream is(value.c_str());
            std::string ref;
            for (int icnt = 0; icnt < count; ++icnt) {
                is >> ref;

                ParamVertex *pv1 = NULL;
                // map parametric verts from parent
                if (parentMeshTopo) {
                    pv1 = parentMeshTopo->getParamVertByVref(ref);
                }
                else {
                    pv1 = model->getParamVertByVref(ref);
                }

                result = model->addPoint(ref,
                    mid, aref, gref, name, pv1, mapID);
                if (!result) {
                    printf("MeshPointReference: error storing\n   %s\n",
                        ref.c_str());
                }

                if (meshSheet) {
                    // Add point to sheet
                }
                if (meshString) {
                    // Add point to string
                }

#if DEBUG
                MeshPoint *point = model->getMeshPointByRef(ref);
                ML_assert(nullptr != point);
                if (meshSheet) {
                    //point = meshSheet->getMeshPointByRef(ref);
                    ML_assert(nullptr != point);
                }
                if (meshString) {
                    //point = meshString->getMeshPointByRef(ref);
                    ML_assert(nullptr != point);
                }
#endif
            }
        }
    }
    else {
        // Should be a 'Text Node', i.e., contains at least one point
        std::cout << "MeshPointReference: no ref values." << std::endl;
        return false;
    }

    return true;
}

bool
MeshLinkParserXerces::parseMeshPoint(MeshModel *model,
    xercesc_3_2::DOMElement *meshPointNode)
{
    return false;
}

bool
MeshLinkParserXerces::parseMeshSheetReference(MeshModel *model,
    xercesc_3_2::DOMElement *meshSheetNode)
{
    return parseMeshSheet(model, meshSheetNode);
}

bool
MeshLinkParserXerces::parseMeshSheet(MeshModel *model,
    xercesc_3_2::DOMElement *meshSheetNode)
{
    // Map tag name -> parsing function pointer
    ParseMeshObjMap parseMap;
    parseMap["MeshFace"] = &MeshLinkParserXerces::parseMeshFace;
    parseMap["MeshFaceReference"] = &MeshLinkParserXerces::parseMeshFaceReference;

    // Create new MeshSheet to hold parsed face data
    // Becomes the responsibility of the MeshModel if
    // parsing is successful.
    MeshSheet* meshSheet(new MeshSheet());
    MeshTopo* meshTopo = meshSheet;
    bool mapID = false;

    bool result = parseMeshObject(model, meshTopo, meshSheetNode, "MeshSheet",
        parseMap, mapID);
    if (result) {
        result = model->addMeshSheet(meshSheet);
        if (!result) {
            printf("MeshSheet: error storing\n  %s \n",
                (char*)meshSheetNode->getNodeName());
        }
    }
    if (!result) {
        delete meshSheet;
        return result;
    }

    if (verbose_level_ > 0) {
        printf("MeshSheet %s geometry associations:\n", meshSheet->getName().c_str());
        printf("%8" MLINT_FORMAT " parametric vertices\n", meshSheet->getNumParamVerts());
        printf("%8" MLINT_FORMAT " mesh edges\n", meshSheet->getNumFaceEdges());
        printf("%8" MLINT_FORMAT " mesh faces\n", meshSheet->getNumFaces());
    }

    return true;
}


bool
MeshLinkParserXerces::parseParamVertex(MeshTopo *meshTopo,
    xercesc_3_2::DOMElement *vertNode)
{
    std::vector<std::string> attrs;
    attrs.push_back("mid"); // optional
    attrs.push_back("vref"); // required
    attrs.push_back("gref"); // required
    attrs.push_back("dim"); // required
    std::string value;
    if (parseNode(vertNode, attrs, value)) {
        bool mapID = false;
        int mid = MESH_TOPO_INVALID_REF;
        if (attrs[0] != "") {
            std::istringstream is(attrs[0]);
            int mid;
            is >> mid;
            mapID = true;
        }

        if (attrs[1] == "") {
            std::cout << "ParamVertex: missing vref attribute." <<
                std::endl;
            return false;
        }
        std::string vref = attrs[1];

        int gref;
        if (attrs[2] == "") {
            std::cout << "ParamVertex: missing gref attribute." <<
                std::endl;
            return false;
        }
        else {
            gref = std::stoi(attrs[2]);
        }

        int dim;
        if (attrs[3] == "") {
            std::cout << "ParamVertex: missing dim attribute." <<
                std::endl;
            return false;
        }
        else {
            // check dim
            dim = std::stoi(attrs[3]);
            if (dim < 1 || dim > 2) {
                std::cout << "ParamVertex node: dim is bad " << dim <<
                    std::endl;
                return false;
            }
        }

        // Element content is UV
        MLVector2D uv = {0.0, 0.0};
        if (1 == dim) {
            uv[0] = std::stod(value);
        }
        else {
            std::istringstream points(value);
            double val;
            for (int ii = 0; ii < dim; ++ii) {
                points >> val;
                uv[ii] = val;
            }
        }

        ParamVertex *pv = new ParamVertex(vref,gref,mid,uv[0],uv[1]);
        meshTopo->addParamVertex( pv, mapID);

    }
    else {
        // Should be a 'Text Node', i.e., contains at least one point
        std::cout << "Mesh vertex: no point values." << std::endl;
        return false;
    }
    return true;
}

bool
MeshLinkParserXerces::parseMeshFace(MeshModel *model,
    MeshTopo* parentMeshTopo,
    xercesc_3_2::DOMElement *faceArrayNode)
{
    if (NULL == parentMeshTopo) { return false; }
    MeshSheet* meshSheet =
        dynamic_cast <MeshSheet*> (parentMeshTopo);

    std::vector<std::string> attributeNames;
    // Required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // Optional attributes
    attributeNames.push_back("mid");
    attributeNames.push_back("aref");
    attributeNames.push_back("gref");
    attributeNames.push_back("name");
    attributeNames.push_back("format");
    attributeNames.push_back("count");
    std::vector<std::string> attrs = attributeNames;
    std::string value;
    bool triFace;
    if (parseNode(faceArrayNode, attrs, value)) {
        int n;
        int iattr = 0;
        // First numReqAttrs are required
        for (n = 0; n < numReqAttrs; ++n) {
            if (attrs[n] == "") {
                std::cout << "MeshFace: missing required attribute: " <<
                    attributeNames[n] << std::endl;
                return false;
            }
        }

        std::string etype = attrs[iattr];
        triFace = false;
        if ("Tri3" == etype) {
            triFace = true;
        }
        else if ("Quad4" == etype) {
            triFace = false;
        }
        else {
            std::cout << "MeshFace: illegal etype value: " <<
                etype << std::endl;
            return false;
        }

        // Optional mid attribute
        ++iattr;
        int mid = MESH_TOPO_INVALID_REF;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // Optional aref attribute
        ++iattr;
        int aref = MESH_TOPO_INVALID_REF;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // Optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default is parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // Optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        // Optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format && "base64" != format) {
            std::cout << "MeshFace: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // Optional count attribute
        ++iattr;
        int count = 1;
        if (attrs[iattr] != "") {
            std::istringstream isCount(attrs[iattr]);
            isCount >> count;
            if (count <= 0 || count > 1e12) {
                std::cout << "MeshFace: illegal format value: " <<
                    format << std::endl;
                return false;
            }
        }

        if ("" != value && count > 0) {
            std::istringstream is(value.c_str());
            MLINT i1, i2, i3, i4;
            ParamVertex *pv1, *pv2, *pv3, *pv4;
            for (int icnt = 0; icnt < count; ++icnt) {
                if (triFace) {
                    // 1 group of 3
                    is >> i1 >> i2 >> i3;
                    // Map parametric verts from parent
                    pv1 = parentMeshTopo->getParamVertByVref(std::to_string(i1));
                    pv2 = parentMeshTopo->getParamVertByVref(std::to_string(i2));
                    pv3 = parentMeshTopo->getParamVertByVref(std::to_string(i3));
                    model->addFaceEdge(i1, i2, mid, aref, gref, pv1, pv2);
                    model->addFaceEdge(i2, i3, mid, aref, gref, pv2, pv3);
                    model->addFaceEdge(i3, i1, mid, aref, gref, pv3, pv1);

                    model->addFaceEdgePoint(i1, mid, aref, gref, pv1);
                    model->addFaceEdgePoint(i2, mid, aref, gref, pv2);
                    model->addFaceEdgePoint(i3, mid, aref, gref, pv3);

                    // Record map from mesh face indices to geometry group ID
                    bool result = model->addFace(i1, i2, i3,
                        mid, aref, gref, name, pv1, pv2, pv3, mapID);
                    if (!result) {
                        printf("MeshFace: error storing\n   %s\n",
                            (char*)faceArrayNode->getNodeName());
                    }

                    if (meshSheet) {
                        meshSheet->addFaceEdge(i1, i2, mid, aref, gref, pv1, pv2);
                        meshSheet->addFaceEdge(i2, i3, mid, aref, gref, pv2, pv3);
                        meshSheet->addFaceEdge(i3, i1, mid, aref, gref, pv3, pv1);
                        bool result = meshSheet->addFace(i1, i2, i3,
                            mid, aref, gref, name, pv1, pv2, pv3, mapID);
                        if (!result) {
                            printf("MeshFace: error storing\n   %s\n",
                                (char*)faceArrayNode->getNodeName());
                        }
                    }

#if DEBUG
                    MeshFace *face = model->findFaceByInds(i1, i2, i3);
                    ML_assert(nullptr != face);
                    face = meshSheet->findFaceByInds(i1, i2, i3);
                    ML_assert(nullptr != face);
#endif

                }
                else if (!triFace) {
                    // 1 group of 4
                    is >> i1 >> i2 >> i3 >> i4;

                    // Map parametric verts from parent
                    pv1 = parentMeshTopo->getParamVertByVref(std::to_string(i1));
                    pv2 = parentMeshTopo->getParamVertByVref(std::to_string(i2));
                    pv3 = parentMeshTopo->getParamVertByVref(std::to_string(i3));
                    pv4 = parentMeshTopo->getParamVertByVref(std::to_string(i4));

                    if (model) {
                        model->addFaceEdge(i1, i2, mid, aref, gref, pv1, pv2);
                        model->addFaceEdge(i2, i3, mid, aref, gref, pv2, pv3);
                        model->addFaceEdge(i3, i4, mid, aref, gref, pv3, pv4);
                        model->addFaceEdge(i4, i1, mid, aref, gref, pv4, pv1);

                        model->addFaceEdgePoint(i1, mid, aref, gref, pv1);
                        model->addFaceEdgePoint(i2, mid, aref, gref, pv2);
                        model->addFaceEdgePoint(i3, mid, aref, gref, pv3);
                        model->addFaceEdgePoint(i4, mid, aref, gref, pv4);

                        // record map from mesh face indices to geometry group ID
                        bool result = model->addFace(i1, i2, i3, i4,
                            mid, aref, gref, name, pv1, pv2, pv3, pv4, mapID);
                        if (!result) {
                            printf("MeshFace: error storing\n   %s\n",
                                (char*)faceArrayNode->getNodeName());
                        }
                    }

                    if (meshSheet) {
                        meshSheet->addFaceEdge(i1, i2, mid, aref, gref, pv1, pv2);
                        meshSheet->addFaceEdge(i2, i3, mid, aref, gref, pv2, pv3);
                        meshSheet->addFaceEdge(i3, i4, mid, aref, gref, pv3, pv4);
                        meshSheet->addFaceEdge(i4, i1, mid, aref, gref, pv4, pv1);
                        bool result = meshSheet->addFace(i1, i2, i3, i4,
                            mid, aref, gref, name, pv1, pv2, pv3, pv4, mapID);
                        if (!result) {
                            printf("MeshFace: error storing\n   %s\n",
                                (char*)faceArrayNode->getNodeName());
                        }
                    }

#if DEBUG
                    MeshFace *face = model->findFaceByInds(i1, i2, i3, i4);
                    ML_assert(nullptr != face);
                    face = meshSheet->findFaceByInds(i1, i2, i3, i4);
                    ML_assert(nullptr != face);
#endif

                }
                else {
                    std::cout << "MeshFace: illegal etype value: " <<
                        etype << std::endl;
                    return false;
                }
            }
        }
    }
    else {
        // Should be a 'Text Node', i.e., contains at least one point
        std::cout << "MeshFace: no point values." << std::endl;
        return false;
    }
    return true;
}

bool
MeshLinkParserXerces::parseMeshFaceReference(MeshModel *model,
    MeshTopo* parentMeshTopo,
    xercesc_3_2::DOMElement *faceArrayNode)
{
    if (NULL == parentMeshTopo) { return false; }
    MeshSheet* meshSheet =
        dynamic_cast <MeshSheet*> (parentMeshTopo);

    std::vector<std::string> attributeNames;
    // Required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // Optional attributes
    attributeNames.push_back("mid");
    attributeNames.push_back("format");
    attributeNames.push_back("count");
    attributeNames.push_back("aref");
    attributeNames.push_back("gref");
    attributeNames.push_back("name");
    std::vector<std::string> attrs = attributeNames;
    std::string value;
    bool triFace;
    if (parseNode(faceArrayNode, attrs, value)) {
        int n;
        // First numReqAttrs are required
        for (n = 0; n < numReqAttrs; ++n) {
            if (attrs[n] == "") {
                std::cout << "MeshFaceReference: missing required attribute: " <<
                    attributeNames[n] << std::endl;
                return false;
            }
        }

        // Required etype attribute
        int iattr = 0;
        std::string etype = attrs[iattr];
        triFace = false;
        if ("Tri3" == etype) {
            triFace = true;
        }
        else if ("Quad4" == etype) {
            triFace = false;
        }
        else {
            std::cout << "MeshFaceReference: unknown etype value: " <<
                etype << std::endl;
            return false;
        }

        // Optional mid attribute
        ++iattr;
        int mid = MESH_TOPO_INVALID_REF;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // Optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format && "base64" != format) {
            std::cout << "MeshFaceReference: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // Optional count attribute
        ++iattr;
        int count = 1;
        if (attrs[iattr] != "") {
            std::istringstream isCount(attrs[iattr]);
            isCount >> count;
            if (count <= 0 || count > 1e12) {
                std::cout << "MeshFaceReference: illegal count value: " <<
                    count << std::endl;
                return false;
            }
        }

        // Optional aref attribute
        ++iattr;
        int aref = MESH_TOPO_INVALID_REF;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // Optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // Optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        if (!name.empty() && count > 1) {
            printf("MeshFaceReference: error name att cannot be specified when "
                "count > 1\n   %s\n", name.c_str());
            return false;
        }

        ParamVertex *pv1 = NULL;
        ParamVertex *pv2 = NULL;
        ParamVertex *pv3 = NULL;
        ParamVertex *pv4 = NULL;

        if ("" != value && count > 0) {
            std::istringstream is(value.c_str());
            std::string ref;
            for (int icnt = 0; icnt < count; ++icnt) {
                is >> ref;

                // Record map from mesh face indices to geometry group ID
                bool result = model->addFace(ref,
                    mid, aref, gref, name, pv1, pv2, pv3, mapID);
                if (!result) {
                    printf("MeshFaceReference: error storing\n   %s\n",
                        ref.c_str());
                }

                if (meshSheet) {
                    //meshSheet->addFaceEdge(i1, i2, mid, aref, gref, pv1, pv2);
                    //meshSheet->addFaceEdge(i2, i3, mid, aref, gref, pv2, pv3);
                    //meshSheet->addFaceEdge(i3, i1, mid, aref, gref, pv3, pv1);
                    bool result = meshSheet->addFace(ref,
                        mid, aref, gref, name, pv1, pv2, pv3, mapID);
                    if (!result) {
                        printf("MeshFaceReference: error storing\n   %s\n",
                            ref.c_str());
                    }
                }

#if DEBUG
                MeshFace *face = model->getMeshFaceByRef(ref);
                ML_assert(nullptr != face);
                face = meshSheet->getMeshFaceByRef(ref);
                ML_assert(nullptr != face);
#endif
            }
        }

    }
    else {
        // Should be a 'Text Node', i.e., contains at least one point
        std::cout << "MeshFaceReference: no ref values." << std::endl;
        return false;
    }
    return true;
}


bool
MeshLinkParserXerces::parseMeshEdge(MeshModel *model,
    MeshTopo* parentMeshTopo,
    xercesc_3_2::DOMElement *edgeNode)
{
    if (NULL == parentMeshTopo) { return false; }
    MeshString* meshString =
        dynamic_cast <MeshString*> (parentMeshTopo);

    std::vector<std::string> attributeNames;
    // Required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // Optional attributes
    attributeNames.push_back("mid");
    attributeNames.push_back("format");
    attributeNames.push_back("count");
    attributeNames.push_back("aref");
    attributeNames.push_back("gref");
    attributeNames.push_back("name");
    std::vector<std::string> attrs = attributeNames;
    std::string value;
    if (parseNode(edgeNode, attrs, value)) {
        int n;
        int iattr = 0;
        // First numReqAttrs are required
        for (n = 0; n < numReqAttrs; ++n) {
            if (attrs[n] == "") {
                std::cout << "Mesh Edge: missing required attribute: " <<
                    attributeNames[n] << std::endl;
                return false;
            }
        }

        std::string etype = attrs[iattr];
        if ("Edge2" != etype) {
            std::cout << "Mesh Edge: illegal etype value: " <<
                etype << std::endl;
            return false;
        }

        // Optional mid attribute
        ++iattr;
        int mid = MESH_TOPO_INVALID_REF;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // Optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format) {
            std::cout << "MeshEdge: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // Optional count attribute
        ++iattr;
        int count = 1;
        if (attrs[iattr] != "") {
            std::istringstream isCount(attrs[iattr]);
            isCount >> count;
            if (count <= 0 || count > 1e12) {
                std::cout << "MeshEdge: illegal count value: " <<
                    count << std::endl;
                return false;
            }
        }

        // Optional aref attribute
        ++iattr;
        int aref = MESH_TOPO_INVALID_REF;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // Optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // Optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        if ("" != value && count > 0) {
            std::istringstream is(value.c_str());
            MLINT i1, i2;
            for (int icnt = 0; icnt < count; ++icnt) {
                // 1 group of 2
                is >> i1 >> i2;

                // Map parametric verts from parent
                ParamVertex *pv1,*pv2;
                pv1 = parentMeshTopo->getParamVertByVref(std::to_string(i1));
                pv2 = parentMeshTopo->getParamVertByVref(std::to_string(i2));

                model->addEdgePoint(i1, mid, aref, gref, pv1);
                model->addEdgePoint(i2, mid, aref, gref, pv2);

                bool result = model->addEdge(i1, i2,
                    mid,
                    aref,
                    gref,
                    name,
                    pv1,pv2, mapID);
                if (!result) {
                    printf("MeshEdge: error storing\n   %s\n",
                        (char*)edgeNode->getNodeName());
                }

                if (meshString) {
                    bool results = meshString->addEdge(i1, i2,
                        mid,
                        aref,
                        gref,
                        name,
                        pv1, pv2, mapID);
                    if (!result) {
                        printf("MeshEdge: error storing\n   %s\n",
                            (char*)edgeNode->getNodeName());
                    }
                }
#if DEBUG
                MeshEdge *edge = model->findEdgeByInds(i1, i2);
                ML_assert(nullptr != edge);
                edge = meshString->findEdgeByInds(i1, i2);
                ML_assert(nullptr != edge);
#endif

            }
        }
    }
    else {
        // Should be a 'Text Node', i.e., contains at least one point
        std::cout << "MeshEdge: no point values." << std::endl;
        return false;
    }
    return true;
}

bool
MeshLinkParserXerces::parseMeshEdgeReference(MeshModel *model,
    MeshTopo* parentMeshTopo,
    xercesc_3_2::DOMElement *edgeNode)
{
    if (NULL == parentMeshTopo) { return false; }
    MeshString* meshString =
        dynamic_cast <MeshString*> (parentMeshTopo);

    std::vector<std::string> attributeNames;
    // Required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // Optional attributes
    attributeNames.push_back("mid");
    attributeNames.push_back("format");
    attributeNames.push_back("count");
    attributeNames.push_back("aref");
    attributeNames.push_back("gref");
    attributeNames.push_back("name");
    std::vector<std::string> attrs = attributeNames;
    std::string value;
    if (parseNode(edgeNode, attrs, value)) {
        int n;
        int iattr = 0;
        // First numReqAttrs are required
        for (n = 0; n < numReqAttrs; ++n) {
            if (attrs[n] == "") {
                std::cout << "MeshEdgeReference: missing required attribute: " <<
                    attributeNames[n] << std::endl;
                return false;
            }
        }

        // Required etype attribute
        std::string etype = attrs[iattr];
        if ("Edge2" != etype) {
            std::cout << "MeshEdgeReference: illegal etype value: " <<
                etype << std::endl;
            return false;
        }

        // Optional mid attribute
        ++iattr;
        int mid = MESH_TOPO_INVALID_REF;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // Optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format) {
            std::cout << "MeshEdgeReference: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // Optional count attribute
        ++iattr;
        int count = 1;
        if (attrs[iattr] != "") {
            std::istringstream isCount(attrs[iattr]);
            isCount >> count;
            if (count <= 0 || count > 1e12) {
                std::cout << "MeshEdgeReference: illegal count value: " <<
                    count << std::endl;
                return false;
            }
        }

        // Optional aref attribute
        ++iattr;
        int aref = MESH_TOPO_INVALID_REF;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // Optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // Optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        ParamVertex *pv1 = NULL;
        ParamVertex *pv2 = NULL;

        if (!name.empty() && count > 1) {
            printf("MeshEdgeReference: error name att cannot be specified when "
                "count > 1\n   %s\n",
                name.c_str());
            return false;
        }

        if ("" != value && count > 0) {
            std::istringstream is(value.c_str());
            std::string ref;
            for (int icnt = 0; icnt < count; ++icnt) {
                is >> ref;

                bool result = model->addEdge(
                    ref,
                    mid,
                    aref,
                    gref,
                    name,
                    pv1, pv2, mapID);
                if (!result) {
                    printf("MeshEdgeReference: error storing\n   %s\n",
                        ref.c_str());
                }

                if (meshString) {
                    bool results = meshString->addEdge(
                        ref,
                        mid,
                        aref,
                        gref,
                        name,
                        pv1, pv2, mapID);
                    if (!result) {
                        printf("MeshEdgeReference: error storing\n   %s\n",
                            ref.c_str());
                    }
                }
#if DEBUG
                MeshEdge *edge = model->getMeshEdgeByRef(ref);
                ML_assert(nullptr != edge);
                edge = meshString->getMeshEdgeByRef(ref);
                ML_assert(nullptr != edge);
#endif
            }
        }
    }
    else {
        // Should be a 'Text Node', i.e., contains at least one point
        std::cout << "MeshEdgeReference: no ref values." << std::endl;
        return false;
    }
    return true;
}



// Read MeshLink file
bool
MeshLinkParserXerces::parseMeshLinkFile(
    std::string fname,
    MeshAssociativity *meshAssociativity)
{
    if (!meshAssociativity) { return false; }
    meshAssociativity_ = meshAssociativity;
    bool result = true;
    try {
        XMLPlatformUtils::Initialize();
        std::cout << "Successfully initialized xerces API." << std::endl;
    }
    catch (const XMLException &) {
        std::cout << "Failed to initialize xerces API." << std::endl;
        return false;
    }

    XercesDOMParser *parser = new XercesDOMParser();
    SAXErrorHandler* errHandler(new SAXErrorHandler());
    parser->setErrorHandler(errHandler);

    try {
        parser->setValidationSchemaFullChecking(false);
        parser->setDoNamespaces(true);
        parser->parse(fname.c_str());
        errHandler->outputCounts();
    }
    catch (const XMLException &toCatch) {
        char *msg = XMLString::transcode(toCatch.getMessage());
        std::cout << "Exception message: " << msg << std::endl;
        XMLString::release(&msg);
        result = false;
    }
    catch (const DOMException &toCatch) {
        char *msg = XMLString::transcode(toCatch.getMessage());
        std::cout << "DOM Exception message: " << msg << std::endl;
        XMLString::release(&msg);
        result = false;
    }

    if (result) {
        DOMDocument *doc;
        DOMElement *meshLinkRoot;
        if (nullptr == (doc = parser->getDocument())) {
            std::cout << "Mesh Link document was not created from XML file." <<
                std::endl;
            XMLPlatformUtils::Terminate();
            result = false;
            return result;
        }

        if (nullptr == (meshLinkRoot = doc->getDocumentElement())) {
            std::cout << "Mesh Link root element not found." << std::endl;
            XMLPlatformUtils::Terminate();
            result = false;
            return result;
        }

        // Attribute and AttributeGroup elements
        parseAttributes(meshLinkRoot);

        // GeometryFile, GeometryReference and GeometryGroup elements
        parseGeometryRefs(meshLinkRoot);

        // MeshFile elements

        // Parse MeshFiles - multiple ModelReferences each containing
        // multiple MeshSheets, each containing multiple ParamVertices
        // (points) and one MeshFaceArray
        result = true;

        XMLSize_t count = 0;
        {
            DOMNodeTagNameFilter attFilter("MeshFile");
            DOMDocument *doc = meshLinkRoot->getOwnerDocument();
            DOMNode *node;
            DOMNodeIterator *iter = doc->createNodeIterator(meshLinkRoot, DOMNodeFilter::SHOW_ELEMENT,
                &attFilter, false);
            DOMElement *meshFile;
            while ((node = iter->nextNode()) != nullptr) {
                ++count;
                if (nullptr != node &&
                    DOMNode::ELEMENT_NODE == node->getNodeType()) {
                    meshFile = dynamic_cast<DOMElement*>(node);
                    result = parseMeshFile(meshFile);
                }
            }
            iter->release();
        }

        if (!result || 0 == count) {
            std::cout << "Mesh Link File element(s) not found." << std::endl;
            XMLPlatformUtils::Terminate();
            return result;
        }


        // Transform and MeshElementLinkage elements
        parsePeriodicInfo(meshLinkRoot);


        // Cached items for later use if/when writing out Xml file based
        // on parsed/modified Meshassociativity.
        XMLCopier<XMLCh, char> uri = doc->getDocumentURI();
        DOMNode *node = doc->getFirstChild();
        DOMNamedNodeMap *atts = node->getAttributes();
        if (nullptr != atts) {
            XMLSize_t size = atts->getLength();
            if (size > 3) {
                // Version
                DOMNode *attNode = atts->getNamedItem(X("version"));
                xmlVersion_ = Char(attNode->getNodeValue());
                // Xml Namespace
                attNode = atts->getNamedItem(X("xmlns"));
                xmlns_ = Char(attNode->getNodeValue());
                // Xml Namespace schema instance
                attNode = atts->getNamedItem(X("xmlns:xsi"));
                xmlns_xsi_ = Char(attNode->getNodeValue());
                // Xml schema location
                attNode = atts->getNamedItem(X("xsi:schemaLocation"));
                schemaLocation_ = Char(attNode->getNodeValue());
            }
        }
    }

    delete parser;
    delete errHandler;

    try {
        XMLPlatformUtils::Terminate();
        std::cout << "Successfully terminated xerces API." << std::endl;
    }
    catch (const XMLException &) {
        std::cout << "Failed to terminate xerces API." << std::endl;
        return false;
    }

    return result;
}


MeshLinkWriterXerces *
MeshLinkParserXerces::getXMLWriter()
{
    if (!xmlns_.empty() && !xmlns_xsi_.empty() && !schemaLocation_.empty()) {
        return new MeshLinkWriterXerces(xmlns_, xmlns_xsi_, schemaLocation_);
    }
    return nullptr;
}

/****************************************************************************
 *
 * This file is licensed under the Cadence Public License Version 1.0 (the
 * "License"), a copy of which is found in the included file named "LICENSE",
 * and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
 * LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
 * ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
 * Please see the License for the full text of applicable terms.
 *
 ****************************************************************************/
