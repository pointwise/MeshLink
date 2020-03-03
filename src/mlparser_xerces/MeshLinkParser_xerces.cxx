
#include "MeshAssociativity.h"


#include "MeshLinkParser_xerces.h"

#include <iostream>
#include <sstream>
#include <istream>
#include <iterator>
#include <sstream>

#define SSTR( x ) static_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/XMLString.hpp>

using namespace xercesc;



// Replaces xerces XMLString::transcode() method to take care of
// releasing the transcoded result automatically
template <typename T, typename U>
class XMLCopier {
public:
    XMLCopier(const T *input) :
        input_(input),
        val_(0)
    {}

    ~XMLCopier() {
        XMLString::release(&val_);
    }

    operator U *() {
        if (nullptr == val_) {
            val_ = XMLString::transcode(input_);
        }
        return val_;
    }

    bool empty() {
        if (nullptr != val_) {
            return XMLString::stringLen(val_) > 0;
        }
        return false;
    }

private:
    const T *input_;
    U *val_;
};


// Returns true if Element node is a Text Node
static bool
parseNode(DOMElement *node, std::vector<std::string> &atts, std::string &value)
{
    bool result = false;
    // Attributes vector contains names, to be replaced with parsed values
    for (size_t i = 0; i < atts.size(); ++i) {
        XMLCopier<char, XMLCh> name(atts.at(i).c_str());
        XMLCopier<XMLCh, char> attr(node->getAttribute(name));
        if (!attr.empty()) {
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
            value  = contents;
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
    XMLCopier<char, XMLCh> attName("Attribute");
    XMLCopier<char, XMLCh> attGroupName("AttributeGroup");
    XMLCopier<char, XMLCh> formatName("#text");
    DOMNode *attr;

    DOMNodeList *attNodes;
    if (nullptr == (attNodes = root->getElementsByTagName(attName))) {
        return true;
    }

    for (XMLSize_t i = 0; i < attNodes->getLength(); ++i) {
        DOMNode *attNode = attNodes->item(i);
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
            const MeshLinkAttribute *existing = meshAssociativity_->getAttributeByID(attid);
            if (existing) {
                std::cout << "Attribute reuses existing attid attribute \"" << attid << "\"" << std::endl;
                continue;
            }

            // optional atts
            attr = attMap->getNamedItem(nameName);
            if (nullptr != attr) {
                XMLCopier<XMLCh, char> xmlStr(attr->getNodeValue());
                name = (char *) xmlStr;
            }

            // contents
            DOMNode *child = attNode->getFirstChild();
            while (nullptr != child) {
                if (DOMNode::TEXT_NODE == child->getNodeType()) {
                    XMLCopier<XMLCh, char> xmlStr(child->getNodeValue());
                    contents = (char *) xmlStr;
                    break;
                }
                child = child->getNextSibling();
            }

            if (contents.empty()) {
                std::cout << "Attribute missing content" << std::endl;
                continue;
            }
            else {
                MeshLinkAttribute mlAtt((MLINT)attid, name, contents, false, *meshAssociativity_);
                if (mlAtt.isValid()) {
                    meshAssociativity_->addAttribute(mlAtt);
                }
            }
        }
    }


    // AttributeGroup elements
    if (nullptr == (attNodes = root->getElementsByTagName(attGroupName))) {
        return true;
    }

    for (XMLSize_t i = 0; i < attNodes->getLength(); ++i) {
        DOMNode *attNode = attNodes->item(i);
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
                std::cout << "AttributeGroup missing attid attribute" << std::endl;
                continue;
            }
            attid = XMLString::parseInt(attr->getNodeValue());
            const MeshLinkAttribute *existing = meshAssociativity_->getAttributeByID(attid);
            if (existing) {
                std::cout << "AttributeGroup reuses existing attid attribute \"" << attid << "\"" << std::endl;
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
                MeshLinkAttribute mlAtt((MLINT)attid, name, contents, true, *meshAssociativity_);
                if (mlAtt.isValid()) {
                    meshAssociativity_->addAttribute(mlAtt);
                }
            }
        }
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
        // Use explicitly defined schema file, not the schemaLocation specified in meshlink file
        try {
            XMLCopier<char, XMLCh> sc(schemaName.c_str());
            parser->useCachedGrammarInParse(true);
            Grammar *schema = parser->loadGrammar(sc, Grammar::SchemaGrammarType, true);
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
    XMLCopier<char, XMLCh> gidName("gid");
    XMLCopier<char, XMLCh> refName("ref");
    XMLCopier<char, XMLCh> arefName("aref");
    XMLCopier<char, XMLCh> nameName("name");

    DOMNamedNodeMap *attMap = geometryReferenceDOM->getAttributes();
    if (nullptr == attMap) {
        std::cout << "GeometryReference node not an element" << std::endl;
        return false;
    }
    // geometry id - 'gid' attribute
    DOMNode *gidAttr = attMap->getNamedItem(gidName);
    if (nullptr == gidAttr) {
        std::cout << "GeometryReference missing gid attribute" << std::endl;
        return false;
    }
    const XMLCh *gid = gidAttr->getNodeValue();
    group.setID(XMLString::parseInt(gid));

    // entity name - only one per DOMNode - 'ref' attribute
    DOMNode *refAttr = attMap->getNamedItem(refName);
    if (nullptr == refAttr) {
        std::cout << "GeometryReference missing ref attribute" << std::endl;
        return false;
    }
    XMLCopier<XMLCh, char> ref(refAttr->getNodeValue());
    group.addEntityName((char *)ref);

    // always need a name - default to 'ref' value
    group.setName((char *)ref);

    // optional 'name' attribute
    DOMNode *nameAttr = attMap->getNamedItem(nameName);
    if (nullptr != nameAttr) {
        XMLCopier<XMLCh, char> xmlStr(nameAttr->getNodeValue());
        group.setName((char *)xmlStr);
    }
    
    // optional aref attribute
    DOMNode *arefAttr = attMap->getNamedItem(arefName);
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
    XMLCopier<char, XMLCh> gidName("gid");
    XMLCopier<char, XMLCh> arefName("aref");
    XMLCopier<char, XMLCh> nameName("name");

    DOMNamedNodeMap *attMap = geometryGroupDOM->getAttributes();
    if (nullptr == attMap) {
        std::cout << "GeometryGroup node not an element" << std::endl;
        return false;
    }
    // geometry group id - 'gid' attribute
    DOMNode *gidAttr = attMap->getNamedItem(gidName);
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
        std::cout << "GeometryGroup with gid=\"" << gid << "\" missing content" << std::endl;
        return false;
    }

    // construct a stream from the string
    std::stringstream strstr(grefstr);

    // use stream iterators to copy the stream to the vector as whitespace separated strings
    std::istream_iterator<std::string> it(strstr);
    std::istream_iterator<std::string> end;
    std::vector<std::string> grefs(it, end);

    size_t i;
    for (i = 0; i < grefs.size(); ++i) {
        std::string &gref = grefs[i];
        int grefID = -1;
        try {
            grefID = std::stoi(gref);
        }
        catch (...) {
            std::cout << "GeometryGroup invalid content \"" << gref << "\"" << std::endl;
            return false;
        }
        GeometryGroup *refgroup = meshAssociativity->getGeometryGroupByID(grefID);
        if (refgroup) {
            const std::vector<std::string> &ents = refgroup->getEntityNames();
            std::vector<std::string>::const_iterator iter;
            for (iter = ents.cbegin(); iter != ents.cend(); ++iter) {
                group.addEntityName((*iter).c_str());
            }
        }
        else {
            std::cout << "GeometryGroup unknown gid in content \"" << grefID << "\"" << std::endl;
            return false;
        }
    }

    // always need a name - default to "geom_group_<gid>"
    std::ostringstream s;  s << gid;
    std::string name = std::string("geom_group_") + s.str();
    group.setName( name.c_str() );
    group.setID(gid);

    // optional 'name' attribute
    DOMNode *nameAttr = attMap->getNamedItem(nameName);
    if (nullptr != nameAttr) {
        XMLCopier<XMLCh, char> xmlStr(nameAttr->getNodeValue());
        group.setName((char *)xmlStr);
    }

    // optional aref attribute
    DOMNode *arefAttr = attMap->getNamedItem(arefName);
    if (nullptr != arefAttr) {
        const XMLCh *valStr = arefAttr->getNodeValue();
        group.setAref(XMLString::parseInt(valStr));
    }

    return true;
}


