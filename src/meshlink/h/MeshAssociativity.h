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

#ifndef MESH_ASSOCIATIVITY_CLASS
#define MESH_ASSOCIATIVITY_CLASS

/*! \file MeshAssociativity.h
  \brief Public MeshLink functions.

  The public interface to MeshLink.
*/

/*! \mainpage MeshLink Computational Geometry Access Documentation
 
 \section intro_sec Introduction
 
 The MeshLink library contains functions for import of
 geometry-mesh association data, import of computational geometry
 and querying, point projection, and evaluation of computational geometry.

 This document is the programming reference, for more information vist https://www.pointwise.com/meshlink/.

 Generated on <I>\today</I>.
 
 \section public_api_sec Public API
 &nbsp;&nbsp;&nbsp;&nbsp; All public function header files are located in the meshlink/src/h
  directory.<BR>
 
 \subsection public_types_sec Public Intrinsic Data Types
 &nbsp;&nbsp;&nbsp;&nbsp; All public types are declared in Types.h.<BR>
 
 \subsection public_cpp_sec Public C++ Classes
 MeshAssociativity - Container for all mesh-geometry associativity information.<BR>
 GeometryGroup - Container for geometry association by GID to geometry kernel entities.<BR>
 GeometryKernel - Base class for geometry kernel interface.<BR>
 MeshLinkParser - Base class for MeshLink data parser interface.<BR>
 MeshLinkWriter - Base class for MeshLink data writer interface.<BR>

 <B>Mesh Topology</B><BR>
 &nbsp;&nbsp;&nbsp;&nbsp; Mesh topology refers to 1D, 2D, and 3D constructs within the mesh heirarchy.<BR>
 &nbsp;&nbsp;&nbsp;&nbsp; MeshTopo: Base class for mesh topology and entity classes.<BR>
 &nbsp;&nbsp;&nbsp;&nbsp; MeshString: A 1D collection of mesh edges. A Mesh String may reference one 
  or more geometry curves.<BR>
 &nbsp;&nbsp;&nbsp;&nbsp; MeshSheet: A 2D collection of mesh faces. A Mesh Sheet may reference one 
  or more geometry surfaces.<BR>
 &nbsp;&nbsp;&nbsp;&nbsp; MeshModel: A 3D mesh volume. A Mesh Model is a container for Mesh String 
  and Mesh Sheet elements used in the definition of the volume mesh.<BR>
 
<B>Mesh Entity</B><BR>
&nbsp;&nbsp;&nbsp;&nbsp; Mesh entity refers to 0D, 1D, and 2D constructs within the mesh connectivity.<BR>
&nbsp;&nbsp;&nbsp;&nbsp; MeshPoint: A mesh point.<BR>
&nbsp;&nbsp;&nbsp;&nbsp; ParamVertex: Parametric geometry information associated with a %MeshPoint.<BR>
&nbsp;&nbsp;&nbsp;&nbsp; MeshEdge: A 1D mesh element. Typically referencing 2 MeshPoints.<BR>
&nbsp;&nbsp;&nbsp;&nbsp; MeshFace: A 2D mesh element. Typically referencing 3 or 4 MeshPoints.<BR>

<P>
\subsection public_c_sec Public C Functions
&nbsp;&nbsp;&nbsp;&nbsp; All public C functions are declared in MeshAssociativity_c.h.<BR>

\section test_api_sec API Test Harness
 &nbsp;&nbsp;&nbsp;&nbsp; A C++ test harness and programming reference is available in 
 app/harness_cpp/main.cxx.<BR>
 &nbsp;&nbsp;&nbsp;&nbsp; A C test harness and programming reference is available in 
 app/harness_c/main.c.<BR>
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
 * MeshLink attributes are for use by applications, and have no expressed
 * nor implied usage within MeshLink. Access to attributes by an application
 * are through MeshAssociativity.getAttribute and MeshAssociativity.getAttributeByID.
 *
 */
class ML_STORAGE_CLASS MeshLinkAttribute {
public:
    friend class MeshAssociativity;

    /// Default constructor
    MeshLinkAttribute():
        attid_(-1),
        is_group_(false),
        is_valid_(true)
    {}

