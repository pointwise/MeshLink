/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
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
 app/harness_cpp/%main.cxx.<BR>
 &nbsp;&nbsp;&nbsp;&nbsp; A C test harness and programming reference is available in 
 app/harness_c/%main.c.<BR>
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
 * MeshLinkTransform class
 ***************************************************************************/
 /**
  * \class MeshLinkTransform
  *
  * \brief Storage for MeshLink Transform data.
  *
  * MeshElementLinkages reference a transform through an XREF attribute
  * which is the integer ID of the %MeshLinkTransform associated with
  * the entity. 
  *
  * MeshLink transforms define an affine transform from one position to another.
  * Access to transforms by an application are through MeshElementLinkage.getTransform 
  * and MeshAssociativity.getTransformByID.
  *
  */
class ML_STORAGE_CLASS MeshLinkTransform {
public:
    friend class MeshAssociativity;

    /// Default constructor
    MeshLinkTransform();

    /// Constructor for an transform with a unique transform ID (AttID) and name.
    /// The transform's definition is given by its contents.
    MeshLinkTransform(MLINT xid, std::string &name, std::string &contents,
        MeshAssociativity &meshAssoc);

    /// \brief Set the MeshLinkAttribute AttID referenced by this MeshLinkTransform
    ///
    /// \param aref the attribute reference ID (AttID) for this transform
    virtual void setAref(MLINT aref);

    /// \brief Whether the MeshLinkTransform has a MeshLinkAttribute AttID reference defined
    bool hasAref() const;

    /// \brief Get the MeshLinkAttribute AttID referenced by this MeshLinkTransform
    MLINT getAref() const;

    /// \brief Get the XID of this MeshLinkTransform
    MLINT getXID() const;

    /// \brief Get the name attribute of this MeshLinkTransform
    const std::string & getName() const;

    /// \brief Get the contents of this MeshLinkTransform
    const std::string & getContents() const;

    /// Whether this transform is valid. A transform may be invalid if it's content
    /// does not define a valid transformation.
    bool isValid() const { return is_valid_; }

    /// \brief Get the transform quaternion
    ///
    /// \param[out] 4x4 matrix of the transform quaternion
    void getQuaternion(MLREAL xform_quaternion[4][4]) const;

private:
    /// The XID of this transform
    MLINT xid_;
    /// The name of this transform
    std::string name_;
    /// The attribute reference ID (AttID)
    MLINT aref_;
    /// The application-defined contents of the transform, from the MeshLink file.
    std::string contents_;
    /// The quaternion transform
    MLREAL xform_[4][4];
    /// Whether this transform is valid.
    bool is_valid_;
};


/****************************************************************************
 * MeshElementLinkage class
 ***************************************************************************/
 /**
  * \class MeshElementLinkage
  *
  * \brief Storage for MeshLink ElementLinkage data.
  *
  * A MeshElementLinkage provides a mapping of one referenced mesh element
  * (sheet, face, string, edge or vertex) to another, with an optional
  * transform (to identify slaved periodic transformations, for example).
  *
  * sourceEntityRef and targetEntityRef attributes are defined in an
  * application-specific manner; for example, mesh element reference IDs or
  * mesh element names.
  *
  */
class ML_STORAGE_CLASS MeshElementLinkage {
public:
    friend class MeshAssociativity;

    /// Default constructor
    MeshElementLinkage();

    /// Constructor for an transform with a unique transform ID (AttID) and name.
    /// The transform's definition is given by its contents.
    MeshElementLinkage(std::string &name, 
        std::string &sourceEntityRef,
        std::string &targetEntityRef,
        MeshAssociativity &meshAssoc);

    /// \brief Set the MeshLinkAttribute AttID referenced by this MeshElementLinkage
    ///
    /// \param aref the attribute reference ID (AttID) for this linkage
    virtual void setAref(MLINT aref);

