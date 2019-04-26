#ifndef MESH_MODEL_CLASS
#define MESH_MODEL_CLASS

#include "Types.h"
#include "MeshTopo.h"
#include "MeshString.h"
#include "MeshSheet.h"

#include <algorithm>
#include <map>
#include <string>


/****************************************************************************
* MeshModel class
***************************************************************************/
/**
* \class MeshModel
*
* \brief 3D (volume) mesh topology
*
* Provides access to MeshLink schema %MeshModel data and
* is a container for MeshString and MeshSheet objects.
*
*/
class ML_STORAGE_CLASS MeshModel : public MeshTopo {
public:
    friend class MeshAssociativity;

    /// Construct with reference to Model entity in mesh data
    MeshModel(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name);

    /// \brief Add a MeshPoint to the MeshModel using index
    bool addPoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, bool mapID);

    /// \brief Add a MeshPoint to the MeshModel using reference
    bool addPoint(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, bool mapID);

    /// \brief Find a point at the lowest topology level of the associativity data
    ///
    /// Searches MeshString, MeshSheet, MeshModel topology levels in that order
    MeshPoint *findLowestTopoPointByInd(MLINT i1) const;

    /// \brief Find a point at the highest topology level of the associativity data
    ///
    /// Searches MeshModel, MeshSheet, MeshString topology levels in that order
    MeshPoint *findHighestTopoPointByInd(MLINT i1) const;

    /// \brief Find a point in the MeshModel associativity data
    ///
    /// Search is restricted to the MeshModel topology level
    MeshPoint * findPointByInd(MLINT i1) const;


    /// \brief Delete an MeshPoint from the MeshModel
    ///
    /// Removes MeshPoint, edge-point, and face-edge-point from the MeshModel.
    ///
    /// Removes the MeshPoint from the MeshModel only,
    /// MeshString and MeshSheet data is unchanged. 
    void deletePointByInd(MLINT i1);
    void deletePointByRef(const std::string &ref);
    void deletePointByRef(MLINT iref);

    MeshPoint* getMeshPointByName(const std::string &name) const;
    MeshPoint* getMeshPointByID(MLINT id) const;
    MeshPoint* getMeshPointByRef(const std::string &ref) const;


    /// \brief Add a MeshEdge to the MeshModel using indices
    ///
    /// MeshEdges belong to MeshStrings, however, adding
    /// them to the model is useful for access patterns
    /// when working with MeshModels.
    bool addEdge(MLINT i1, MLINT i2,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2, bool mapID);

    /// \brief Add a MeshEdge to the MeshModel using reference
    ///
    /// MeshEdges belong to MeshStrings, however, adding
    /// them to the model is useful for access patterns
    /// when working with MeshModels.
    bool addEdge(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2, bool mapID);

    /// \brief Add a edge-point to the MeshModel
    ///
    /// Edge-points are created when a MeshEdge is added
    /// to a MeshString.  They belong to MeshStrings, however, 
    /// adding them to the model is useful for access patterns
    /// when working with MeshModels.
    void addEdgePoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1);

    /// \brief Add a face-edge-point to the MeshModel
    ///
    /// Face-edge-points are created when a MeshFace is added
    /// to a MeshSheet.  They belong to MeshSheets, however, 
    /// adding them to the model is useful for access patterns
    /// when working with MeshModels.
    void addFaceEdgePoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1);

    /// \brief Find an edge-point in the MeshString associativity data.
    ///
    /// Search is restricted to the MeshString topology level
    MeshPoint * findEdgePointByInd(MLINT i1) const;

    /// \brief Find a face-edge-point in the MeshSheet associativity data.
    ///
    /// Search is restricted to the MeshSheet topology level
    MeshPoint *findFaceEdgePointByInd(MLINT i1) const;


    /// \brief Add a face-edge to the MeshModel
    ///
    /// Face-edges are created when a MeshFace is added
    /// to a MeshSheet.  They belong to MeshSheets, however, 
    /// adding them to the model is useful for access patterns
    /// when working with MeshModels.
    void addFaceEdge(MLINT i1, MLINT i2, 
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1, ParamVertex *pv2);


    /// \brief Find a MeshEdge by name
    MeshEdge * getMeshEdgeByName(const std::string &name) const;

    /// \brief Find a MeshEdge by reference
    MeshEdge * getMeshEdgeByRef(const std::string &ref) const;

    /// \brief Find an edge at the lowest topology level of the associativity data
    ///
    /// Searches MeshString and MeshSheet topology levels in that order
    MeshEdge *findLowestTopoEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Find an edge in the MeshString associativity data
    ///
    /// Search is restricted to the MeshString topology level
    MeshEdge *findEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Find an edge in the MeshSheet associativity data
    ///
    /// Search is restricted to the MeshSheet topology level
    MeshEdge *findFaceEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Delete an MeshEdge from the MeshModel
    ///
    /// Removes MeshEdge and face-edge from the MeshModel.
    ///
    /// Removes the MeshEdge from the MeshModel only,
    /// MeshString and MeshSheet data is unchanged. 
    void deleteEdgeByInds(MLINT i1, MLINT i2);
    void deleteEdgeByRef(const std::string &ref);
    void deleteEdgeByRef(MLINT iref);

    /// \brief Add a MeshFace to the MeshModel using indices
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with MeshModels.
    bool addFace(MLINT i1, MLINT i2, MLINT i3,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Add a MeshFace to the MeshModel using reference
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with MeshModels.
    bool addFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Add a MeshFace to the MeshModel using indices
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with MeshModels.
    bool addFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);

    /// \brief Add a MeshFace to the MeshModel using reference
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with MeshModels.
    bool addFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);

    /// \brief Find a face in the MeshSheet associativity data
    MeshFace *findFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// \brief Delete an MeshFace from the MeshModel using indices
    ///
    /// Removes MeshFace from the MeshModel only.
    /// MeshSheet data is unchanged. 
    void deleteFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// \brief Delete an MeshFace from the MeshModel using reference
    ///
    /// Removes MeshFace from the MeshModel only.
    /// MeshSheet data is unchanged. 
    void deleteFaceByRef(const std::string &ref);


    /// \brief Delete an MeshFace from the MeshModel using reference
    ///
    /// Removes MeshFace from the MeshModel only.
    /// MeshSheet data is unchanged. 
    void deleteFaceByRef(MLINT iref);

    /// \brief Find a MeshFace in the MeshModel using name
    MeshFace * getMeshFaceByName(const std::string &name) const;

    /// \brief Find a MeshFace in the MeshModel using reference
    MeshFace * getMeshFaceByRef(const std::string &ref) const;


    /// \brief Return the number of edges in the MeshModel
    //
    ///  Return will be the count of all MeshEdges from all MeshStrings 
    MLINT getNumEdges() const;

    /// \brief Return the number of faces in the MeshModel
    //
    ///  Return will be the count of all MeshFaces from all MeshSheets 
    MLINT getNumFaces() const;

    /// \brief Add a MeshString to the MeshModel
    bool addMeshString(MeshString* meshString, bool mapID=false);

    /// \brief Find a MeshString by ID
    MeshString*     getMeshStringByID(MLINT id) const;

    /// \brief Find a MeshString by name
    MeshString*     getMeshStringByName(const std::string &name) const;

    /// \brief Find a MeshString by reference
    MeshString * getMeshStringByRef(const std::string &ref) const;

    /// \brief Add a MeshSheet to the MeshModel
    bool addMeshSheet(MeshSheet* MeshSheet, bool mapID=false);

    /// \brief Find a MeshSheet by ID
    MeshSheet*      getMeshSheetByID(MLINT id) const;

    /// \brief Find a MeshSheet by name
    MeshSheet*      getMeshSheetByName(const std::string &name) const;

    /// \brief Find a MeshSheet by reference
    MeshSheet * getMeshSheetByRef(const std::string &ref) const;

    /// \brief Return list of MeshEdges in the MeshModel
    ///
    ///  Return will include all MeshEdges from all MeshStrings 
    std::vector<const MeshEdge *> getMeshEdges() const;


    /// \brief Return list of MeshFaces in the MeshModel
    ///
    ///  Return will include all MeshFaces from all MeshSheets 
    std::vector<const MeshFace *> getMeshFaces() const;

    /// \brief Return list of MeshStrings in the MeshModel
    std::vector<MeshString *> getMeshStrings() const;

    /// \brief Return list of MeshSheets in the MeshModel
    std::vector<MeshSheet *> getMeshSheets() const;

    // Destructor
    ~MeshModel();

    // get unique name base and counter
    virtual const std::string &getBaseName() const;
    virtual MLUINT &getNameCounter();