    /// Constructor for an attribute with a unique attribute ID (AttID) and name.
    /// The attribute's definition is given by its contents.
    MeshLinkAttribute(MLINT attid, std::string &name, std::string &contents,
        bool is_group, MeshAssociativity &meshAssoc) :
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

    /// Whether this attribute is a group of other MeshLinkAttribute s.
    bool isGroup() const { return is_group_; }

    /// Whether this attribute is valid. An attribute may be invalid if it is a group
    /// of other attribute IDs, and any one of which do not exist.
    bool isValid() const { return is_valid_; }

    /// \brief Get the AttIDs referenced by this attribute.
    ///
    /// If the %MeshLinkAttribute is a group, the AttIDs of the group
    /// members are returned, otherwise, this attribute's AttID is returned
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

    /// The AttID of this attribute
    MLINT attid_;
    /// The name of this attribute
    std::string name_;
    /// The application-defined contents of the attribute, from the MeshLink file.
    std::string contents_;
    /// Whether this attribute is a group of other attributes.
    bool is_group_;
    /// Whether this attribute is valid.
    bool is_valid_;
    /// The list of other attribute AttIDs to which this attribute refers
    /// (for attribute groups).
    std::vector<MLINT> group_arefs_;
};


/****************************************************************************
 * MeshLinkFile class
 ***************************************************************************/
/**
 * \class MeshLinkFile
 *
 * \brief Storage for MeshLink GeometryFile and MeshFile data.
 *
 */
class ML_STORAGE_CLASS MeshLinkFile {
public:
    friend class MeshAssociativity;

    /// Default constructor
    MeshLinkFile() {};

    /// Explicit constructor with file name (path) and attribute reference string.
    MeshLinkFile(std::string &filename, std::string &arefString):
        filename_(filename),
        arefString_(arefString)
    {};

    /// Destructor
    ~MeshLinkFile() {};

    /// Return the filename specified by the %MeshLinkFile
    virtual const char *getFilename() const { return filename_.c_str(); }

    /// Return list of attribute IDs referenced by the %MeshLinkFile
    std::vector<MLINT> getAttributeIDs(const MeshAssociativity &meshAssoc) const;

    /// Return whether the file has an attribute reference.
    bool hasAref() const { return !arefString_.empty(); }

    /// Return whether the file references the given attribute reference ID.
    bool getArefID(MLINT *aref) const {
        if (!hasAref()) { return false; }
        if (1 == sscanf(arefString_.c_str(), "%" MLINT_FORMAT, aref)) {
            return true;
        }
        return false;
    }

protected:
    /// The mesh file name.
    std::string filename_;

    /// The mesh file attribute reference string.
    std::string arefString_;
};

/****************************************************************************
* GeometryFile class
***************************************************************************/
/**
 * \class GeometryFile
 *
 * \brief Storage for MeshLink schema GeometryFile data.
 *
 * GeometryFile objects are accessed by an application using
 * MeshAssociativity.getGeometryFiles.  A GeometryFile is used
 * only to link the kernel implementation to attributes specified
 * by a MeshLink file.
 *
 * Geometry data itself is managed by an application's
 * custom implemenation of GeometryKernel.
 *
 */
class ML_STORAGE_CLASS GeometryFile : public MeshLinkFile {
public:
    /// \brief Default constructor
    GeometryFile() {};

    /// \brief Explicit constructor with file name (path) of geometry file
    /// and the attribute reference string.
    GeometryFile(std::string &filename, std::string &arefString) :
        MeshLinkFile(filename, arefString)
    {};

    /// \brief Destructor
    ~GeometryFile() {};

    /// \brief Add a geometry group ID to the geometry file. Typically used by
    /// the %MeshLinkParser when a new "GeometryGroup" element is encountered.
    void addGeometryGroupID(MLINT gid) {
        groupIDs_.push_back(gid);
    }

    /// \brief Return a list of all geometry group IDs associated with this
    /// %GeometryFile.
    const std::vector<MLINT> &getGeometryGroupIDs() const
    {
        return groupIDs_;
    }
private:
    /// The geometry group IDs in this file
    std::vector<MLINT> groupIDs_;
};

