/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#ifndef MESH_LINK_WRITER
#define MESH_LINK_WRITER

#include <string>

class MeshAssociativity;

/****************************************************************************
* MeshLinkWriter class
***************************************************************************/
/**
* \class MeshLinkWriter
*
* \brief Base class for MeshLink data writer interface
*
* Provides a neutral interface for writing  MeshLink data to XML
* using a wrapped parsing/writing library.
*
*/
class ML_STORAGE_CLASS MeshLinkWriter {
public:
    MeshLinkWriter() {}
    ~MeshLinkWriter() {};

    // Write MeshAssociativity data to MeshLink XML file
    virtual bool writeMeshLinkFile(const std::string &fname,
        MeshAssociativity *meshAssociativity, bool compress=true,
        bool terminatePlatform=true) { return false; }

    // Need to provide these if using the default constructor (C API
    // call), otherwise XML file will not validate
    virtual bool setMeshLinkAttributes(const std::string &xmlns,
        const std::string &xmlns_xsi, const std::string &schemaLocation) { return false; }

private:
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
