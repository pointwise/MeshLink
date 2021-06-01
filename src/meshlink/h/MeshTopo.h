/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#ifndef MESH_TOPO_CLASS
#define MESH_TOPO_CLASS

#include "Types.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#define MESH_TOPO_INVALID_REF -101
#define MESH_TOPO_INDEX_UNUSED -101

/****************************************************************************
 *
 * pwiFnvHash class
 *
 * A simple hashing routine used to uniquely identify mesh topology entities.
 * Fowler-Noll-Vo Hash Function
 * Designed to be fast with decent dispersion.
 * https://en.wikipedia.org/wiki/Fowler%E2%80%93Noll%E2%80%93Vo_hash_function
 *
 ***************************************************************************/
class pwiFnvHash {
public:
    //! Hash type
    typedef MLUINT FNVHash;

    static FNVHash hashInit();

    //! Add an integer to the developing hash
    static FNVHash hash(MLINT data, FNVHash hash);

private:
    const static FNVHash   fnv_init_;
    const static FNVHash   fnv_prime_;
    const static FNVHash   fnv_num_octets_;
};


#define FNV1_32_INIT ((pwiFnvHash::FNVHash)0x811c9dc5)
#define FNV1_64_INIT ((pwiFnvHash::FNVHash)0xcbf29ce484222325ULL)
#define FNV1_32_PRIME ((pwiFnvHash::FNVHash)0x01000193)
#define FNV1_64_PRIME ((pwiFnvHash::FNVHash)0x100000001b3ULL)


/****************************************************************************
 * ParamVertex class
 ***************************************************************************/
/**
 * \class ParamVertex
 *
 * \brief Parametric geometry data
 *
 * Provides access to MeshLink schema parametric vertex data
 *
 */
class ParamVertex {
public:
    /// Default constructor
    ParamVertex() :
        gref_(MESH_TOPO_INVALID_REF),
        mid_(MESH_TOPO_INVALID_REF),
        u_(0.0),
        v_(0.0) {}

    /// Constructor with parametric data
    ///
    /// \param vref the application-define vertex reference string (required non-empty)
    /// \param gref the geometry reference ID (required non-zero)
    /// \param mid the unique ID (optional)
    /// \param u the parametric U value in the space of the referenced geometry
    /// \param v the parametric V value in the space of the referenced geometry
    ParamVertex(const std::string & vref, MLINT gref, MLINT mid, MLREAL u, MLREAL v) :
        vref_(vref),
        gref_(gref),
        mid_(mid),
        u_(u),
        v_(v) {}

    /// \brief Return the MeshPoint vertex reference
    const std::string &getVref() const { return vref_; }
    /// \brief Return the associated geometry GID
    MLINT getGref() const { return gref_; }
    /// \brief Return this entity's ID
    MLINT getID() const { return mid_; }
    /// \brief Return the associated geometry parametric coordinates
    /// \param[out] u the parametric U value
    /// \param[out] v the parametric V value
    void  getUV(MLREAL *u, MLREAL *v) const { *u = u_; *v = v_; }
private:
    /// The application-defined reference string
    std::string vref_;  // required attr
    /// The geometry reference ID
    MLINT gref_;  // required attr
    /// The unique ID
    MLINT mid_;   // optional attr
    /// The parametric U value
    MLREAL u_;
    /// The parametric V value
    MLREAL v_;
};

typedef std::map<std::string /*vref*/, ParamVertex *> ParamVertVrefMap;
typedef std::map<MLINT /*mid*/, std::string /*vref*/> ParamVertIDToVrefMap;

class MeshAssociativity;

/****************************************************************************
 * MeshTopo class
 ***************************************************************************/
/**
 * \class MeshTopo
 *
 * \brief Base class for mesh topology entities
 *
 * Common data for MeshString, MeshSheet, MeshModel topology and
 * MeshPoint, MeshEdge, MeshFace elements.
 *
 */
class ML_STORAGE_CLASS MeshTopo {
public:
    friend class MeshAssociativity;

    /// \brief Construct without application-defined reference
    ///
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    MeshTopo(
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name );

    /// \brief Construct with application-defined reference to entity in mesh data
    ///
    /// \param ref the application-defined reference of the mesh entity
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    MeshTopo(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name );

    /// \brief Return the ID of this MeshTopo
    virtual MLINT getID() const;

    /// \brief Return the GID of GeometryGroup associated with this MeshTopo
    virtual MLINT getGref() const;

    /// \brief Return the AttID of MeshLinkAttribute associated with this MeshTopo
    virtual MLINT getAref() const;

    /// \brief Return the name of this MeshTopo
    ///
    /// N.B. return value subject to change
    ///
    /// \param[out] name the non-modifiable name of the entity
    virtual void getName(const char **name) const;