/****************************************************************************
* MeshFile class
***************************************************************************/
/**
 * \class MeshFile
 *
 * \brief Storage for MeshLink schema %MeshFile data.
 *
 * MeshFile objects are accessed by an application using
 * MeshAssociativity.getMeshFiles.  A MeshFile is used
 * only to link application-defined mesh attributes specified
 * by a MeshLink file.
 *
 * Mesh data itself is managed by a MeshModel and its associated
 * MeshTopo derived classes (MeshSheet, MeshString, MeshFace, MeshEdge, and
 * MeshPoint) and ParamVertex.
 *
 */
class ML_STORAGE_CLASS MeshFile : public MeshLinkFile {
public:
    /// \brief Default constructor
    MeshFile() {};

    /// \brief Explicit constructor with mesh file name and attribute reference string
    MeshFile(std::string &filename, std::string &arefString) :
        MeshLinkFile(filename, arefString)
    {};

    /// \brief Destructor
    ~MeshFile() {};

    /// \brief Add a MeshModel reference to this mesh file. Typically used by
    /// a MeshLinkParser when a new "MeshModelReference" element is encountered.
    ///
    /// \param refName the model reference name to add
    void addModelRef(const std::string &refName) {
        modelRefs_.push_back(refName);
    }

    // \brief Return the list of mesh model references associated with this mesh file
    const std::vector<std::string> &getModelRefs() {
        return modelRefs_;
    }

private:
    /// The model references associated with a mesh file
    std::vector<std::string> modelRefs_;
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
 *  This is the main interface for defining, querying and updating
 *  mesh-geometry associativity.
 *
 */
class ML_STORAGE_CLASS MeshAssociativity {
public:
    /// \brief Default constructor
    MeshAssociativity() {}

    /// \brief Destructor
    ~MeshAssociativity();

    /// \brief Add a GeometryGroup to the database
    ///
    /// \param[in] geometry_group the group to add
    /// \return true if group was added
    bool addGeometryGroup(GeometryGroup &geometry_group);

    /// \brief Get GeometryGroup by name
    ///
    /// \param[in] name the name of the desired geometry group
    /// \return group or NULL if not found
    GeometryGroup * getGeometryGroupByName(const char *name);

    /// \brief Get GeometryGroup by ID
    ///
    /// \param[in] id the ID of the desired geometry group
    /// \return group or NULL if not found
    GeometryGroup * getGeometryGroupByID(MLINT id);

    /// \brief Add a MeshModel to the database
    ///
    /// \param[in] model the model to add
    /// \param[in] mapID whether to map the unique ID to the model name
    bool addMeshModel(MeshModel *model, bool mapID);

    /// \brief Get MeshModel by ID
    ///
    /// \param[in] id the ID of the desired model
    /// \return model or NULL if not found
    MeshModel * getMeshModelByID(MLINT id) const;

    /// \brief Get MeshModel by name
    ///
    /// \param[in] name the name of the desired model
    /// \return model or NULL if not found
    MeshModel * getMeshModelByName(const std::string &name) const;

    /// \brief Get MeshModel by reference
    ///
    /// \param[in] ref the application-defined reference string of the desired model
    /// \return model or NULL if not found
    MeshModel* getMeshModelByRef(const char * ref) const;

    /// \brief Add a GeometryKernel to the database
    ///
    /// Geometry kernels are application-defined. An application will typically  add
    /// a kernel to its MeshAssociativity and set it active after loading MeshLink
    /// data and before loading geometry.
    ///
    /// \param[in] kernel the geometry kernel to add
    void addGeometryKernel(GeometryKernel *kernel);

    /// \brief Remove a GeometryKernel from the database
    ///
    /// \param[in] kernel the geometry kernel to remove
    void removeGeometryKernel(GeometryKernel *kernel);

    /// \brief Get GeometryKernel by name
    ///
    /// \param[in] name the name of the desired geometry kernel
    /// \return geometry kernel or NULL if not found
    GeometryKernel * getGeometryKernelByName(const char *name);

    /// \brief Get the active GeometryKernel
    ///
    /// \return geometry kernel or NULL if none active
    GeometryKernel * getActiveGeometryKernel();

    /// \brief Set the active GeometryKernel by name
    ///
    /// The active geometry kernel is set by the application before performing
    /// geometric evaluations and projections.
    ///
    /// \param[in] name the name of the desired geometry kernel
    /// \return true if geometry kernel found and set active
    bool setActiveGeometryKernelByName(const char *name);