// Parse GeometryFile element and child GeometryReference and GeometryGroup elements 
bool
MeshLinkParserXerces::parseGeometryRefs(DOMElement *root)
{
    if (!meshAssociativity_) { return false; }
    bool result = true;
    XMLCh *geoFileName = XMLString::transcode("GeometryFile");
    XMLCh *geoRefName = XMLString::transcode("GeometryReference");
    XMLCh *geoGroupName = XMLString::transcode("GeometryGroup");

    // Loop through GeometryFile nodes parsing GeometryReference elements
    DOMNodeList *gfNode = root->getElementsByTagName(geoFileName);
    for (XMLSize_t i = 0; i < gfNode->getLength() && result; ++i) {
        DOMElement *elem = dynamic_cast<DOMElement *>(gfNode->item(i));
        if ( nullptr != elem ) {

            XMLCopier<char, XMLCh> filenameName("filename");
            XMLCopier<char, XMLCh> arefName("aref");

            std::string filename, aref;

            if (elem->hasAttribute(filenameName)) {
                XMLCopier<XMLCh, char>  xmlStr(elem->getAttribute(filenameName));
                filename = (char *) xmlStr;
            }
            else {
                std::cout << "GeometryFile node: no filename attribute." << std::endl;
                return false;
            }
            if (elem->hasAttribute(arefName)) {
                XMLCopier<XMLCh, char>  xmlStr(elem->getAttribute(arefName));
                aref = (char *)xmlStr;
            }

            GeometryFile geomFile(filename, aref);
            meshAssociativity_->addGeometryFile(geomFile);

        }

        DOMNodeList *geomRefs = root->getElementsByTagName(geoRefName);

        for (XMLSize_t i = 0; i < geomRefs->getLength() && result; ++i) {
            DOMNode *geomRef = geomRefs->item(i);
            GeometryGroup geom_group;
            if ((result = parseGeomRefDOM(geomRef, geom_group))) {
                meshAssociativity_->addGeometryGroup(geom_group);
            }
        }
    }

    // Loop through GeometryFile nodes parsing GeometryGroup elements
    // GeometryGroup content is list of attids which must match
    // those defined by GeometryReference elements above.
    for (XMLSize_t i = 0; i < gfNode->getLength() && result; ++i) {

        DOMNodeList *geomGroups = root->getElementsByTagName(geoGroupName);

        for (XMLSize_t i = 0; i < geomGroups->getLength() && result; ++i) {
            DOMNode *geomGroup = geomGroups->item(i);
            GeometryGroup geom_group;
            if ((result = parseGeomGroupDOM(geomGroup, meshAssociativity_, geom_group))) {
                meshAssociativity_->addGeometryGroup(geom_group);
            }
        }
    }

    XMLString::release(&geoFileName);
    XMLString::release(&geoRefName);
    XMLString::release(&geoGroupName);
    return result;
}


