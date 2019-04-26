#ifndef MESH_STRING_CLASS
#define MESH_STRING_CLASS

#include "Types.h"
#include "MeshTopo.h"

#include <algorithm>
#include <map>
#include <string>

/****************************************************************************
* MeshString class
***************************************************************************/
/**
* \class MeshString
*
* \brief 1D (curve) mesh topology
*
* Provides access to MeshLink schema %MeshString data
*
*/
class ML_STORAGE_CLASS MeshString : public MeshTopo {
public:
    friend class MeshAssociativity;
    friend class MeshModel;

    /// Constructor
    MeshString(
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name);

    /// Construct with reference to String entity in mesh data
    MeshString(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name);

    /// \brief Add a MeshEdge to the MeshString using indices
    virtual bool addEdge(MLINT i1, MLINT i2, 
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2, bool mapID);

    /// \brief Add a MeshEdge to the MeshString using reference
    virtual bool addEdge(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2, bool mapID);

    /// \brief Find a MeshEdge in the MeshString associativity data
    virtual MeshEdge *findEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Delete a MeshEdge from the MeshString associativity data
    virtual void deleteEdgeByInds(MLINT i1, MLINT i2);

    /// \brief Find a MeshEdge by name
    virtual MeshEdge * getMeshEdgeByName(const std::string &name) const;

    /// \brief Find a MeshEdge by reference
    virtual MeshEdge * getMeshEdgeByRef(const std::string &ref) const;

    /// \brief Return the number of MeshEdges in the MeshString
    virtual MLINT getNumEdges() const;

    /// \brief Return array of MeshEdges in the MeshString
    virtual std::vector<const MeshEdge *> getMeshEdges() const;

    MeshString();
    ~MeshString();

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

private:
    // unique name counter
    static MLUINT nameCounter_;

    // Map point index hash to point
    std::map<pwiFnvHash::FNVHash, MeshPoint*> pointMap_;  // not the owner
    MeshPointNameMap        meshPointNameMap_;  // owner
    MeshTopoIDToNameMap     meshPointIDToNameMap_;

    // Map edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> edgeMap_; // not the owner
    MeshEdgeNameMap         meshEdgeNameMap_;  // owner
    MeshTopoIDToNameMap     meshEdgeIDToNameMap_;
    MeshTopoRefToNameMap    meshEdgeRefToNameMap_;
};
typedef std::map<std::string, MeshString *> MeshStringNameMap;


#endif