    /// \brief Get MeshLinkAttribute by ID
    ///
    /// \param[in] id the unique ID of the desired attribute
    /// \return attribute or NULL if not found
    const MeshLinkAttribute * getAttributeByID(const MLINT id) const;

    /// \brief Remove all MeshLinkAttribute objects from the database
    void clearAttributes();

    /// \brief Add a MeshLinkAttribute to the database
    ///
    /// \param[in] att the attribute to add
    /// \return true if the attribute was added
    bool addAttribute(MeshLinkAttribute &att);

    /// \brief Return the number of GeometryFile objects in the database
    MLINT getNumGeometryFiles() const;

    /// \brief Add a GeometryFile to the database
    ///
    /// \param[in] file the geometry file to add
    void addGeometryFile(GeometryFile &file);

    /// \brief Return the number of MeshFile objects in the database
    MLINT getNumMeshFiles() const;

    /// \brief Add a MeshFile to the database
    ///
    /// \param[in] file the mesh file to add
    void addMeshFile(MeshFile &file);

    /// \brief Get the name and value of a MeshLinkAttribute by ID
    ///
    /// \param[in] attID the unique ID of the desired attribute
    /// \param[out] name the name of the attribute, if found
    /// \param[out] value the value (content) of the attribute, if found
    /// \return true if attribute was found
    bool getAttribute(MLINT attID, const char **name, const char **value) const;

    /// \brief Get a list of all GeometryFile objects in the database
    const std::vector<GeometryFile> & getGeometryFiles() const;

    /// \brief Get a list of all MeshFile objects in the database
    const std::vector<MeshFile> & getMeshFiles() const;

    /// \brief Return list of MeshModels in the MeshAssociativity database
    std::vector<MeshModel *> getMeshModels() const;

    /// \brief Return count of MeshModels in the MeshModel
    size_t getMeshModelCount() const;

private:
    /// Geometry files
    std::vector<GeometryFile> geometryFiles_;
    /// Mesh files
    std::vector<MeshFile> meshFiles_;

    /// The Singleton geometry kernel manager
    static GeometryKernelManager geometry_kernel_manager_;

    /// The associativity geometry group manager
    GeometryGroupManager geometry_group_manager_;

    /// Map of MeshAttribute to associated ID (aref)
    MeshAttributeIDMap meshAttributeIDMap_;
    /// Map of MeshAttribute name to associated ID (aref)
    MeshAttributeNameToIDMap meshAttributeNameToIDMap_;

    /// Map of mesh model name to MeshModel
    MeshModelNameMap   meshModelNameMap_;
    /// Map of mesh model ID to MeshModel name
    MeshTopoIDToNameMap   meshModelIDToNameMap_;
    /// Map of mesh model reference string to MeshModel name
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
 *  Only one GeometryKernel may be active at a time.
 *
 *  This singleton object is used internally by the MeshAssociativity.
 *
 */
using GeometryKernelArray = std::vector<GeometryKernel *>;

class GeometryKernelManager {
public:
    friend class MeshAssociativity;
    /// Default constructor, no active GeometryKernel by default
    GeometryKernelManager() :
        active_(NULL)
    {};

private:
    /// Manage a new geometry kernel.
    ///
    /// \param[in] kernel geometry kernel to add
    void addKernel(GeometryKernel *kernel);

    /// Unmanage a geometry kernel.
    ///
    /// \param[in] kernel geometry kernel to remove
    void removeKernel(GeometryKernel *kernel);

    /// Return the GeometryKernel associated with the given name.
    ///
    /// \param[in] name the name of the desired kernel
    /// \return geometry kernel or NULL if not found
    GeometryKernel * getByName(const char *name);

    /// Return the active GeometryKernel.
    ///
    /// \return geometry kernel or NULL if none active
    GeometryKernel * getActive();

    /// Set the active GeometryKernel by name.
    ///
    /// \param[in] name the name of the desired kernel
    /// \return true if kernel found and set active
    bool setActiveByName(const char *name);

    /// The list of managed GeometryKernel objects
    GeometryKernelArray geometry_kernels_;
    /// The active GeometryKernel
    GeometryKernel * active_;
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