// Parse single MeshFile element and children 
bool
MeshLinkParserXerces::parseMeshFile(xercesc_3_2::DOMElement *meshFile)
{
    if (!meshAssociativity_) { return false; }
    // Attributes
    XMLCopier<char, XMLCh> filenameName("filename");
    XMLCopier<char, XMLCh> arefName("aref");

    std::string filename, aref;

    if (meshFile->hasAttribute(filenameName)) {
        XMLCopier<XMLCh, char>  xmlStr(meshFile->getAttribute(filenameName));
        filename = (char *)xmlStr;
    }
    else {
        std::cout << "MeshFile node: no filename attribute." << std::endl;
        return false;
    }
    if (meshFile->hasAttribute(arefName)) {
        XMLCopier<XMLCh, char>  xmlStr(meshFile->getAttribute(arefName));
        aref = (char *)xmlStr;
    }

    MeshFile mFile(filename, aref);
    meshAssociativity_->addMeshFile(mFile);

    // Model References
    XMLCopier<char, XMLCh> modelRefTag("MeshModelReference");
    DOMNodeList *nodes;
    if (nullptr == (nodes = meshFile->getElementsByTagName(modelRefTag)) ||
        0 == nodes->getLength()) {
        std::cout << "MeshFile node: no model references." << std::endl;
        return false;
    }

    bool result = true;
    for (XMLSize_t i = 0; i < nodes->getLength() && result; ++i) {
        DOMElement *modelRef;
        DOMNode *node = nodes->item(i);
        if (DOMNode::ELEMENT_NODE == node->getNodeType()) {
            modelRef = dynamic_cast<DOMElement*>(node);
            result = parseMeshRefModel(modelRef);
        }
    }

    if (!result) {
        std::cout << "MeshFile node: problem parsing mesh reference." <<
            std::endl;
        return false;
    }

    return true;
}


