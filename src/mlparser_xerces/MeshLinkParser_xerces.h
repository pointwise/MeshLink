/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

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

class MeshLinkWriterXerces;
class MeshAssociativity;
class MeshModel;
class MeshTopo;
class ParamVertex;

class XMLMeshEdge;
class XMLMeshFace;
class MeshLinkParser;


/****************************************************************************
 * MeshLinkParserXerces class
 ***************************************************************************/
/**
 * \class MeshLinkParserXerces
 *
 * \brief A MeshLink data parser using Apache Xerces XML library.
 *
 * This is a reference implementation of the MeshLinkParser API.
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
    /// Default constructor
    MeshLinkParserXerces():
        meshAssociativity_(NULL),
        verbose_level_(0)
    {}
    /// Destructor
    ~MeshLinkParserXerces();

    /// \brief Validate XML file against MeshLink schema using the Xerces parser
    ///
    /// \param fileName the MeshLink XML file name
    /// \param schemaName the MeshLink XML Schema Definition (XSD) file name
    ///
    /// \return true if the XML conforms to the schema
    bool validate(const std::string &fileName, const std::string &schemaName) override;

    /// \brief Parse a MeshLink XML file and populate a MeshAssociativity using the
    ///     Xerces XML parser.
    ///
    /// \param fname the MeshLink XML file name
    /// \param meshAssociativity the MeshAssociativity object to populate
    ///
    /// \return true if the file was successfully read and the MeshAssociativity object
    ///     was populated
    bool parseMeshLinkFile(std::string fname, MeshAssociativity *meshAssociativity) override;

    /// \brief Obtain the MeshLink XML file properties after reading
    ///
    /// \param[out] xmlns the XML namespace string
    /// \param[out] xmlns_xsi the XML Schema Instance (XSI) namespace string
    /// \param[out] schemaLocation the file name (path) of the XML Schema Definition
    ///     (XSD) file
    bool getMeshLinkAttributes(std::string &xmlns, std::string &xmlns_xsi,
        std::string &schemaLocation) override
    {
        if (xmlns_.empty()) {
            return false;
        }
        xmlns = xmlns_;
        xmlns_xsi = xmlns_xsi_;
        schemaLocation = schemaLocation_;
        return true;
    }

    /// \brief Return a new MeshLinkWriter object using the Xerces API
    ///
    /// It is the responsibility of the caller to delete the returned object.
    MeshLinkWriterXerces *getXMLWriter();

    /// \brief Set the verbosity of the parser message output.
    ///
    /// Larger values result in more output. 0 = errors only.
    void setVerboseLevel(int level) {
        verbose_level_ = level;
    }
private:
    /// \brief Populate the MeshLinkAttribute objects
    bool parseAttributes(xercesc_3_2::DOMElement *root);

    /// \brief Populate the Transform and MeshElementLinkage objects
    bool parsePeriodicInfo(xercesc_3_2::DOMElement *root);

    /// \brief Populate the GeometryGroup objects
    bool parseGeometryRefs(xercesc_3_2::DOMElement *root);

    /// \brief Read the MeshLink XML into a Xerces DOM model
    bool parseMeshFile(xercesc_3_2::DOMElement *element);
    /// \brief Create a MeshModel from the Xerces DOM model
    bool parseMeshRefModel(MeshFile &meshFile, xercesc_3_2::DOMElement *modelRef);
    /// \brief Create a MeshSheet in the given model from the Xerces DOM model
    bool parseMeshSheet(MeshModel *model, xercesc_3_2::DOMElement *meshSheetNode);
    /// \brief Create a MeshSheet from reference data in the given model from the Xerces DOM model
    bool parseMeshSheetReference(MeshModel *model, xercesc_3_2::DOMElement *meshSheetNode);
    /// \brief Create a MeshString from reference data in the given model from the Xerces DOM model
    bool parseMeshString(MeshModel *model, xercesc_3_2::DOMElement *meshStringNode);
    /// \brief Create a MeshString from reference data in the given model from the Xerces DOM model
    bool parseMeshStringReference(MeshModel *model, xercesc_3_2::DOMElement *meshStringNode);
    /// \brief Create a ParamVertex for the given MeshTopo entity from the Xerces DOM model
    bool parseParamVertex(MeshTopo *meshTopo, xercesc_3_2::DOMElement *element);
    /// \brief Create a MeshPoint in the given model from the Xerces DOM model
    bool parseMeshPoint(MeshModel *model, xercesc_3_2::DOMElement *meshPoint);
    /// \brief Create a MeshPoint from reference data in the given model from the Xerces DOM model
    bool parseMeshPointReference(MeshModel *model,
        xercesc_3_2::DOMElement *meshPoint);
    /// \brief Create a MeshPoint from reference data in the given model and parent MeshTopo entity
    ///     from the Xerces DOM model
    bool parseMeshPointReference(MeshModel *model, MeshTopo* parentMeshTopo,
        xercesc_3_2::DOMElement *meshPoint);

    /// \brief Create a MeshEdge in the given model and parent MeshTopo entity
    ///     from the Xerces DOM model
    bool parseMeshEdge(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);
    /// \brief Create a MeshEdge from reference data in the given model and parent
    ///     MeshTopo entity from the Xerces DOM model
    bool parseMeshEdgeReference(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);
    /// \brief Create a MeshFace in the given model and parent MeshTopo entity
    ///     from the Xerces DOM model
    bool parseMeshFace(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);
    /// \brief Create a MeshFace from reference data in the given model and parent
    ///     MeshTopo entity from the Xerces DOM model
    bool parseMeshFaceReference(MeshModel *model,
        MeshTopo * meshTopo, xercesc_3_2::DOMElement *element);

    /// \brief Create MeshTopo object in the given MeshModel from the Xerces
    ///     DOM model. The type of object to create is determined from the
    ///     object name and lookup map.
    bool parseMeshObject(MeshModel *model,
        MeshTopo* meshTopo,
        xercesc_3_2::DOMElement *xmlObj,
        const char *objName,
        ParseMeshObjMap &parseMeshObjMap, bool &mapID);

    /// \brief the MeshAssociativity object to be populated
    MeshAssociativity *meshAssociativity_;

    int verbose_level_;

    // Parsed Xml 'header' attributes cached for use when writing out
    // Xml file based on parsed/modified Meshassociativity. Supports
    // 'round tripping' of MeshLink files

    /// The XML version string read from the input file
    std::string xmlVersion_;
    /// The XML Namespace string read from the input file
    std::string xmlns_;
    /// The XML XSI Namespace string read from the input file
    std::string xmlns_xsi_;
    /// The XSD file name read from the input file
    std::string schemaLocation_;
};

#endif

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
