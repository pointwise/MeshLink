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
* Provides access to MeshLink schema %ParamVertex data
*
*/
class ParamVertex {
public:
    ParamVertex() :
        gref_(MESH_TOPO_INVALID_REF),
        mid_(MESH_TOPO_INVALID_REF),
        u_(0.0),
        v_(0.0) {}

    ParamVertex(std::string & vref, MLINT gref, MLINT mid, MLREAL u, MLREAL v) :
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
    void  getUV(MLREAL *u, MLREAL *v) const { *u = u_; *v = v_; }
private:
    std::string vref_;  // required attr
    MLINT gref_;  // required attr
    MLINT mid_;   // optional attr
    MLREAL u_;
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

    /// Constructor
    MeshTopo(
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name );

    /// Construct with reference to entity in mesh data
    MeshTopo(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name );

    /// \brief Return the ID of this MeshTopo
    virtual MLINT getID() const;

    /// \brief Return the GID of GeometryGroup associated with this MeshTopo
    virtual MLINT getGref() const;

    /// \brief Return the AttID of MeshLinkAttribute associated with this MeshTopo
    virtual MLINT getAref() const;

    /// \brief Return the name of this MeshTopo
    ///
    /// N.B. return value subject to change
    virtual void getName(const char **name) const;

    /// \brief Return the name of this MeshTopo
    virtual const std::string & getName() const;

    /// \brief Return the mesh data reference of this MeshTopo
    virtual const std::string & getRef() const;

    /// \brief Add a ParamVertex 
    virtual void addParamVertex(ParamVertex *pv, bool mapID);

    /// \brief Find a ParamVertex by vertex reference
    virtual ParamVertex * getParamVertByVref(const std::string & vref) const;

    /// \brief Find a ParamVertex by ID
    virtual ParamVertex * getParamVertByID(MLINT id) const;

    /// \brief Return the number of ParamVertices for this MeshTopo
    virtual MLINT getNumParamVerts() const;


    /// \brief Set the ID of this MeshTopo
    virtual void setID( MLINT id );

    /// \brief Set the GeometryGroup GID referenced by this MeshTopo
    virtual void setGref( MLINT gref );

    /// \brief Set the MeshLinkAttribute AttID referenced by this MeshTopo
    virtual void setAref( MLINT aref );

    /// \brief Set the reference of this MeshTopo
    virtual void setRef(const char *ref);

    /// \brief Set the name of this MeshTopo
    ///
    /// setName is special and should not be overrided
    /// it provides a mechanism for generating unique names
    void setName(const std::string &name);

    /// \brief Set the name of this MeshTopo
    ///
    /// setName is special and should not be overrided
    /// it provides a mechanism for generating unique names
    void setName(const char *name);

    /// \brief Whether the MeshTopo has an ID defined
    bool hasID() const;

    /// \brief Whether the MeshTopo has a GeometryGroup GID reference defined
    bool hasGref() const;

    /// \brief Whether the MeshTopo has a MeshLinkAttribute AttID reference defined
    bool hasAref() const;

    /// \brief Return list of MeshLinkAttribute AttIDs referenced by the MeshTopo
    std::vector<MLINT> getAttributeIDs(const MeshAssociativity &meshAssoc) const;

    // get a unique name for the entity
    std::string getNextName();

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

    MeshTopo();
    ~MeshTopo();
protected:
    std::string ref_;

    // MeshLink schema: MeshElementAttributes 
    MLINT mid_;
    MLINT aref_;
    MLINT gref_;
    std::string name_;

    // ParamVerts associated with this entity
    ParamVertVrefMap paramVertVrefMap_;
    ParamVertIDToVrefMap paramVertIDToVrefMap_;

private:
    // unique name counter
    static MLUINT nameCounter_;
};
typedef std::map<MLINT, std::string> MeshTopoIDToNameMap;

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

    /// Constructor
    MeshPoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1
    );

    /// Construct with reference to Point entity in mesh data
    MeshPoint(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1
    );

    /// \brief Return ParamVertex associated with the MeshPoint
    ParamVertex*const getParamVert() const {
        return paramVert_;
    }

    /// \brief Return array of ParamVertices associated with the MeshPoint
    MLINT getParamVerts(ParamVertex * const ** pvs) const {
        *pvs = &(paramVert_);
        return 1;
    }

    ~MeshPoint();

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

private:
    // unique name counter
    static MLUINT nameCounter_;

    MeshPoint():
        i1_(-1),
        paramVert_(NULL)
    {};

    static pwiFnvHash::FNVHash computeHash(MLINT i1);

    pwiFnvHash::FNVHash getHash() const;

    MLINT i1_;
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
    MeshEdge(MLINT i1, MLINT i2,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2);

    MeshEdge(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2);

    ~MeshEdge();

    MeshEdge(const MeshEdge &other);

    MeshEdge & operator=(const MeshEdge &other);

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

    /// \brief Return array of vertex indices associated with the MeshEdge
    ///
    /// N.B. Assumes inds is array of size 2.
    void getInds(MLINT **inds) const;

    /// \brief Return pointer to array of ParamVertices associated with the MeshEdge
    MLINT getParamVerts( ParamVertex * const ** pvs) const {
        *pvs = &(paramVerts_[0]);
        return 2; 
    }

    /// \brief Return vector of pointers to ParamVertices associated with the MeshEdge
    std::vector<ParamVertex*> getParamVerts() const {
        std::vector<ParamVertex*> pvs;
        pvs.push_back(paramVerts_[0]);
        pvs.push_back(paramVerts_[1]);
        return pvs;
    }

private:
    // unique name counter
    static MLUINT nameCounter_;

    MeshEdge():
        i1_(-1),
        i2_(-1)
    {
        paramVerts_[0] = paramVerts_[1] = NULL;
    };

    pwiFnvHash::FNVHash getHash() const;

    static pwiFnvHash::FNVHash computeHash(MLINT i1, MLINT i2);

    MLINT i1_;
    MLINT i2_;
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
    MeshFace(MLINT i1, MLINT i2, MLINT i3,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3);
    MeshFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3);

    MeshFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4);
    MeshFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4);

    ~MeshFace();

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

    /// \brief Return array of vertex indices associated with the MeshFace
    ///
    /// N.B. Assumes inds is array of size 4.
    void getInds(MLINT **inds) const;

    /// \brief Return array of ParamVertices associated with the MeshFace
    MLINT getParamVerts(ParamVertex * const ** pvs) const {
        *pvs = &(paramVerts_[0]);
        return 4;
    }

    /// \brief Return vector of ParamVertices associated with the MeshFace
    std::vector<ParamVertex*> getParamVerts() const {
        std::vector<ParamVertex*> pvs;
        pvs.push_back(paramVerts_[0]);
        pvs.push_back(paramVerts_[1]);
        pvs.push_back(paramVerts_[2]);
        pvs.push_back(paramVerts_[3]);
        return pvs;
    }

private:
    // unique name counter
    static MLUINT nameCounter_;

    MeshFace():
        i1_(-1),
        i2_(-1),
        i3_(-1),
        i4_(-1)
    {
        paramVerts_[0] = paramVerts_[1] = paramVerts_[2] = paramVerts_[3] = NULL;
    };

    pwiFnvHash::FNVHash getHash() const;
    static pwiFnvHash::FNVHash computeHash(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);
    MLINT i1_;
    MLINT i2_;
    MLINT i3_;
    MLINT i4_;
    ParamVertex * paramVerts_[4];
};
typedef std::map<std::string, MeshFace *> MeshFaceNameMap;

#endif