    /// \brief Return the name of this MeshTopo
    virtual const std::string & getName() const;

    /// \brief Return the application-defined mesh data reference of this MeshTopo
    virtual const std::string & getRef() const;

    /// \brief Add a ParamVertex
    ///
    /// \param pv the vertex to add
    /// \param mapID whether to map the unique ID to the entity name
    virtual void addParamVertex(ParamVertex *pv, bool mapID);

    /// \brief Find a ParamVertex by vertex reference
    ///
    /// \param vref the application-defined reference string of the desired ParamVertex
    virtual ParamVertex * getParamVertByVref(const std::string & vref) const;

    /// \brief Find a ParamVertex by ID
    ///
    /// \param id the unique ID of the desired ParamVertex
    virtual ParamVertex * getParamVertByID(MLINT id) const;

    /// \brief Return the number of ParamVertex objects for this MeshTopo
    virtual MLINT getNumParamVerts() const;

    /// \brief Return vector of ParamVertex objects for this MeshTopo
    void getParamVerts(std::vector<const ParamVertex *> &pv) const;

    /// \brief Return the map of application-defined reference string to ParamVertex objects
    /// for this MeshTopo
    virtual const ParamVertVrefMap & getParamVertVrefMap() const;

    /// \brief Set the ID of this MeshTopo
    ///
    /// \param id the unique ID of this mesh entity
    virtual void setID( MLINT id );

    /// \brief Set the GeometryGroup GID referenced by this MeshTopo
    ///
    /// \param gref the geometry reference ID for this mesh entity
    virtual void setGref( MLINT gref );

    /// \brief Set the MeshLinkAttribute AttID referenced by this MeshTopo
    ///
    /// \param aref the attribute reference ID (AttID) for this mesh entity
    virtual void setAref( MLINT aref );

    /// \brief Set the reference of this MeshTopo
    //
    /// \param ref the application-defined reference string for this mesh entity
    virtual void setRef(const char *ref);

    /// \brief Set the name of this MeshTopo
    ///
    /// setName is special and should not be overridden, as
    /// it provides a mechanism for generating unique names
    ///
    /// \param name the name of the mesh entity, or empty if a unique name is to be generated
    void setName(const std::string &name);

    /// \brief Set the name of this MeshTopo
    ///
    /// setName is special and should not be overrided
    /// it provides a mechanism for generating unique names
    //
    /// \param name the name of the mesh entity, or null if a unique name is to be generated
    void setName(const char *name);

    /// \brief Whether the MeshTopo has an ID defined
    bool hasID() const;

    /// \brief Whether the MeshTopo has a GeometryGroup GID reference defined
    bool hasGref() const;

    /// \brief Whether the MeshTopo has a MeshLinkAttribute AttID reference defined
    bool hasAref() const;

    /// \brief Return list of MeshLinkAttribute AttIDs referenced by the MeshTopo
    std::vector<MLINT> getAttributeIDs(const MeshAssociativity &meshAssoc) const;

    /// Generate and return a unique name for the entity
    std::string getNextName();

    /// Returns the base name used for generating unique mesh entity names
    virtual const std::string &getBaseName() const;
    /// Returns the current value of the counter used for generating unique mesh entity names
    virtual MLUINT &getNameCounter();

    /// Set order counter (in parent topo)
    void setOrderCounter(MLUINT counter);

    /// Returns order counter (in parent topo)
    MLUINT getOrderCounter() const ;

    static bool OrderCompare(const MeshTopo *topo1, const MeshTopo *topo2);

    /// Default constructor
    MeshTopo();
    /// Destructor
    ~MeshTopo();
protected:
    /// The application-defined reference string
    std::string ref_;

    // MeshLink schema: MeshElementAttributes
    /// The unique entity ID
    MLINT mid_;
    /// The attribute reference ID (AttID)
    MLINT aref_;
    /// The geometry reference ID
    MLINT gref_;
    /// The name of the mesh entity
    std::string name_;
    /// creation order counter in parent entity
    MLUINT orderCounter_;

    /// ParamVertex objects mapped to this entity by application-defined reference string
    ParamVertVrefMap paramVertVrefMap_;
    /// ParamVertex objects mapped to this entity by unique entity ID
    ParamVertIDToVrefMap paramVertIDToVrefMap_;

private:
    /// The unique name counter
    static MLUINT nameCounter_;
};

typedef std::map<MLINT, std::string> MeshTopoIDToNameMap;

typedef std::vector<MeshTopo *> MeshTopoArray;

typedef std::map<std::string, std::string> MeshTopoRefToNameMap;



/****************************************************************************
 * MeshPoint class
 ***************************************************************************/
/**
 * \class MeshPoint
 *
 * \brief 0-D (point) mesh entity
 *
 * Provides access to MeshLink schema %MeshPoint data
 */