private:
    // unique name counter
    static MLUINT nameCounter_;

    MeshModel() {};

    // Map point index hash to point
    std::map<pwiFnvHash::FNVHash, MeshPoint*> pointMap_; // not the owner
    MeshPointNameMap        meshPointNameMap_;  // owner
    MeshTopoIDToNameMap     meshPointIDToNameMap_;
    MeshTopoRefToNameMap    meshPointRefToNameMap_;

    // Map edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> edgeMap_; // not the owner
    MeshEdgeNameMap         meshEdgeNameMap_;  // owner
    MeshTopoIDToNameMap     meshEdgeIDToNameMap_;
    MeshTopoRefToNameMap    meshEdgeRefToNameMap_;

    // Map edge-point index hash to point
    std::map<pwiFnvHash::FNVHash, MeshPoint*> edgePointMap_; // owner
    std::map<pwiFnvHash::FNVHash, MeshPoint*> faceEdgePointMap_; // owner


    // Map face edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> faceEdgeMap_; // owner

    // Map face indices hash to face
    std::map<pwiFnvHash::FNVHash, MeshFace*> faceMap_; // not the owner
    MeshFaceNameMap         meshFaceNameMap_;  // owner
    MeshTopoIDToNameMap     meshFaceIDToNameMap_;
    MeshTopoRefToNameMap    meshFaceRefToNameMap_;

    // Mesh Strings
    MeshStringNameMap       meshStringNameMap_;
    MeshTopoIDToNameMap     meshStringIDToNameMap_;
    MeshTopoRefToNameMap    meshStringRefToNameMap_;

    // Mesh Sheets
    MeshSheetNameMap        meshSheetNameMap_;
    MeshTopoIDToNameMap     meshSheetIDToNameMap_;
    MeshTopoRefToNameMap    meshSheetRefToNameMap_;
};
typedef std::map<std::string, MeshModel *> MeshModelNameMap;


#endif
