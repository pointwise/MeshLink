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

#ifndef MESH_LINK_PARSER
#define MESH_LINK_PARSER

#include <string>

class MeshAssociativity;

/****************************************************************************
 * MeshLinkParser class
 ***************************************************************************/
/**
 * \class MeshLinkParser
 *
 * \brief Base class for MeshLink data parser interface
 *
 * Provides a neutral interface for parsing MeshLink XML data
 * according to an XML Schema Definition (XSD) using a wrapped parsing
 * library.
 *
 */
class ML_STORAGE_CLASS MeshLinkParser {
public:
    /// Default constructor
    MeshLinkParser() {}
    /// Destructor
    ~MeshLinkParser() {};

    /// \brief Validate MeshLink XML file against MeshLink schema
    ///
    /// \param fileName the name of the XML file to validate
    /// \param schemaName the name of the schema file used for validation;
    ///     if empty, the XML file must designate the XML Schema
    ///     Definition (XSD) file location
    ///
    /// \return true if XML file is valid within the defintion of the schema
    virtual bool validate(const std::string &fileName,
            const std::string &schemaName) { return false; }

    /// \brief Parse MeshLink xml file into MeshAssociativity
    ///
    /// \param fname the name (path) of the %MeshLink XML file to parse
    /// \param meshAssociativity the MeshAssociativity object to populate
    //
    /// \return true if XML file is valid within the defintion of the schema
    ///         and the file was successfully read into the MeshAssociativity
    virtual bool parseMeshLinkFile(std::string fname,
            MeshAssociativity *meshAssociativity) { return false; }

    /// \brief Return schema-related attributes
    ///
    /// These attributes are typically used when exporting MeshAssociativity
    /// data to a MeshLink XML file.
    ///
    /// \param[out] xmlns the XML namespace string
    /// \param[out] xmlns_xsi the XML Schema Instance string
    /// \param[out] schemaLocation the XML Schema Definition (XSD) file name
    virtual bool getMeshLinkAttributes(std::string &xmlns, std::string &xmlns_xsi,
            std::string &schemaLocation) { return false; }

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