class MeshPoint : public MeshTopo {
public:
    friend class MeshAssociativity;
    friend class MeshModel;
    friend class MeshSheet;
    friend class MeshString;

    /// \brief Constructor without application-defined reference data
    ///
    /// \param i1 the index of the point
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1 (optional) the ParamVertex to associate with this point
    MeshPoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1
    );

    /// Construct with reference to Point entity in mesh data
    ///
    /// \param ref the application-defined reference of the mesh entity
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1 (optional) the ParamVertex to associate with this point
    MeshPoint(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1
    );

    /// \brief Return ParamVertex associated with the MeshPoint
    ParamVertex * const getParamVert() const {
        return paramVert_;
    }

    /// \brief Return array of ParamVertices associated with the MeshPoint
    ///
    /// For MeshPoint, there ever only one ParamVertex associated
    ///
    /// \param[out] pvs the C-style array of associated ParamVertex
    MLINT getParamVerts(ParamVertex * const ** pvs) const {
        *pvs = &(paramVert_);
        return 1;
    }

    /// \brief Destructor
    ~MeshPoint();

    /// Return the base name used for generating unique names for points
    virtual const std::string &getBaseName() const;
    /// Return the current value used for generating unique names for points
    virtual MLUINT &getNameCounter();

private:
    /// The unique name counter for points
    static MLUINT nameCounter_;

    /// Hidden default constructor
    MeshPoint():
        i1_(-1),
        paramVert_(NULL)
    {};

    /// Compute a point hash value for the given index value
    static pwiFnvHash::FNVHash computeHash(MLINT i1);

    /// Return the hash value for this point
    pwiFnvHash::FNVHash getHash() const;

    /// The index of this point
    MLINT i1_;
    /// The ParamVertex associated with this point
    ParamVertex * paramVert_;
};

typedef std::map<std::string, MeshPoint *> MeshPointNameMap;


/****************************************************************************
 * MeshEdge class
 ***************************************************************************/
/**
 * \class MeshEdge
 *
 * \brief 1-D (edge) mesh entity
 *
 * Provides access to MeshLink schema %MeshEdge data
 */
class MeshEdge : public MeshTopo {
public:
    friend class MeshAssociativity;
    friend class MeshModel;
    friend class MeshSheet;
    friend class MeshString;

    /// \brief Construct an edge with a start and end point index
    ///
    /// \param i1,i2 the indices of the points defining the edge
    /// \param mid unique ID of the edge
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the edge
    /// \param pv1,pv2 (optional) ParamVertex objects associated with the edge points
    MeshEdge(MLINT i1, MLINT i2,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2);

    /// \brief Construct an edge with application-defined reference data
    //
    /// \param ref the application-defined reference of the mesh edge
    /// \param mid unique ID of the mesh edge
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2 (optional) ParamVertex objects associated with the edge points
    MeshEdge(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2);

    /// \brief Destructor
    ~MeshEdge();

    /// \brief Copy constructor
    MeshEdge(const MeshEdge &other);

    /// \brief Copy operator
    MeshEdge & operator=(const MeshEdge &other);

    /// Return the base name used for generating unique names for edges
    virtual const std::string &getBaseName() const;
    /// Return the current value used for generating unique names for edges
    virtual MLUINT &getNameCounter();

    /// \brief Return array of vertex indices associated with the MeshEdge
    ///
    /// N.B. Assumes inds is array of size 2.
    ///
    /// \param[in,out] inds the array of unique point indices for the edge
    /// \param[out] numInds the number of points used in the edge
    void getInds(MLINT *inds, MLINT *numInds) const;

    /// \brief Return pointer to array of ParamVertices associated with the MeshEdge
    ///
    /// \param[out] pvs the array of ParamVertex for this edge
    /// \return number of ParamVertex in the array
    MLINT getParamVerts( ParamVertex * const ** pvs) const {
        *pvs = &(paramVerts_[0]);
        return getNumParamVerts();
    }

    /// \brief Return vector of pointers to ParamVertices associated with the MeshEdge
    void getParamVerts(std::vector<ParamVertex*> &pvs) const {
        pvs.clear();
        for (int i = 0; i < 2; ++i) {
            if (NULL != paramVerts_[i]) pvs.push_back(paramVerts_[i]);
        }
        return;
    }

    /// \brief Return number of ParamVertices associated with the MeshEdge
    MLINT getNumParamVerts() const
    {
        MLINT count = 0;
        for (int i = 0; i < 2; ++i) {
            if (NULL != paramVerts_[i]) ++count;
        }
        return count;
    }

private:
    /// The unique name counter for edges
    static MLUINT nameCounter_;

