/****************************************************************************
 *
 * Copyright (c) 2019-2020 Pointwise, Inc.
 * All rights reserved.
 *
 * This sample Pointwise source code is not supported by Pointwise, Inc.
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
 * DISCLAIMER:
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, POINTWISE DISCLAIMS
 * ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, WITH REGARD TO THIS SCRIPT. TO THE MAXIMUM EXTENT PERMITTED
 * BY APPLICABLE LAW, IN NO EVENT SHALL POINTWISE BE LIABLE TO ANY PARTY
 * FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE
 * USE OF OR INABILITY TO USE THIS SCRIPT EVEN IF POINTWISE HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES AND REGARDLESS OF THE
 * FAULT OR NEGLIGENCE OF POINTWISE.
 *
 ***************************************************************************/
