/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#ifndef MESH_LINK_WRITER_XERCES
#define MESH_LINK_WRITER_XERCES

#include "Types.h"
#include "MeshLinkWriter.h"

#include <map>
#include <string>
#include <vector>


namespace xercesc_3_2 {
    class DOMNode;
    class DOMElement;
    class DOMDocument;
}

class MeshAssociativity;
class MeshModel;
class MeshTopo;
//class MeshVertex;
class ParamVertex;


/****************************************************************************
* MeshLinkWriterXerces class
***************************************************************************/
/**
* \class MeshLinkWriterXerces
*
* \brief A MeshLink data writer using Apache Xerces XML library
*
*
*/
class MeshLinkWriterXerces : public MeshLinkWriter {
public:
    MeshLinkWriterXerces()
    {
    }

    MeshLinkWriterXerces(const std::string &xmlns, const std::string &xmlns_xsi,
        const std::string &schemaLocation):
        xmlns_(xmlns),
        xmlns_xsi_(xmlns_xsi),
        schemaLocation_(schemaLocation)
    {}
    ~MeshLinkWriterXerces()
    {
    }

    bool writeMeshLinkFile(const std::string &fname,
        MeshAssociativity *meshAssociativity, bool compress=true,
        bool terminatePlatform=true) override;

    // Need to provide these if using the default constructor (C API
    // call), otherwise XML file will not validate
    bool setMeshLinkAttributes(const std::string &xmlns,
        const std::string &xmlns_xsi, const std::string &schemaLocation) override
    {
        xmlns_ = xmlns;
        xmlns_xsi_ = xmlns_xsi;
        schemaLocation_ = schemaLocation;
        return true;
    }

private:
    bool writeRootAttributes();
    bool writePeriodicInfo();
    bool writeGeometryRefs();
    bool writeMeshFiles(bool compress);

    bool writeParamVertices(xercesc_3_2::DOMElement *node,
             const ParamVertVrefMap &vertMap);
    bool writeMeshFace(xercesc_3_2::DOMElement *sheet, MeshTopo *container);
    bool writeMeshEdge(xercesc_3_2::DOMElement *string, MeshTopo *container);
    bool writeMeshPointReferences(xercesc_3_2::DOMElement *model,
        MeshModel *meshModel);

    bool compress_ { false };
    MeshAssociativity *meshAssoc_ { nullptr };
    xercesc_3_2::DOMDocument *meshDoc_;
    xercesc_3_2::DOMElement *meshLinkRoot_;

    // Parsed Xml 'header' attributes cached for use when writing out
    // Xml file based on parsed/modified Meshassociativity. Supports
    // 'round tripping' of MeshLink files
    std::string xmlns_;
    std::string xmlns_xsi_;
    std::string schemaLocation_;
};
#endif // MESH_LINK_WRITER_XERCES

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
