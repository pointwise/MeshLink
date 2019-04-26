#ifndef MESH_ASSOCIATIVITY_CLASS
#define MESH_ASSOCIATIVITY_CLASS

/*! \file MeshAssociativity.h
  \brief Public MeshLink functions.

  The public interface to MeshLink.
*/

/*! \mainpage MeshLink Computational Geometry Access Documentation
 *
 * \section intro_sec Introduction
 *
 * The MeshLink library contains functions for import of
 * geometry-mesh association data, import of computational geometry
 * and querying, point projection, and evaluation of computational geometry.
 *
 * \section public_api_sec Public API
 *
 * All \ref public_types are declared in Types.h.<BR>
 * All \ref public_functions are declared in the meshlink/src/\ref h directory.<BR>
 *
 */


#include "Types.h"
#include "GeometryGroup.h"
#include "GeometryKernel.h"
#include "MeshModel.h"



#include <stdio.h>
#include <string>
#include <vector>

class GeometryKernelManager;
class MeshAssociativity;


 /****************************************************************************
 * MeshLinkAttribute class
 ***************************************************************************/
 /**
 * \class MeshLinkAttribute
 *
 * \brief Storage for MeshLink Attribute and AttributeGroup data.
 *
 * MeshLink entities reference attributes through an AREF attribute
 * which is the integer ID of the %MeshLinkAttribute associated with
 * the entity. A %MeshLinkAttribute can be a group of other %MeshLinkAttributes.
 *
 */
class ML_STORAGE_CLASS MeshLinkAttribute {
public:
    friend class MeshAssociativity;
    MeshLinkAttribute():
        attid_(-1),
        is_group_(false),
        is_valid_(true)
    {}

    MeshLinkAttribute(MLINT attid, std::string &name, std::string &contents, bool is_group, MeshAssociativity &meshAssoc) :
        attid_(attid),
        name_(name),
        contents_(contents),
        is_group_(is_group)
    {
        if (is_group) {
            is_valid_ = buildGroupArefs(meshAssoc);
        }
        else {
            is_valid_ = true;
        }
    }

    /// Whether the %MeshLinkAttribute is a group of other the %MeshLinkAttributes
    bool isGroup() const { return is_group_; }

    /// Whether the %MeshLinkAttribute is valid
    bool isValid() const { return is_valid_; }

    /// \brief Get the %MeshLinkAttribute AttIDs referenced by the %MeshLinkAttribute
    ///
    /// If the %MeshLinkAttribute is a group, the AttIDs of the group members are returned,
    /// otherwise, this %MeshLinkAttribute's AttID is returned
    const std::vector<MLINT> & getAttributeIDs() const {
        return group_arefs_;
    }
private:

    /**
     * \brief Build list of MeshLinkAttribute IDs referenced by the %GeometryGroup
     *
     * For an attribute group, recursively dereference attributes to build a 
     * complete and unique list of all attributes referenced by the group.
     */
    bool buildGroupArefs(MeshAssociativity &meshAssoc);

    MLINT attid_;
    std::string name_;
    std::string contents_;
    bool is_group_;
    bool is_valid_;
    std::vector<MLINT> group_arefs_;
};


/****************************************************************************
* MeshLinkFile class
***************************************************************************/
/**
* \class MeshLinkFile
*
* \brief Storage for MeshLink %GeometryFile and %MeshFile data.
*
*/
class ML_STORAGE_CLASS MeshLinkFile {
public:
    friend class MeshAssociativity;
    MeshLinkFile() {};
    MeshLinkFile(std::string &filename, std::string &arefString):
        filename_(filename),
        arefString_(arefString)
    {};
    ~MeshLinkFile() {};

    /// Return the filename specified by the %MeshLinkFile
    virtual const char *getFilename() const { return filename_.c_str(); }

    /// Return list of attribute IDs referenced by the %MeshLinkFile
    std::vector<MLINT> getAttributeIDs(const MeshAssociativity &meshAssoc) const;

protected:
    bool hasAref() const { return !arefString_.empty(); }
    bool getArefID(MLINT *aref) const {
        if (!hasAref()) { return false; }
        if (1 == sscanf(arefString_.c_str(), "%" MLINT_FORMAT, aref)) {
            return true;
        }
        return false;
    }

    std::string filename_;
    std::string arefString_;
};

/****************************************************************************
* GeometryFile class
***************************************************************************/
/**
* \class GeometryFile
*
* \brief Storage for MeshLink schema %GeometryFile data.
*
*/
class ML_STORAGE_CLASS GeometryFile : public MeshLinkFile {
public:
    GeometryFile() {};
    GeometryFile(std::string &filename, std::string &arefString) :
        MeshLinkFile(filename, arefString)
    {};
    ~GeometryFile() {};
private:
};