    /// \brief Whether the MeshElementLinkage has a MeshLinkAttribute AttID reference defined
    bool hasAref() const;

    /// \brief Get the MeshLinkAttribute AttID referenced by this MeshElementLinkage
    MLINT getAref() const;

    /// \brief Set the Transform XID referenced by this MeshElementLinkage
    ///
    /// \param xref the transform reference ID (XID) for this linkage
    virtual bool setXref(MLINT xref, MeshAssociativity &meshAssoc);

    /// \brief Whether the MeshElementLinkage has a Transform XID reference defined
    bool hasXref() const;

    /// \brief Get the Transform XID referenced by this MeshElementLinkage
    ///
    /// \param[out] xref the transform reference ID (XID) for this linkage
    virtual bool getXref(MLINT *xref) const;

    /// \brief Get the Transform referenced by this MeshElementLinkage
    ///
    /// returns NULL if XREF is unset or invalid
    const MeshLinkTransform * getTransform(const MeshAssociativity &meshAssoc) const;

    /// Whether this linkage is valid. A linkage may be invalid if the sourceEntityRef
    /// or targetEntityRef are unknown.
    bool isValid() const { return is_valid_; }

    /// \brief Return the name of this MeshElementLinkage
    ///
    /// N.B. return value subject to change
    ///
    /// \param[out] name the non-modifiable name of the entity
    virtual void getName(const char **name) const;

    /// \brief Return the name of this MeshElementLinkage
    virtual const std::string & getName() const;

    /// \brief Set the name of this MeshElementLinkage
    ///
    /// setName is special and should not be overridden, as
    /// it provides a mechanism for generating unique names
    ///
    /// \param name the name of the mesh entity, or empty if a unique name is to be generated
    void setName(const std::string &name);

    /// \brief Set the name of this MeshElementLinkage
    ///
    /// setName is special and should not be overrided
    /// it provides a mechanism for generating unique names
    //
    /// \param name the name of the mesh entity, or null if a unique name is to be generated
    void setName(const char *name);

    /// Generate and return a unique name for the entity
    std::string getNextName();

    /// \brief Return the Entity references linked by this MeshElementLinkage
    ///
    /// \param[out] sourceEntityRef reference to the source entity
    /// \param[out] targetEntityRef reference to the target entity
    void getEntityRefs(std::string &sourceEntityRef, std::string &targetEntityRef) const;

    /// Return the base name used for generating unique names for linkages
    virtual const std::string &getBaseName() const;
    /// Return the current value used for generating unique names for linkages
    virtual MLUINT &getNameCounter();

private:
    /// The unique name counter for linkages
    static MLUINT nameCounter_;

    /// The name of this linkage
    std::string name_;
    /// The attribute reference ID (AttID)
    MLINT aref_;
    /// The transform reference ID (XID)
    MLINT xref_;
    /// The source entity reference.
    std::string sourceEntityRef_;
    /// The target entity reference.
    std::string targetEntityRef_;
    /// Whether this linkage is valid.
    bool is_valid_;
};

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
    MeshLinkAttribute();

    /// Constructor for an attribute with a unique attribute ID (AttID) and name.
    /// The attribute's definition is given by its contents.
    MeshLinkAttribute(MLINT attid, std::string &name, std::string &contents,
        bool is_group, MeshAssociativity &meshAssoc);

    /// \brief Get the AttID of this attribute.
    MLINT getAttID() const;

    /// Whether this attribute is a group of other MeshLinkAttribute s.
    bool isGroup() const;

    /// Whether this attribute is valid. An attribute may be invalid if it is a group
    /// of other attribute IDs, and any one of which do not exist.
    bool isValid() const;

    /// \brief Get the AttIDs referenced by this attribute.
    ///
    /// If the %MeshLinkAttribute is a group, the AttIDs of the group
    /// members are returned, otherwise, this attribute's AttID is returned
    const std::vector<MLINT> & getAttributeIDs() const;
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

