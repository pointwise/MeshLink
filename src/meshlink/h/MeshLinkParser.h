/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
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
 * This file is licensed under the Cadence Public License Version 1.0 (the
 * "License"), a copy of which is found in the included file named "LICENSE",
 * and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
 * LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
 * ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
 * Please see the License for the full text of applicable terms.
 *
 ****************************************************************************/
