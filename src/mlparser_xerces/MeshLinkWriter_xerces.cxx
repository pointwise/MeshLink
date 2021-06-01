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


#include "MeshLinkWriter_xerces.h"

#include <ctime>
#include <iostream>
#include <iomanip>
#include <iterator>
#include <istream>
#include <memory>
#include <sstream>
#include <stdlib.h>
#define SSTR( x ) static_cast< std::ostringstream & >(          \
        ( std::ostringstream() << std::dec << x ) ).str()

// DOM Parser
#include <xercesc/dom/DOM.hpp>

// DOM serialization
#include <xercesc/dom/DOMImplementationRegistry.hpp>
#include <xercesc/dom/DOMImplementation.hpp>
#include <xercesc/dom/DOMLSSerializer.hpp>
#include <xercesc/dom/DOMLSOutput.hpp>
#include <xercesc/framework/MemBufFormatTarget.hpp>
#include <xercesc/util/XercesDefs.hpp>
#include <xercesc/util/XMLUni.hpp>
#include <xercesc/util/Base64.hpp>
#include <fstream>

using namespace xercesc;

static const int MAX_ID = 1024;

class PlatformGuard {
public:
    PlatformGuard(bool terminate) :
        terminate_(terminate),
        init_(false)
    {
        init();
    }

    ~PlatformGuard()
    {
        if (terminate_ && init_) {
            XMLPlatformUtils::Terminate();
            std::cout << "Successfully terminated xerces API." << std::endl;
        }
    }

    operator bool()
    {
        return init_;
    }
private:
    void init()
    {
        init_ = true;
        try {
            XMLPlatformUtils::Initialize();
            std::cout << "Successfully initialized xerces API." << std::endl;
        }
        catch (const XMLException &) {
            std::cout << "Failed to initialize xerces API." << std::endl;
            init_ = false;
        }
    }

    bool terminate_;
    bool init_;
};

// Replaces xerces XMLString::transcode() method to take care of
// releasing the transcoded result automatically (inefficent but convenient!)
template <typename T, typename U>
class XMLCopier {
public:
    XMLCopier(const T *input) :
        input_(input)
    {
        val_ = XMLString::transcode(input_);
    }

