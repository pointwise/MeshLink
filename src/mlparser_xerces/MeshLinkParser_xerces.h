#ifndef MESH_LINK_PARSER_XERCES
#define MESH_LINK_PARSER_XERCES

#include "Types.h"
#include "MeshLinkParser.h"

#include <map>
#include <string>
#include <vector>


namespace xercesc_3_2 {
    class DOMNode;
    class DOMElement;
}

class MeshAssociativity;
class MeshModel;
class MeshTopo;
//class MeshVertex;
class ParamVertex;

class XMLMeshEdge;
class XMLMeshFace;
class MeshLinkParser;

//typedef std::map<MLINT, XMLMeshEdge*> XMLMeshEdges;
//typedef std::map<MLINT, XMLMeshFace*> XMLMeshFaces;
//typedef std::map<MLINT, MeshVertex*> MeshVerts;



/****************************************************************************
* MeshLinkParserXerces class
***************************************************************************/
/**
* \class MeshLinkParserXerces
*
* \brief A MeshLink data parser using Apache Xerces XML library
*
*
*/
class MeshLinkParserXerces;

// Function pointer typedef for MeshTopo parser, e.g. parseMeshEdge
typedef bool (MeshLinkParserXerces::*pParseMeshObj)(MeshModel *model,
    MeshTopo* meshTopo,
    xercesc_3_2::DOMElement *domElement);

// Map MeshTopo element name to parser function 
typedef std::map<const char *, pParseMeshObj> ParseMeshObjMap;

class MeshLinkParserXerces : public MeshLinkParser {
public:
    MeshLinkParserXerces():
        meshAssociativity_(NULL)
    {}
    ~MeshLinkParserXerces();

    // Validate xml file against Mesh link schema
    bool validate(const std::string &fileName, const std::string &schemaName);

    bool parseMeshLinkFile(std::string fname, MeshAssociativity *meshAssociativity);

private:

    // parsing mesh object storage
    //void            addMeshObj(MeshObjPtr meshObj);
    //MeshObjPtr      findMeshObj(MLINT id);
    //void            clearMeshObjs();

    bool parseAttributes(xercesc_3_2::DOMElement *root);

    bool parseGeometryRefs(xercesc_3_2::DOMElement *root);

    bool parseMeshFile(xercesc_3_2::DOMElement *element);
    bool parseMeshRefModel(xercesc_3_2::DOMElement *modelRef);
    bool parseMeshSheet(MeshModel *model, xercesc_3_2::DOMElement *meshSheetNode);
    bool parseMeshSheetReference(MeshModel *model, xercesc_3_2::DOMElement *meshSheetNode);
    bool parseMeshString(MeshModel *model, xercesc_3_2::DOMElement *meshStringNode);
    bool parseMeshStringReference(MeshModel *model, xercesc_3_2::DOMElement *meshStringNode);
    bool parseParamVertex(MeshTopo *meshTopo, xercesc_3_2::DOMElement *element);
    bool parseMeshPoint(MeshModel *model, xercesc_3_2::DOMElement *meshPoint);
    bool parseMeshPointReference(MeshModel *model,
        xercesc_3_2::DOMElement *meshPoint);
    bool parseMeshPointReference(MeshModel *model, MeshTopo* parentMeshTopo,
        xercesc_3_2::DOMElement *meshPoint);

    // MeshTopo parsing functions must match typedef defined above
    bool parseMeshEdge(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);
    bool parseMeshEdgeReference(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);
    bool parseMeshFace(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);
    bool parseMeshFaceReference(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);

    // Parse generic MeshTopo element using parsing function map
    bool parseMeshObject(MeshModel *model,
        MeshTopo* meshTopo,
        xercesc_3_2::DOMElement *xmlObj,
        const char *objName,
        ParseMeshObjMap &parseMeshObjMap, bool &mapID);

    MeshAssociativity *meshAssociativity_;

    // Parse objects
    //MeshObjs meshObjs_;
};



//class MeshVertex {
//public:
//    MeshVertex(MLINT id, MLINT owner) : mid_(id), owner_(owner) {}
//
//    MLINT mid_;
//    MLINT owner_;
//    MLINT gref_;
//    MLINT vref_;
//    std::vector<double> points_;
//};
//
//class XMLMeshFace {
//public:
//    XMLMeshFace(MLINT id, MLINT owner) : mid_(id), owner_(owner) {}
//
//    MLINT mid_;
//    MLINT owner_;
//    std::string etype_;
//    std::string format_;
//    std::vector<MLINT> vrefs_;
//};
//
//class XMLMeshEdge {
//public:
//    XMLMeshEdge(MLINT id, MLINT owner) : mid_(id), owner_(owner) {}
//
//    MLINT mid_;
//    MLINT owner_;
//    std::string etype_;
//    std::string format_;
//    std::vector<MLINT> vrefs_;
//};
//

#endif