/****************************************************************************
* MeshFile class
***************************************************************************/
/**
* \class MeshFile
*
* \brief Storage for MeshLink schema %MeshFile data.
*
*/
class ML_STORAGE_CLASS MeshFile : public MeshLinkFile {
public:
    MeshFile() {};
    MeshFile(std::string &filename, std::string &arefString) :
        MeshLinkFile(filename, arefString)
    {};
    ~MeshFile() {};
private:
};


typedef std::map<std::string, MLINT> MeshAttributeNameToIDMap;
typedef std::map<MLINT, MeshLinkAttribute> MeshAttributeIDMap;


/****************************************************************************
* MeshAssociativity class
***************************************************************************/
/**
* \class MeshAssociativity
*
* \brief Storage for mesh topology and geometry associativity data.
*
*  This is the main interface for defining, querying and updating mesh-geometry associativity.
*
*/
class ML_STORAGE_CLASS MeshAssociativity {
public:
    MeshAssociativity() {}

    // Destructor
    ~MeshAssociativity();

    // Add a GeometryGroup to the database
    bool addGeometryGroup(GeometryGroup &geometry_group);

    /// Get GeometryGroup by name - returns NULL if not found
    GeometryGroup * getGeometryGroupByName(const char *name);

    /// Get GeometryGroup by ID - returns NULL if not found
    GeometryGroup * getGeometryGroupByID(MLINT id);

    /// Add a MeshModel to the database
    bool addMeshModel(MeshModel *model, bool mapID);

    /// Get MeshModel by ID - returns NULL if not found
    MeshModel * getMeshModelByID(MLINT id) const;

    /// Get MeshModel by name - returns NULL if not found
    MeshModel * getMeshModelByName(const std::string &name) const;

    /// Get MeshModel by reference - returns NULL if not found
    MeshModel* getMeshModelByRef(const char * ref) const;

    /// Add a GeometryKernel to the database
    void addGeometryKernel(GeometryKernel *kernel);

    /// Get GeometryKernel by name - returns NULL if not found
    GeometryKernel * getGeometryKernelByName(const char *name);

    /// Get the active GeometryKernel - returns NULL if none active
    GeometryKernel * getActiveGeometryKernel();

    /// Set the active GeometryKernel by name
    bool setActiveGeometryKernelByName(const char *name);

    /// Get MeshLinkAttribute by ID - returns NULL if not found
    const MeshLinkAttribute * getAttributeByID(const MLINT id) const;

    /// Remove all MeshLinkAttributes from the database
    void clearAttributes();

    /// Add a MeshLinkAttribute to the database
    bool addAttribute(MeshLinkAttribute &att);

    /// Return the number of GeometryFiles in the database
    MLINT getNumGeometryFiles() const;

    /// Add a GeometryFile to the database
    void addGeometryFile(GeometryFile &file);

    /// Return the number of MeshFiles in the database
    MLINT getNumMeshFiles() const;

    /// Add a MeshFile to the database
    void addMeshFile(MeshFile &file);

    /// Get the name and value of a MeshLinkAttribute by ID
    bool getAttribute(MLINT attID, const char **name, const char **value) const;

    /// Get a list of all GeometryFiles int the database
    const std::vector<GeometryFile> & getGeometryFiles() const;

    /// Get a list of all MeshFiles int the database
    const std::vector<MeshFile> & getMeshFiles() const;

    /// \brief Return list of MeshModels in the MeshAssociativity database
    std::vector<MeshModel *> getMeshModels() const;

private:

    // Geometry files
    std::vector<GeometryFile> geometryFiles_;
    std::vector<MeshFile> meshFiles_;

    // The geometry kernel manager
    static GeometryKernelManager geometry_kernel_manager_;

    // The associativity geometry group manager
    GeometryGroupManager geometry_group_manager_;

    // Attribute elements
    MeshAttributeIDMap meshAttributeIDMap_;
    MeshAttributeNameToIDMap meshAttributeNameToIDMap_;

    // Mesh Models
    MeshModelNameMap   meshModelNameMap_;
    MeshTopoIDToNameMap   meshModelIDToNameMap_;
    MeshTopoRefToNameMap    meshModelRefToNameMap_;

};





/****************************************************************************
* GeometryKernelManager class
***************************************************************************/
/**
* \class GeometryKernelManager
*
* \brief Management of geometry kernel interfaces
*
*  Storage for geometry kernel interfaces and the current active kernel.
*
*/
class GeometryKernelManager {
public:
    friend class MeshAssociativity;
    GeometryKernelManager() :
        active_(NULL)
    {};

private:
    void addKernel(GeometryKernel *kernel);

    GeometryKernel * getByName(const char *name);

    GeometryKernel * getActive();

    bool setActiveByName(const char *name);

    std::vector<GeometryKernel *> geometry_kernels_;
    GeometryKernel * active_;
};

#endif
