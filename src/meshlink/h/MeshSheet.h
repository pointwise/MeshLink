#ifndef MESH_SHEET_CLASS
#define MESH_SHEET_CLASS

#include "Types.h"
#include "MeshTopo.h"
#include "MeshString.h"

#include <algorithm>
#include <map>
#include <string>


/****************************************************************************
* MeshSheet class
***************************************************************************/
/**
* \class MeshSheet
*
* \brief 2D (surface) mesh topology
*
* Provides access to MeshLink schema %MeshSheet data
*
*/
class ML_STORAGE_CLASS MeshSheet : public MeshTopo {
public:
    friend class MeshAssociativity;
    friend class MeshModel;

    /// Constructor
    MeshSheet(
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name);

    /// Construct with reference to Sheet entity in mesh data
    MeshSheet(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name);

    /// \brief Add a tri MeshFace to the MeshSheet using indices
    virtual bool addFace(MLINT i1, MLINT i2, MLINT i3,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Find a MeshFace in the MeshSheet associativity data
    virtual MeshFace *findFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED) const;

    /// \brief Delete a MeshFace from the MeshSheet associativity data
    virtual void deleteFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// \brief Add a tri MeshFace to the MeshSheet using reference
    virtual bool addFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Add a quad MeshFace to the MeshSheet using indices
    virtual bool addFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);

    /// \brief Add a quad MeshFace to the MeshSheet using reference
    virtual bool addFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);


    /// \brief Find a MeshFace by name
    virtual MeshFace * getMeshFaceByName(const std::string &name) const;

    /// \brief Find a MeshFace by reference
    virtual MeshFace * getMeshFaceByRef(const std::string &ref) const;

    /// \brief Add a face-edge to the MeshSheet
    ///
    /// Face-edges are a MeshEdge using the MeshFace's
    /// associativity data.  This is useful when
    /// performing MeshEdge operations with MeshFace constraints.
    virtual void addFaceEdge(MLINT i1, MLINT i2,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1 = NULL, ParamVertex *pv2 = NULL);

    /// \brief Find a face-edge in the associativity data
    virtual MeshEdge *findFaceEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Delete a face-edge from the associativity data
    virtual void deleteFaceEdgeByInds(MLINT i1, MLINT i2);

    /// \brief Return the number of face-edges in the MeshSheet
    virtual MLINT getNumFaceEdges() const;

    /// \brief Return the number of MeshFaces in the MeshSheet
    virtual MLINT getNumFaces() const;

    /// \brief Return array of face-edges in the MeshSheet
    virtual std::vector<const MeshEdge *> getFaceEdges() const;

    /// \brief Return array of MeshFaces in the MeshSheet
    virtual std::vector<const MeshFace *> getMeshFaces() const;

    MeshSheet();
    ~MeshSheet();

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

private:
    // unique name counter
    static MLUINT nameCounter_;

    // Map point index hash to point
    std::map<pwiFnvHash::FNVHash, MeshPoint*> pointMap_; // not the owner
    MeshPointNameMap        meshPointNameMap_;  // owner
    MeshTopoIDToNameMap     meshPointIDToNameMap_;

    // Map face edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> faceEdgeMap_;

    // Map face indices hash to face
    std::map<pwiFnvHash::FNVHash, MeshFace*> faceMap_; // not the owner
    MeshFaceNameMap         meshFaceNameMap_;  // owner
    MeshTopoIDToNameMap     meshFaceIDToNameMap_;
    MeshTopoRefToNameMap    meshFaceRefToNameMap_;
};
typedef std::map<std::string, MeshSheet *> MeshSheetNameMap;


#endif