    /// Hidden default constructor
    MeshEdge():
        i1_(-1),
        i2_(-1)
    {
        paramVerts_[0] = paramVerts_[1] = NULL;
    };

    /// Return the hash value for this edge
    pwiFnvHash::FNVHash getHash() const;

    /// Return a hash value for the given point indices
    static pwiFnvHash::FNVHash computeHash(MLINT i1, MLINT i2);

    /// The index of the starting point in the edge
    MLINT i1_;
    /// The index of the ending point in the edge
    MLINT i2_;
    /// The ParamVertex objects associated with the points in the edge
    ParamVertex * paramVerts_[2];
};
typedef std::map<std::string, MeshEdge *> MeshEdgeNameMap;



/****************************************************************************
 * MeshFace class
 ***************************************************************************/
/**
 * \class MeshFace
 *
 * \brief 2-D (face) mesh entity
 *
 * Provides access to MeshLink schema %MeshFace data
 */
class MeshFace : public MeshTopo {
public:
    friend class MeshAssociativity;
    friend class MeshModel;
    friend class MeshSheet;
    friend class MeshString;

    /// \brief Construct a triangular MeshFace with point indices
    ///
    /// \param i1,i2,i3 the indices of the points defining the triangular face
    /// \param mid unique ID of the mesh face
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh face
    /// \param pv1,pv2,pv3 (optional) the ParamVertex objects associated with the face points
    MeshFace(MLINT i1, MLINT i2, MLINT i3,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3);

    /// \brief Construct a triangular MeshFace from application-defined reference data
    //
    /// \param ref the application-defined reference of the mesh face
    /// \param mid unique ID of the mesh face
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh face
    /// \param pv1,pv2,pv3 (optional) the ParamVertex objects associated with the face points
    MeshFace(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3);

    /// \brief Construct a quadrilateral MeshFace with point indices
    ///
    /// \param i1,i2,i3,i4 the indices of the points defining the quadrilateral face
    /// \param mid unique ID of the mesh face
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh face
    /// \param pv1,pv2,pv3,pv4 (optional) the ParamVertex objects associated with the face points
    MeshFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4);

    /// \brief Construct a quadrilateral MeshFace from application-defined reference data
    ///
    /// \param ref the application-defined reference of the mesh face
    /// \param mid unique ID of the mesh face
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh face
    /// \param pv1,pv2,pv3,pv4 (optional) the ParamVertex objects associated with the face points
    MeshFace(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4);

    /// \brief Destructor
    ~MeshFace();

    /// Return the base name used for generating unique names for faces
    virtual const std::string &getBaseName() const;
    /// Return the current value used for generating unique names for faces
    virtual MLUINT &getNameCounter();

    /// \brief Return array of vertex indices associated with the MeshFace
    ///
    /// N.B. Assumes inds is array of size 4.
    ///
    /// \param[in,out] inds the array of point indices
    /// \param[out] numInds the number of valid point indices in the array
    void getInds(MLINT *inds, MLINT *numInds) const;

    /// \brief Return array of ParamVertices associated with the MeshFace
    /// \return the number of ParamVertex objects in the array
    MLINT getParamVerts(ParamVertex * const ** pvs) const {
        *pvs = &(paramVerts_[0]);
        return getNumParamVerts();
    }

    /// \brief Return vector of pointers to ParamVertices associated with the MeshFace
    void getParamVerts(std::vector<ParamVertex*> &pvs) const {
        pvs.clear();
        for (int i = 0; i < 4; ++i) {
            if (NULL != paramVerts_[i]) pvs.push_back(paramVerts_[i]);
        }
        return;
    }

    /// \brief Return number of ParamVertices associated with the MeshFace
    MLINT getNumParamVerts() const
    {
        MLINT count = 0;
        for (int i = 0; i < 4; ++i) {
            if (NULL != paramVerts_[i]) ++count;
        }
        return count;
    }

private:
    /// The unique name counter for faces
    static MLUINT nameCounter_;

    /// Hidden default constructor
    MeshFace():
        i1_(-1),
        i2_(-1),
        i3_(-1),
        i4_(-1)
    {
        paramVerts_[0] = paramVerts_[1] = paramVerts_[2] = paramVerts_[3] = NULL;
    };

    /// Return the hash value for this face
    pwiFnvHash::FNVHash getHash() const;

    /// Return a hash value for the given point indices
    static pwiFnvHash::FNVHash computeHash(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// The index of the first point
    MLINT i1_;
    /// The index of the second point
    MLINT i2_;
    /// The index of the third point
    MLINT i3_;
    /// The index of the fourth point
    MLINT i4_;
    /// The array of ParamVertex objects associated with the face points
    ParamVertex * paramVerts_[4];
};

typedef std::map<std::string, MeshFace *> MeshFaceNameMap;

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