    ~XMLCopier() {
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

    bool empty() const
    {
        return !(XMLString::stringLen(val_) > 0);
    }

private:
    const T *input_;
    U *val_;
};

static const char *NSStr{"https://pointwise.com"};

// Very temporary - use in function calls only
#define X(str) XMLCopier<char, XMLCh>(str).convert()
#define Char(str) XMLCopier<XMLCh, char>(str).convert()


static auto setUintAtt = [](MLINT id, DOMElement *node, const char *s)
{
    char buffer[MAX_ID];
    sprintf(buffer, "%ld", (long)id);
    node->setAttribute(X(s), X(buffer));
};


bool
MeshLinkWriterXerces::writeRootAttributes()
{
    bool result = true;

    std::vector<const MeshLinkAttribute *> atts = meshAssoc_->getAttributes();
    for (auto att : atts) {
        MLINT attID = att->getAttID();
        const char *name{ nullptr };
        const char *value{ nullptr };
        meshAssoc_->getAttribute(attID, &name, &value);
        if (nullptr != name && nullptr != value) {
            // Create the element, assign the attID and name
            DOMElement *node;
            if (att->isGroup()) {
                node = meshDoc_->createElementNS(X(NSStr), X("AttributeGroup"));
            }
            else {
                node = meshDoc_->createElementNS(X(NSStr), X("Attribute"));
            }
            setUintAtt(attID, node, "attid");
            node->setAttribute(X("name"), X(name));
            // Add child node of type DOMNode::TEXT_NODE to hold the value
            DOMText *textNode = meshDoc_->createTextNode(X(value));
            node->appendChild(textNode);
            meshLinkRoot_->appendChild(node);
        }
        else {
            std::cout << "Failed to getAttribute (id: " << attID <<
                ") name and/or value." << std::endl;
        }
        // Get the next attribute
        att = meshAssoc_->getAttributeByID(++attID);
    }
    return result;
}


bool
MeshLinkWriterXerces::writePeriodicInfo()
{
    bool result = true;

    std::vector<const MeshLinkTransform *> xforms;
    meshAssoc_->getTransforms(xforms);
    for (auto xform : xforms) {
        // Create the element, assign the attributes
        DOMElement *node;
        node = meshDoc_->createElementNS(X(NSStr), X("Transform"));
        if (node) {
            setUintAtt(xform->getXID(), node, "xid");

            const std::string &name = xform->getName();
            if (!name.empty()) {
                node->setAttribute(X("name"), X(name.c_str()));
            }

            if (xform->hasAref()) {
                setUintAtt(xform->getAref(), node, "aref");
            }

            const std::string &contents = xform->getContents();
            if (!contents.empty()) {
                // Add child node of type DOMNode::TEXT_NODE to hold the contents
                DOMText *textNode = meshDoc_->createTextNode(X(contents.c_str()));
                node->appendChild(textNode);
            }
            meshLinkRoot_->appendChild(node);
        }
    }


    std::vector<MeshElementLinkage *> links;
    meshAssoc_->getMeshElementLinkages(links);
    for (auto link : links) {
        // Create the element, assign the attributes
        DOMElement *node;
        node = meshDoc_->createElementNS(X(NSStr), X("MeshElementLinkage"));
        if (node) {
            std::string sourceEntityRef;
            std::string targetEntityRef;
            link->getEntityRefs(sourceEntityRef, targetEntityRef);
            node->setAttribute(X("sourceEntityRef"), X(sourceEntityRef.c_str()));
            node->setAttribute(X("targetEntityRef"), X(targetEntityRef.c_str()));


            const char *name{ nullptr };
            link->getName(&name);
            node->setAttribute(X("name"), X(name));

            if (link->hasAref()) {
                setUintAtt(link->getAref(), node, "aref");
            }
            MLINT xref;
            if (link->getXref(&xref)) {
                setUintAtt(xref, node, "xref");
            }
            meshLinkRoot_->appendChild(node);
        }
    }

    return result;
}

bool
MeshLinkWriterXerces::writeGeometryRefs()
{
    // Geometry File
    //  -- GeometryReference
    char buf[MAX_ID];
    MLINT id;
    // Stores the GeometryGroup ids of GeometryReferences that are
    // part of GeometryGroups
    std::set<MLINT> groupIDs;

    // Process the GeometryFiles
    const std::vector<GeometryFile> &geomFiles =
        meshAssoc_->getGeometryFiles();
    for (auto geomFile : geomFiles) {
        // Create DOMElement for each geometry file
        DOMElement *fileElement = meshDoc_->createElementNS(X(NSStr),
            X("GeometryFile"));
        fileElement->setAttribute(X("filename"), X(geomFile.getFilename()));
        MLINT aref;
        if (geomFile.getArefID(&aref)) {
            setUintAtt(aref, fileElement, "aref");
        }

        // Get the GeometryReferences
        std::vector<MLINT> gids = geomFile.getGeometryGroupIDs();
        for (auto gid : gids) {
            GeometryGroup *group = meshAssoc_->getGeometryGroupByID(gid);
            if (nullptr != group) {
                DOMElement *refElement = meshDoc_->createElementNS(X(NSStr),
                    X("GeometryReference"));
                setUintAtt(group->getID(), refElement, "gid");
                if (group->getArefID(&id)) {
                    setUintAtt(id, refElement, "aref");
                }
                // Entity name - should only be one
                const std::set<std::string> &names = group->getEntityNameSet();
                refElement->setAttribute(X("ref"), X(names.begin()->c_str()));
                fileElement->appendChild(refElement);
                // Get the Group id and add to groupIDs set
                MLINT groupID;
                if (group->getGroupID(groupID)) {
                    groupIDs.insert(groupID);
                }
            }
        }
        meshLinkRoot_->appendChild(fileElement);
    }

    // Geometry Group - we only know about them if they're in the groupIDs set
    if (groupIDs.empty()) {
        return true;
    }

    for (auto gid : groupIDs) {
        // Get the GeometryGroup
        GeometryGroup *group = meshAssoc_->getGeometryGroupByID(gid);
        DOMElement *elem = meshDoc_->createElementNS(X(NSStr),
            X("GeometryGroup"));
        // gid
        setUintAtt(group->getID(), elem, "gid");
        // name
        elem->setAttribute(X("name"), X(group->getName().c_str()));
        // ids of GeometryReferences that make this a group
        const std::vector<MLINT> &groupIDs = group->getGIDs();
        if (!groupIDs.empty()) {
            std::string strIDs;
            for (auto id : groupIDs) {
                sprintf(buf, "%ld", (long)id);
                strIDs += std::string(buf) + std::string(" ");
            }
            strIDs.pop_back();
            DOMText *textNode = meshDoc_->createTextNode(X(strIDs.c_str()));
            elem->appendChild(textNode);
        }
        meshLinkRoot_->appendChild(elem);
    }

    return true;
}


bool
MeshLinkWriterXerces::writeParamVertices(xercesc_3_2::DOMElement *node,
    const ParamVertVrefMap &vertMap)
{
    if (nullptr == node || 0 == vertMap.size()) {
        return false;
    }

    // n ParameVertices
    ParamVertVrefMap::const_iterator pvIter = vertMap.begin();
    while (pvIter != vertMap.end()) {
        DOMElement *vertex = meshDoc_->createElementNS(X(NSStr), X("ParamVertex"));
        const std::string &vref = pvIter->first;
        const ParamVertex *vert = pvIter->second;
        vertex->setAttribute(X("vref"), X(vref.c_str()));
        if (MESH_TOPO_INVALID_REF < vert->getGref()) {
            setUintAtt(vert->getGref(), vertex, "gref");
        }
        if (MESH_TOPO_INVALID_REF < vert->getID()) {
            setUintAtt(vert->getID(), vertex, "mid");
        }
        vertex->setAttribute(X("dim"), X("2"));
        MLREAL u, v;
        vert->getUV(&u, &v);
        char buf[128];
        sprintf(buf, "%.15g %.15g", u, v);
        DOMText *textNode = meshDoc_->createTextNode(X(buf));
        vertex->appendChild(textNode);
        node->appendChild(vertex);
        ++pvIter;
    }
    return true;
}


static XMLSize_t
encodeCData(std::vector<int> &data, XMLByte *&encodedData)
{
    XMLSize_t length;
    size_t bytesToEncode = data.size() * sizeof(int);
    encodedData = Base64::encode((XMLByte *)(data.data()), bytesToEncode, &length);

    return length;
}


bool
MeshLinkWriterXerces::writeMeshPointReferences(xercesc_3_2::DOMElement *model,
    MeshModel *meshModel)
{

    const ParamVertVrefMap &vertMap = meshModel->getParamVertVrefMap();
    if (vertMap.empty()) {
        return false;
    }
    DOMElement *node = meshDoc_->createElementNS(X(NSStr),
        X("MeshPointReference"));

    // Write out MeshPointReference node first, then the ParamVertex nodes
    // Common gref for MeshPointReference and each ParamVertex
    MLINT gref = MESH_TOPO_INVALID_REF;
    ParamVertVrefMap::const_iterator iter = vertMap.begin();
    std::vector<std::string> ids;
    while (iter != vertMap.end()) {
        ids.push_back(iter->first);
        if (MESH_TOPO_INVALID_REF == gref) {
            gref = iter->second->getGref();
        }
        else if( gref != iter->second->getGref()) {
            assert(0 == "MeshPointReference: grefs not identical");
        }
        ++iter;
    }
    setUintAtt(gref, node, "gref");
    setUintAtt(ids.size(), node, "count");
    std::ostringstream os;
    size_t index = 1;
    for (auto id : ids) {
        os << id;
        if (index < ids.size()) {
            os << " ";
        }
        ++index;
    }
    DOMText *textNode = meshDoc_->createTextNode(X(os.str().c_str()));
    node->appendChild(textNode);
    model->appendChild(node);

    // Now for the ParamVerts
    writeParamVertices(model, vertMap);
    return true;
}


bool
MeshLinkWriterXerces::writeMeshFace(xercesc_3_2::DOMElement *sheet,
    MeshTopo *container)
{
    MeshSheet *meshSheet = dynamic_cast<MeshSheet*>(container);
    if (nullptr == meshSheet) {
        return false;
    }
    std::vector<const MeshFace *> faces;
    meshSheet->getMeshFaces(faces);
    if (faces.empty()) {
        return false;
    }
    size_t count = faces.size();
    const MeshFace *face = faces[0]; // no bounds checking using []

    // Check this face - if indices are not valid, this is a
    // MeshFaceReference
    MLINT numInds;
    MLINT *inds = new MLINT[4];
    face->getInds(inds, &numInds);
    bool reference = (0 == numInds);

    DOMElement *node;
    if (!reference) {
        node = meshDoc_->createElementNS(X(NSStr), X("MeshFace"));
    }
    else {
        node = meshDoc_->createElementNS(X(NSStr), X("MeshFaceReference"));
    }

    // Attributes

    // Can't put name out becaue the parser will assign it
    // to every face in the MeshFace, which will abort import with
    // face name collisions
    // node->setAttribute(X("name"), X(face->getName().c_str()));

    if (MESH_TOPO_INVALID_REF != face->getAref()) {
        setUintAtt(face->getAref(), node, "aref");
    }
    if (MESH_TOPO_INVALID_REF != face->getID()) {
        setUintAtt(face->getID(), node, "mid");
    }
    setUintAtt(count, node, "count");

    MLINT faceType = reference ? count : numInds;
    // etype
    bool triFace = 3 == faceType ? true : false;
    if (triFace) {
        node->setAttribute(X("etype"), X("Tri3"));
    }
    else {
        node->setAttribute(X("etype"), X("Quad4"));
    }

    // Add text node containing face info
    DOMText *faceData;
    if (!reference && compress_) {
        // Get vector of all the face indices
        std::vector<int> indices;
        indices.reserve(faces.size() * (triFace ? 3 : 4));
        for (auto face : faces) {
            // Indices
            face->getInds(inds, &numInds);
            indices.push_back((int)inds[0]);
            indices.push_back((int)inds[1]);
            indices.push_back((int)inds[2]);
            if (!triFace) {
                indices.push_back((int)inds[3]);
            }
        }
        XMLByte *encodedData{ nullptr };
        XMLSize_t len;
        try {
            len = encodeCData(indices, encodedData);
        }
        catch (...) {
            std::cout << "Face data: Base64 encoding failed.\n";
            return false;
        }
        // Store uncompressed size in count
        if (0 < len) {
            node->setAttribute(X("format"), X("base64"));
            faceData = meshDoc_->createTextNode(X("\n\t"));
            faceData->appendData(X((char*)encodedData));
            faceData->appendData(X("\t"));
            XMLString::release((char**)(&encodedData));
        }
        else {
            std::cout << "Failed to compress face data.\n";
            return false;
        }
    }
    else {
        // Put all the face indices into a string
        std::ostringstream os;
        bool first = true;
        size_t index = 1;
        std::vector<const MeshFace *>::const_iterator it = faces.begin();
        for (; it != faces.end(); ++it, ++index) {
            face = *it;
            if (first) {
                first = false;
                os << "\n\t ";
            }
            if (!reference) {
                // Indices
                face->getInds(inds, &numInds);
                os << inds[0] << " " << inds[1] << " " << inds[2];
                if (!triFace) {
                    os << " " << inds[3];
                }
                if (index == count) {
                    os << "\n\t";
                }
                else {
                    os << "\n\t ";
                }
            }
            else {
                os << face->getRef();
                if (index == count) {
                    os << "\n\t";
                }
                else {
                    os << " ";
                }
            }
        }
        node->setAttribute(X("format"), X("text"));
        faceData = meshDoc_->createTextNode(X(os.str().c_str()));
    }
    delete [] inds;
    node->appendChild(faceData);
    sheet->appendChild(node);
    return true;
}


bool
MeshLinkWriterXerces::writeMeshEdge(xercesc_3_2::DOMElement *string,
    MeshTopo *container)
{
    MeshString *meshString = dynamic_cast<MeshString*>(container);
    if (nullptr == meshString) {
        return false;
    }
    std::vector<const MeshEdge *> edges;
    meshString->getMeshEdges(edges);
    if (edges.empty()) {
        return false;
    }
    size_t count = edges.size();
    const MeshEdge *edge = edges[0]; // no bounds checking using []
    MLINT numInds;
    MLINT *inds = new MLINT[2];
    edge->getInds(inds, &numInds);
    bool reference = (0 == numInds);

    DOMElement *node;
    if (!reference) {
        node = meshDoc_->createElementNS(X(NSStr), X("MeshEdge"));
    }
    else {
        node = meshDoc_->createElementNS(X(NSStr), X("MeshEdgeReference"));
    }

    // Attributes
    if (MESH_TOPO_INVALID_REF != edge->getAref()) {
        setUintAtt(edge->getAref(), node, "aref");
    }
    if (MESH_TOPO_INVALID_REF != edge->getID()) {
        setUintAtt(edge->getID(), node, "mid");
    }
    node->setAttribute(X("format"), X("text"));
    // eType
    node->setAttribute(X("etype"), X("Edge2"));
    setUintAtt(count, node, "count");
    std::ostringstream os;
    size_t index = 1;
    for (auto edge : edges) {
        // Indices
        if (!reference) {
            edge->getInds(inds, &numInds);
            os << inds[0] << " " << inds[1];
            if (index < count) {
                os << " ";
            }
        }
        else {
            os << edge->getRef();
            if (index < count) {
                os << " ";
            }
        }
        ++index;
    }
    delete [] inds;
    // Add text node containing edge info
    DOMText *textNode = meshDoc_->createTextNode(X(os.str().c_str()));
    node->appendChild(textNode);
    string->appendChild(node);
    return true;
}


#define WriteMeshContainer(ParentNode, MeshObj, MeshTopo)               \
{                                                                       \
    std::vector<MeshObj *> objs;                                        \
    meshModelRef->get##MeshObj##s(objs);                                \
    for (auto obj : objs) {                                             \
        DOMElement *node = meshDoc_->createElementNS(X(NSStr), X(#MeshObj)); \
        node->setAttribute(X("name"), X(obj->getName().c_str()));       \
        if (MESH_TOPO_INVALID_REF != obj->getGref()) {                  \
            setUintAtt(obj->getGref(), node, "gref");                   \
        }                                                               \
        if (MESH_TOPO_INVALID_REF != obj->getAref()) {                  \
            setUintAtt(obj->getAref(), node, "aref");                   \
        }                                                               \
        if (MESH_TOPO_INVALID_REF != obj->getID()) {                    \
            setUintAtt(obj->getID(), node, "mid");                      \
        }                                                               \
        const ParamVertVrefMap &vertMap = obj->getParamVertVrefMap();   \
        writeParamVertices(node, vertMap);                              \
        write##MeshTopo(node, obj);                                     \
        ParentNode->appendChild(node);                                  \
    }                                                                   \
}


bool
MeshLinkWriterXerces::writeMeshFiles(bool compress)
{
    if (compress) {
        return true;
    }

    // Mesh files
    const std::vector<MeshFile> &meshFiles = meshAssoc_->getMeshFiles();
    for (auto meshFile : meshFiles) {
        DOMElement *fileElement = meshDoc_->createElementNS(X(NSStr),
            X("MeshFile"));
        fileElement->setAttribute(X("filename"), X(meshFile.getFilename()));

        // 1-n MeshModelReferences
        const std::vector<std::string> &modelRefs = meshFile.getModelRefs();
        for (auto ref : modelRefs) {
            MeshModel *meshModelRef = meshAssoc_->getMeshModelByRef(ref.c_str());
            DOMElement *model = meshDoc_->createElementNS(X(NSStr),
                X("MeshModelReference"));
            model->setAttribute(X("ref"), X(meshModelRef->getRef().c_str()));
            const std::string &name = meshModelRef->getName();
            if ( !name.empty() ) {
                // name
                model->setAttribute(X("name"), X(name.c_str()));
            }
            if (MESH_TOPO_INVALID_REF != meshModelRef->getGref()) {
                setUintAtt(meshModelRef->getGref(), model, "gref");
            }
            if (MESH_TOPO_INVALID_REF != meshModelRef->getAref()) {
                setUintAtt(meshModelRef->getAref(), model, "aref");
            }
            if (MESH_TOPO_INVALID_REF != meshModelRef->getID()) {
                setUintAtt(meshModelRef->getID(), model, "mid");
            }

            // n MeshPoints
            writeMeshPointReferences(model, meshModelRef);

            // n Mesh sheets
            WriteMeshContainer(model, MeshSheet, MeshFace);

            // n Mesh strings
            WriteMeshContainer(model, MeshString, MeshEdge);

            fileElement->appendChild(model);
        }
        meshLinkRoot_->appendChild(fileElement);
    }

    return true;
}


// Write MeshAssociativity to MeshLink file (XML roundtrip)
bool
MeshLinkWriterXerces::writeMeshLinkFile(const std::string &fname,
    MeshAssociativity *meshAssociativity, bool compress, bool terminatePlatform)
{
    if (nullptr != meshAssociativity) {
        meshAssoc_ = meshAssociativity;
    }
    if (nullptr == meshAssoc_) {
        std::cout << "Meshassociativity can't be null.\n";
        return false;
    }
    compress_ = compress;

    PlatformGuard guard(terminatePlatform);
    if (!guard) {
        return false;
    }
    bool result = true;

    DOMImplementation *impl =
        DOMImplementationRegistry::getDOMImplementation(X("LS"));

    if (nullptr == impl) {
        std::cout << "Failed to create DOM Implementation." << std::endl;
        return false;
    }

    meshDoc_ = impl->createDocument(X("https://pointwise.com"),
        X("MeshLink"), nullptr);
    if (nullptr == meshDoc_) {
        std::cout << "Failed to create DOM document." << std::endl;
        result = false;
    }
    meshDoc_->setXmlStandalone(true);

    if (nullptr == (meshLinkRoot_ = meshDoc_->getDocumentElement())) {
        std::cout << "Failed to obtain Mesh Link root element." << std::endl;
        result = false;
    }

    meshLinkRoot_->setAttribute(X("version"), X("1.0"));
    if (!xmlns_xsi_.empty()) {
        meshLinkRoot_->setAttribute(X("xmlns:xsi"), X(xmlns_xsi_.c_str()));
    }
    if (!schemaLocation_.empty()) {
        meshLinkRoot_->setAttribute(X("xsi:schemaLocation"),
            X(schemaLocation_.c_str()));
    }

    // Date/Time
    std::time_t t = std::time(nullptr);
    time(&t);
    std::string time = ctime(&t);
    time.pop_back();
    DOMComment *timeComment = meshDoc_->createComment(X(time.c_str()));
    meshLinkRoot_->appendChild(timeComment);

    // Attribute and AttributeGroup elements
    if (!writeRootAttributes()) {
        std::cout << "Failed to write Mesh Link root attributes." << std::endl;
        return false;
    }

    if (!writeGeometryRefs()) {
        std::cout << "Failed to write Geometry references." << std::endl;
        return false;
    }

    // Write MeshFiles - multiple ModelReferences each containing
    // multiple MeshSheets, each containing multiple ParamVertices
    // (points) and one MeshFaceArray - which can be compressed in a
    // CDATA section if called with 'true'
    if (!writeMeshFiles(false)) {
        std::cout << "Failed to write Mesh Files." << std::endl;
        return false;
    }

    // Transform and MeshElementLinkage elements
    if (!writePeriodicInfo()) {
        std::cout << "Failed to write Mesh Link transform and linkage info." << std::endl;
        return false;
    }


    // Set up raw buffer, output stream objects
    MemBufFormatTarget *buffer = new MemBufFormatTarget();
    DOMLSOutput *stream = impl->createLSOutput();

    // Set encoding
    stream->setByteStream(buffer);
    stream->setEncoding(XMLUni::fgUTF8EncodingString);

    // Create serializer and output (serialize) the root DOM node...
    DOMLSSerializer *writer = impl->createLSSerializer();
    writer->setNewLine(X("\n"));
    DOMConfiguration *config = writer->getDomConfig();
    config->setParameter(XMLUni::fgDOMWRTFormatPrettyPrint, true);
    writer->write(meshDoc_, stream);

    // Get char* array containing serialized document and write to file
    std::string xmlOutput  = (char*)(buffer->getRawBuffer());
    std::ofstream outFile(fname);
    outFile << xmlOutput;
    outFile.close();
    std::cout << "Completed writing " << fname << std::endl;

    return result;
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
