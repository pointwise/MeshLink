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
