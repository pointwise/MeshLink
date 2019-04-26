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
* Provides a neutral interface for parsing MeshLink data
* using a wrapped parsing library.
*
*/
class ML_STORAGE_CLASS MeshLinkParser {
public:
    MeshLinkParser() {}
    ~MeshLinkParser() {};

    // Validate MeshLink xml file against MeshLink schema
    virtual bool validate(const std::string &fileName, 
	    const std::string &schemaName) { return false; }

    // Parse MeshLink xml file into MeshAssociativity 
    virtual bool parseMeshLinkFile(std::string fname, 
	    MeshAssociativity *meshAssociativity) { return false; }
private:
};

#endif