#define ParseMeshContainer(MeshModel, MeshObj)                          \
    {                                                                   \
        XMLCopier<char, XMLCh> objTag(#MeshObj);                        \
        DOMNodeList *nodes = MeshModel->getElementsByTagName(objTag);   \
        for (XMLSize_t i = 0; i < nodes->getLength() && result; ++i) {  \
            DOMElement *meshObj;                                        \
            DOMNode *node = nodes->item(i);                             \
            if (DOMNode::ELEMENT_NODE == node->getNodeType()) {         \
                meshObj = dynamic_cast<DOMElement*>(node);              \
                result = parse##MeshObj(meshModel, meshObj);      \
            }                                                           \
            if (!result) {                                              \
                std::cout << "MeshModelReference node: problem "        \
                    "parsing " << #MeshObj << " " << i << std::endl;    \
                return false;                                           \
            }                                                           \
        }                                                               \
    }

#define ParseMeshContainerNoRecurse(MeshModel, MeshObj)                          \
    {                                                                   \
        XMLCopier<char, XMLCh> objTag(#MeshObj);                        \
        DOMNodeList *nodes = MeshModel->getElementsByTagName(objTag);   \
        for (XMLSize_t i = 0; i < nodes->getLength() && result; ++i) {  \
            DOMElement *meshObj;                                        \
            DOMNode *node = nodes->item(i);                             \
            DOMNode *parentNode = node->getParentNode();                \
            if (parentNode && DOMNode::ELEMENT_NODE == parentNode->getNodeType()) {         \
                DOMElement *parentElem = dynamic_cast<DOMElement*>(parentNode);   \
                if (parentElem != MeshModel) continue;                  \
            }                                                           \
            if (DOMNode::ELEMENT_NODE == node->getNodeType()) {         \
                meshObj = dynamic_cast<DOMElement*>(node);              \
                result = parse##MeshObj(meshModel, meshObj);            \
            }                                                           \
            if (!result) {                                              \
                std::cout << "MeshModelReference node: problem "        \
                    "parsing " << #MeshObj << " " << i << std::endl;    \
                return false;                                           \
            }                                                           \
        }                                                               \
    }


bool
MeshLinkParserXerces::parseMeshRefModel(xercesc_3_2::DOMElement *modelRef)
{
    if (!meshAssociativity_) { return false; }

    // MeshModelReference attributes
    XMLCopier<char, XMLCh> refName("ref"); // required
    XMLCopier<char, XMLCh> midName("mid");
    XMLCopier<char, XMLCh> arefName("aref");
    XMLCopier<char, XMLCh> grefName("gref");
    XMLCopier<char, XMLCh> nameName("name");

    bool mapID = false;
    int mid = -1;
    std::string name;
    int aref = -1;
    int gref = -1;
    std::string ref;
    if (modelRef->hasAttribute(refName)) {
        XMLCopier<XMLCh, char>  xmlStr(modelRef->getAttribute(refName));
        ref = std::string(xmlStr);
    }
    else {
        // bad model store
        std::cout << "MeshModelReference: missing ref attribute." << std::endl;
        return false;
    }

    if (modelRef->hasAttribute(midName)) {
        mid = XMLString::parseInt(modelRef->getAttribute(midName));

        MeshModel *existingModel = meshAssociativity_->getMeshModelByID(mid);
        if (existingModel) {
            // Can't have multiple mid values
            std::cout << "MeshModelReference: mid identifier already in "
                "use." << std::endl;
            return false;
        }
        mapID = true;
    }
    if (modelRef->hasAttribute(arefName)) {
        aref = XMLString::parseInt(modelRef->getAttribute(arefName));
    }
    if (modelRef->hasAttribute(grefName)) {
        gref = XMLString::parseInt(modelRef->getAttribute(grefName));
    }
    if (modelRef->hasAttribute(nameName)) {
        XMLCopier<XMLCh, char>  xmlStr(modelRef->getAttribute(nameName));
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

    MeshModel* meshModel = new MeshModel(ref, mid,aref,gref,name);

    bool result = meshAssociativity_->addMeshModel(meshModel, mapID);

    if (!result) {
        // bad model store
        std::cout << "MeshModelReference: error storing model." << std::endl;
        delete meshModel;
        return false;
    }
    else {
        // Parse MeshSheets
        ParseMeshContainer(modelRef, MeshSheet);
        ParseMeshContainer(modelRef, MeshSheetReference);

        // Parse MeshStrings
        ParseMeshContainer(modelRef, MeshString);
        ParseMeshContainer(modelRef, MeshStringReference);

        // Parse MeshPoints

        // Param vertices (do prior to MeshPointRef)
        XMLCopier<char, XMLCh> vertexName("ParamVertex");
        DOMNodeList *verts;
        if (nullptr != (verts = modelRef->getElementsByTagName(vertexName)) &&
            0 != verts->getLength()) {

            for (XMLSize_t i = 0; i < verts->getLength() && result; ++i) {
                DOMNode *node;
                if (nullptr == (node = verts->item(i))) {
                    continue;
                }

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

            if (!result) {
                std::cout << "MeshModel: error parsing vertex." << std::endl;
                return false;
            }
        }

        ParseMeshContainerNoRecurse(modelRef, MeshPointReference);


        if (result) {
            XMLCopier<char, XMLCh> modelRefNameAtt("name");
            DOMAttr *attNode = modelRef->getAttributeNode(modelRefNameAtt);
            if (attNode) {
                const XMLCh *val = attNode->getValue();
                char* meshName = XMLString::transcode(attNode->getValue());
                printf("MeshModel %s geometry associations:\n", meshName);
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

//void
//MeshLinkParserXerces::clearMeshObjs()
//{
//    MeshObjs::iterator moIter;
//    for (moIter = meshObjs_.begin(); moIter != meshObjs_.end(); ++moIter) {
//        delete moIter->second;
//    }
//    meshObjs_.clear();
//}
//
//MeshObjPtr 
//MeshLinkParserXerces::findMeshObj(MLINT id)
//{
//    MeshObjs::iterator it = meshObjs_.find(id);
//    if (it == meshObjs_.end()) {
//        return NULL;
//    }
//    else {
//        return it->second;
//    }
//}
//
//void            
//MeshLinkParserXerces::addMeshObj(MeshObjPtr meshObj)
//{
//    meshObjs_[meshObj->mid_] = meshObj;
//}


bool
MeshLinkParserXerces::parseMeshObject(MeshModel *model,
    MeshTopo* meshTopo,
    xercesc_3_2::DOMElement *xmlObj,
    const char *objName, 
    ParseMeshObjMap &parseMeshObjMap, bool &mapID)
{
    // Attributes
    XMLCopier<char, XMLCh> midName("mid");
    XMLCopier<char, XMLCh> nameName("name");
    XMLCopier<char, XMLCh> refName("ref");
    XMLCopier<char, XMLCh> grefName("gref");
    XMLCopier<char, XMLCh> arefName("aref");

    if (NULL == meshTopo) { return false; }
    MeshString *meshString = dynamic_cast<MeshString *> (meshTopo);
    MeshSheet *meshSheet = dynamic_cast<MeshSheet *> (meshTopo);

    mapID = false;
    if (xmlObj->hasAttribute(midName)) {
        meshTopo->setID( XMLString::parseInt(xmlObj->getAttribute(midName)) );

        if (meshString) {
            MeshString *existingString = model->getMeshStringByID(meshString->getID());
            if (existingString) {
                // Can't have multiple mid values
                std::cout << "MeshString: mid identifier already in use." << std::endl;
                return false;
            }
            mapID = true;
        }
        if (meshSheet) {
            MeshSheet *existingSheet = model->getMeshSheetByID(meshSheet->getID());
            if (existingSheet) {
                // Can't have multiple mid values
                std::cout << "MeshSheet: mid identifier already in use." << std::endl;
                return false;
            }
            mapID = true;
        }
    }
    if (xmlObj->hasAttribute(nameName)) {
        XMLCopier<XMLCh, char>  name(xmlObj->getAttribute(nameName));
        meshTopo->setName( name );

        if (meshString) {
            MeshString *existingString = model->getMeshStringByName(meshString->getName());
            if (existingString) {
                // Can't have multiple name values
                std::cout << "MeshString: name identifier already in use." << std::endl;
                return false;
            }
        }
        if (meshSheet) {
            MeshSheet *existingSheet = model->getMeshSheetByName(meshSheet->getName());
            if (existingSheet) {
                // Can't have multiple name values
                std::cout << "MeshSheet: name identifier already in use." << std::endl;
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
    if (xmlObj->hasAttribute(grefName)) {
        meshTopo->setGref( XMLString::parseInt(xmlObj->getAttribute(grefName)) );
    }
    if (xmlObj->hasAttribute(arefName)) {
        meshTopo->setAref( XMLString::parseInt(xmlObj->getAttribute(arefName)) );
    }


    if (xmlObj->hasAttribute(refName)) {
        XMLCopier<XMLCh, char>  ref(xmlObj->getAttribute(refName));
        meshTopo->setRef(ref);

        if (meshString) {
            MeshString *existingString = model->getMeshStringByRef(meshString->getRef());
            if (existingString) {
                // Can't have multiple narefme values
                std::cout << "MeshStringReference: ref identifier already in use." << std::endl;
                return false;
            }
        }
        if (meshSheet) {
            MeshSheet *existingSheet = model->getMeshSheetByRef(meshSheet->getRef());
            if (existingSheet) {
                // Can't have multiple ref values
                std::cout << "MeshSheetReference: ref identifier already in use." << std::endl;
                return false;
            }
        }
    }

    bool result = true;

    // Param vertices
    XMLCopier<char, XMLCh> vertexName("ParamVertex");
    DOMNodeList *verts;
    if (nullptr != (verts = xmlObj->getElementsByTagName(vertexName)) &&
            0 != verts->getLength()) {
        
        for (XMLSize_t i = 0; i < verts->getLength() && result; ++i) {
            DOMNode *node;
            if (nullptr == (node = verts->item(i))) {
                continue;
            }
            DOMElement *vertNode = dynamic_cast<DOMElement*>(node);
            result = parseParamVertex(meshTopo, vertNode);
        }

        if (!result) {
            std::cout << objName << ": error parsing vertex." << std::endl;
            return false;
        }
    }

    // Child mesh objects
    ParseMeshObjMap::iterator meshObjIter;
    XMLSize_t totalItems = 0;
    for (meshObjIter = parseMeshObjMap.begin(); meshObjIter != parseMeshObjMap.end(); ++meshObjIter) {
        const char *meshObjName = meshObjIter->first;
        pParseMeshObj parseMeshObj = meshObjIter->second;
        XMLCopier<char, XMLCh> childMeshObj(meshObjName);
        DOMNodeList *items;
        if (nullptr == (items = xmlObj->getElementsByTagName(childMeshObj)) ||
            0 == items->getLength()) {
            // element not found
            continue;
        }
        totalItems += items->getLength();
        for (XMLSize_t i = 0; i < items->getLength() && result; ++i) {
            DOMNode *node;
            if (nullptr == (node = items->item(i))) {
                continue;
            }
            DOMElement *itemNode = dynamic_cast<DOMElement*>(node);
            result = (this->*parseMeshObj)(model, meshTopo, itemNode);
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

    printf("MeshString %s geometry associations:\n", meshString->getName().c_str());
    printf("%8" MLINT_FORMAT " parametric vertices\n", meshString->getNumParamVerts());
    printf("%8" MLINT_FORMAT " mesh edges\n", meshString->getNumEdges());

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

    // optional attributes
    attributeNames.push_back("mid");
    attributeNames.push_back("format");
    attributeNames.push_back("count");
    attributeNames.push_back("aref");
    attributeNames.push_back("gref");
    attributeNames.push_back("name");
    std::vector<std::string> attrs = attributeNames;
    std::string value;
    if (parseNode(meshPointNode, attrs, value)) {

        // optional mid attribute
        int iattr = 0;
        int mid = -1;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // optional format attribute
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

        // optional count attribute
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

        // optional aref attribute
        ++iattr;
        int aref = -1;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // optional gref attribute
        ++iattr;
        int gref = -1;
        if (parentMeshTopo) {
            gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        }
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        if (!name.empty() && count > 1) {
            printf("MeshPointReference: error name att cannot be specified when count > 1\n   %s\n",
                name.c_str());
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
                    // add point to sheet
                }
                if (meshString) {
                    // add point to string
                }

#if DEBUG
                MeshPoint *point = model->getMeshPointByRef(ref);
                pw_assert(point);
                if (meshSheet) {
                    //point = meshSheet->getMeshPointByRef(ref);
                    pw_assert(point);
                }
                if (meshString) {
                    //point = meshString->getMeshPointByRef(ref);
                    pw_assert(point);
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
    // map tag name -> parsing function pointer
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

    printf("MeshSheet %s geometry associations:\n", meshSheet->getName().c_str());
    printf("%8" MLINT_FORMAT " parametric vertices\n", meshSheet->getNumParamVerts());
    printf("%8" MLINT_FORMAT " mesh edges\n", meshSheet->getNumFaceEdges());
    printf("%8" MLINT_FORMAT " mesh faces\n", meshSheet->getNumFaces());

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
        int mid = -1;
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

        // element content is UV
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
        meshTopo->addParamVertex( pv, mapID );

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
    // required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // optional attributes
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
        // first numReqAttrs are required
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

        // optional mid attribute
        ++iattr;
        int mid = -1;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // optional aref attribute
        ++iattr;
        int aref = -1;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default is parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        // optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format) {
            std::cout << "MeshFace: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // optional count attribute
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
                    // map parametric verts from parent
                    pv1 = parentMeshTopo->getParamVertByVref(std::to_string(i1));
                    pv2 = parentMeshTopo->getParamVertByVref(std::to_string(i2));
                    pv3 = parentMeshTopo->getParamVertByVref(std::to_string(i3));
                    model->addFaceEdge(i1, i2, mid, aref, gref, pv1, pv2);
                    model->addFaceEdge(i2, i3, mid, aref, gref, pv2, pv3);
                    model->addFaceEdge(i3, i1, mid, aref, gref, pv3, pv1);

                    model->addFaceEdgePoint(i1, mid, aref, gref, pv1);
                    model->addFaceEdgePoint(i2, mid, aref, gref, pv2);
                    model->addFaceEdgePoint(i3, mid, aref, gref, pv3);

                    // record map from mesh face indices to geometry group ID
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
                    pw_assert(face);
                    face = meshSheet->findFaceByInds(i1, i2, i3);
                    pw_assert(face);
#endif

                }
                else if (!triFace) {
                    // 1 group of 4
                    is >> i1 >> i2 >> i3 >> i4;

                    // map parametric verts from parent
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
                    pw_assert(face);
                    face = meshSheet->findFaceByInds(i1, i2, i3, i4);
                    pw_assert(face);
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
    // required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // optional attributes
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
        // first numReqAttrs are required
        for (n = 0; n < numReqAttrs; ++n) {
            if (attrs[n] == "") {
                std::cout << "MeshFaceReference: missing required attribute: " <<
                    attributeNames[n] << std::endl;
                return false;
            }
        }

        // required etype attribute
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

        // optional mid attribute
        ++iattr;
        int mid = -1;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // optional format attribute
        ++iattr;
        std::string format = "text";
        if (attrs[iattr] != "") {
            format = attrs[iattr];
        }
        if ("text" != format) {
            std::cout << "MeshFaceReference: illegal format value: " <<
                format << std::endl;
            return false;
        }

        // optional count attribute
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

        // optional aref attribute
        ++iattr;
        int aref = -1;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        if (!name.empty() && count > 1) {
            printf("MeshFaceReference: error name att cannot be specified when count > 1\n   %s\n",
                name.c_str());
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

                // record map from mesh face indices to geometry group ID
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
                pw_assert(face);
                face = meshSheet->getMeshFaceByRef(ref);
                pw_assert(face);
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
    // required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // optional attributes
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
        // first numReqAttrs are required
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

        // optional mid attribute
        ++iattr;
        int mid = -1;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // optional format attribute
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

        // optional count attribute
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

        // optional aref attribute
        ++iattr;
        int aref = -1;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        if ("" != value && count > 0) {
            std::istringstream is(value.c_str());
            MLINT i1, i2;
            for (int icnt = 0; icnt < count; ++icnt) {
                // 1 group of 2
                is >> i1 >> i2;

                // map parametric verts from parent
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
                pw_assert(edge);
                edge = meshString->findEdgeByInds(i1, i2);
                pw_assert(edge);
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
    // required attributes
    attributeNames.push_back("etype");
    int numReqAttrs = (int)attributeNames.size();

    // optional attributes
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
        // first numReqAttrs are required
        for (n = 0; n < numReqAttrs; ++n) {
            if (attrs[n] == "") {
                std::cout << "MeshEdgeReference: missing required attribute: " <<
                    attributeNames[n] << std::endl;
                return false;
            }
        }

        // required etype attribute
        std::string etype = attrs[iattr];
        if ("Edge2" != etype) {
            std::cout << "MeshEdgeReference: illegal etype value: " <<
                etype << std::endl;
            return false;
        }

        // optional mid attribute
        ++iattr;
        int mid = -1;
        bool mapID = false;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> mid;
            mapID = true;
        }

        // optional format attribute
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

        // optional count attribute
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

        // optional aref attribute
        ++iattr;
        int aref = -1;
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> aref;
        }

        // optional gref attribute
        ++iattr;
        int gref = (int)parentMeshTopo->getGref();  // default to parent's Gref
        if (attrs[iattr] != "") {
            std::istringstream is(attrs[iattr]);
            is >> gref;
        }

        // optional name attribute
        ++iattr;
        std::string name = attrs[iattr];

        ParamVertex *pv1 = NULL;
        ParamVertex *pv2 = NULL;

        if (!name.empty() && count > 1) {
            printf("MeshEdgeReference: error name att cannot be specified when count > 1\n   %s\n",
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
                pw_assert(edge);
                edge = meshString->getMeshEdgeByRef(ref);
                pw_assert(edge);
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
    MeshAssociativity *meshAssociativity
)
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
        }

        if (nullptr == (meshLinkRoot = doc->getDocumentElement())) {
            std::cout << "Mesh Link root element not found." << std::endl;
            XMLPlatformUtils::Terminate();
            result = false;
        }

        // Attribute and AttributeGroup elements
        parseAttributes(meshLinkRoot);

        // GeometryFile, GeometryReference and GeometryGroup elements 
        parseGeometryRefs(meshLinkRoot);

        // MeshFile elements
        XMLCopier<char, XMLCh> meshFileTag("MeshFile");
        DOMNodeList *meshFiles = meshLinkRoot->getElementsByTagName(meshFileTag);

        // Parse MeshFiles - multiple ModelReferences each containing
        // multiple MeshSheets, each containing multiple ParamVertices
        // (points) and one MeshFaceArray
        result = true;
        for (XMLSize_t i = 0; i < meshFiles->getLength() && result; ++i) {
            DOMElement *meshFile;
            DOMNode *node = meshFiles->item(i);
            if (nullptr != node &&
                    DOMNode::ELEMENT_NODE == node->getNodeType()) {
                meshFile = dynamic_cast<DOMElement*>(node);
                result = parseMeshFile(meshFile);
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