typedef std::map<std::string, MLINT> MeshTransformNameToIDMap;
typedef std::map<MLINT, MeshLinkTransform> MeshTransformIDMap;
typedef std::map<std::string, MeshElementLinkage *> MeshElementLinkageNameMap;


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

    /// \brief Get count of GeometryGroups in the database
    ///
    /// \return GeometryGroup count
    MLINT getGeometryGroupCount() const;

    /// \brief Get list of GeometryGroup IDs in the database
    ///
    /// \param[in,out] gids array of GIDs
    void getGeometryGroupIDs(std::vector<MLINT> &gids) const;

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

    /// \brief Get MeshSheet by name
    ///
    /// Search all MeshModels for a MeshSheet with the given name.
    ///
    /// \param[in] name the name of the desired sheet
    /// \param[out] model containing the desired sheet
    /// \param[out] the desired sheet
    /// \return true if found
    bool getMeshSheetByName(const std::string &name,
        MeshModel **model, MeshSheet **sheet) const;

    /// \brief Get MeshString by name
    ///
    /// Search all MeshModels for a MeshString with the given name.
    ///
    /// \param[in] name the name of the desired string
    /// \param[out] model containing the desired string
    /// \param[out] the desired string
    /// \return true if found
    bool getMeshStringByName(const std::string &name,
        MeshModel **model, MeshString **string) const;

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

    /// \brief Return list of MeshLinkAttributes in the MeshAssociativity database
    std::vector<const MeshLinkAttribute *> getAttributes() const;


    /// \brief Get MeshLinkTransform by ID
    ///
    /// \param[in] id the unique ID of the desired transform
    /// \return transform or NULL if not found
    const MeshLinkTransform * getTransformByID(const MLINT id) const;

    /// \brief Remove all MeshLinkTransform objects from the database
    void clearTransforms();

    /// \brief Add a MeshLinkTransform to the database
    ///
    /// \param[in] trans the transform to add
    /// \return true if the transform was added
    bool addTransform(MeshLinkTransform &att);

    /// \brief Return list of MeshLinkTransforms in the MeshAssociativity database
    void getTransforms(std::vector<const MeshLinkTransform *> &xforms) const;

    /// \brief Return count of MeshLinkTransforms in the MeshAssociativity database
    size_t  getTransformCount() const;


    /// \brief Remove all MeshElementLinkage objects from the database
    void clearMeshElementLinkages();

    /// \brief Add a MeshElementLinkage to the database
    ///
    /// \param[in] trans the transform to add
    /// \return true if the transform was added
    bool addMeshElementLinkage(MeshElementLinkage *linkage);


    /// \brief Get MeshElementLinkage by name
    ///
    /// \param[in] name the name of the desired linkage
    /// \return linkage or NULL if not found
    MeshElementLinkage*
        getMeshElementLinkageByName(const std::string &name) const;


    /// \brief Return list of MeshElementLinkages in the MeshAssociativity database
    void getMeshElementLinkages(std::vector<MeshElementLinkage *> &linkages) const;

    /// \brief Return count of MeshElementLinkages in the MeshAssociativity database
    size_t  getMeshElementLinkageCount() const;

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
    void getMeshModels(std::vector<MeshModel *> &models) const;

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

    /// Map of MeshTransform to associated XID (xref)
    MeshTransformIDMap meshTransformIDMap_;
    /// Map of MeshTransform name to associated XID (xref)
    MeshTransformNameToIDMap meshTransformNameToIDMap_;

    /// Map of element linkage name to MeshElementLinkage
    MeshElementLinkageNameMap   meshElementLinkageNameMap_;


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
 * This file is licensed under the Cadence Public License Version 1.0 (the
 * "License"), a copy of which is found in the included file named "LICENSE",
 * and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
 * LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
 * ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
 * Please see the License for the full text of applicable terms.
 *
 ****************************************************************************/
