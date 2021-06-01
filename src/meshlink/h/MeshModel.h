/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#ifndef MESH_MODEL_CLASS
#define MESH_MODEL_CLASS

#include "Types.h"
#include "MeshTopo.h"
#include "MeshString.h"
#include "MeshSheet.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>
using StringArray = std::vector<std::string>;

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
    //
    /// \param ref the application-defined reference string
    /// \param mid the model ID
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the model name
    MeshModel(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name);

    /// \brief Add a MeshPoint to the MeshModel using index
    //
    /// \param i1 the uniuque point index
    /// \param mid the ID of the point
    /// \param aref the point attribute reference ID (AttID)
    /// \param gref the point geometry reference ID
    /// \param name the point name
    /// \param pv1 the ParamVertex parametric location of the point
    /// \param mapID indicates whether to map the ID to the point name
    bool addPoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, bool mapID);

    /// \brief Add a MeshPoint to the MeshModel using reference
    //
    /// \param ref the application-defined reference string
    /// \param mid the ID of the point
    /// \param aref the point attribute reference ID (AttID)
    /// \param gref the point geometry reference ID
    /// \param name the point name (a unique name will be generated if empty)
    /// \param pv1 the ParamVertex parametric location of the point
    /// \param mapID indicates whether to map the ID to the point name
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
    ///
    /// \param i1 the index of the desired MeshPoint
    MeshPoint *findLowestTopoPointByInd(MLINT i1) const;

    /// \brief Find a point at the highest topology level of the associativity data
    ///
    /// Searches MeshModel, MeshSheet, MeshString topology levels in that order
    ///
    /// \param i1 The index of the desired MeshPoint
    MeshPoint *findHighestTopoPointByInd(MLINT i1) const;

    /// \brief Find a point in the MeshModel associativity data
    ///
    /// Search is restricted to the MeshModel topology level
    ///
    /// \param i1 The index of the desired MeshPoint
    MeshPoint * findPointByInd(MLINT i1) const;

    /// \brief Delete an MeshPoint from the MeshModel by point index
    ///
    /// Removes MeshPoint, edge-point, and face-edge-point from the MeshModel.
    ///
    /// Removes the MeshPoint from the MeshModel only,
    /// MeshString and MeshSheet data is unchanged.
    ///
    /// \param i1 the index of the point to delete
    void deletePointByInd(MLINT i1);
    /// \brief Delete an MeshPoint from the MeshModel by application-defined reference
    /// \param ref the application-defined reference of the point to delete
    /// \sa MeshModel.deletePointByInd
    void deletePointByRef(const std::string &ref);
    /// \brief Delete an MeshPoint from the MeshModel by application-defined integer reference
    /// \param iref the application-defined integer reference of the point to delete
    /// \sa MeshModel.deletePointByInd
    void deletePointByRef(MLINT iref);

    /// \brief Return a MeshPoint witht the given name
    /// \param name the name of the desired MeshPoint
    MeshPoint* getMeshPointByName(const std::string &name) const;
    /// \brief Return a MeshPoint witht the given ID
    /// \param id the unique ID of the desired MeshPoint
    MeshPoint* getMeshPointByID(MLINT id) const;
    /// \brief Return a MeshPoint witht the given reference
    /// \param ref the application-defined reference string of the desired MeshPoint
    MeshPoint* getMeshPointByRef(const std::string &ref) const;

    /// Return an array of all MeshPoint references in this model
    StringArray getMeshPointRefs() const;

    /// \brief Add a MeshEdge to the MeshModel using indices
    ///
    /// MeshEdges belong to MeshStrings, however, adding
    /// them to the model is useful for access patterns
    /// when working with a MeshModel.
    ///
    /// \param i1,i2 point indices defining an edge
    /// \param mid unique ID of edge
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the name of the edge (a unique name will be generated if empty)
    /// \param pv1 the ParamVertex for the starting point, if any
    /// \param pv2 the ParamVertex for the ending point, if any
    /// \param mapID whether to record the mapping of unique ID to name
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
    /// when working with a MeshModel.
    //
    /// \param ref the application-defined reference data for the edge
    /// \param mid unique ID of edge
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the name of the edge (a unique name will be generated if empty)
    /// \param pv1 the ParamVertex for the starting point, if any
    /// \param pv2 the ParamVertex for the ending point, if any
    /// \param mapID whether to record the mapping of unique ID to name
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
    /// when working with a MeshModel.
    ///
    /// \param i1 index of edge point
    /// \param mid unique ID of edge point
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param pv1 the ParamVertex for the edge point, if any
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
    /// when working with a MeshModel.
    ///
    /// \param i1 index of face edge point
    /// \param mid unique ID of face edge point
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param pv1 the ParamVertex for the face edge point, if any
    void addFaceEdgePoint(MLINT i1,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1);

    /// \brief Find an edge-point in the MeshString associativity data.
    ///
    /// Search is restricted to the MeshString topology level
    ///
    /// \param i1 the index of the desired edge-point
    MeshPoint * findEdgePointByInd(MLINT i1) const;

    /// \brief Find a face-edge-point in the MeshSheet associativity data.
    ///
    /// Search is restricted to the MeshSheet topology level
    ///
    /// \param i1 the index of the desired face-edge-point
    MeshPoint *findFaceEdgePointByInd(MLINT i1) const;

    /// \brief Add a face-edge to the MeshModel
    ///
    /// Face-edges are created when a MeshFace is added
    /// to a MeshSheet.  They belong to MeshSheets, however,
    /// adding them to the model is useful for access patterns
    /// when working with a MeshModel.
    ///
    /// \param i1,i2 the starting point index of the face-edge
    /// \param mid unique ID of face edge
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param pv1,pv2 (optional) the ParamVertex objects associated with the face-edge points
    void addFaceEdge(MLINT i1, MLINT i2,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1, ParamVertex *pv2);

    /// \brief Find a MeshEdge by name
    ///
    /// \param name the name of the desired edge
    MeshEdge * getMeshEdgeByName(const std::string &name) const;

    /// \brief Find a MeshEdge by reference
    ///
    /// \param ref the application-defined reference of the desired edge
    MeshEdge * getMeshEdgeByRef(const std::string &ref) const;

    /// \brief Find an edge at the lowest topology level of the associativity data
    ///
    /// Searches MeshString and MeshSheet topology levels in that order
    ///
    /// \param i1,i2 the indices of the points defining the desired string or sheet edge
    MeshEdge *findLowestTopoEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Find an edge in the MeshString associativity data
    ///
    /// Search is restricted to the MeshString topology level
    ///
    /// \param i1,i2 the indices of the points defining the desired string edge
    MeshEdge *findEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Find an edge in the MeshSheet associativity data
    ///
    /// Search is restricted to the MeshSheet topology level
    ///
    /// \param i1,i2 the indices of the points defining the desired face-edge
    MeshEdge *findFaceEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Delete an MeshEdge from the MeshModel by point indices
    //
    /// Removes MeshEdge and face-edge from the MeshModel.
    //
    /// Removes the MeshEdge from the MeshModel only,
    /// MeshString and MeshSheet data is unchanged.
    ///
    /// \param i1,i2 indices of the points defining the edge to delete
    void deleteEdgeByInds(MLINT i1, MLINT i2);

    /// \brief Delete an MeshEdge from the MeshModel by application-defined reference
    /// \param ref application-defined reference of the edge to delete
    /// \sa MeshModel.deleteEdgeByInds
    void deleteEdgeByRef(const std::string &ref);

    /// \brief Delete an MeshEdge from the MeshModel by application-defined integer reference
    /// \param iref application-defined integer reference of the edge to delete
    /// \sa MeshModel.deleteEdgeByInds
    void deleteEdgeByRef(MLINT iref);

    /// \brief Add a triangular MeshFace to the MeshModel using indices
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with a MeshModel.
    ///
    /// \param i1,i2,i3 the point indices of the triangular face to add
    /// \param mid unique ID of face
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the name of the triangular face to add
    /// \param pv1,pv2,pv3 (optional) the ParamVertex objects associated with the face points
    /// \param mapID whether to record the mapping of unique ID to name
    bool addFace(MLINT i1, MLINT i2, MLINT i3,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Add a triangular MeshFace to the MeshModel using reference
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with a MeshModel.
    ///
    /// \param ref the application-defined reference of the face to add
    /// \param mid unique ID of face
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the name of the triangular face to add
    /// \param pv1,pv2,pv3 (optional) the ParamVertex objects associated with the face points
    /// \param mapID whether to record the mapping of unique ID to name
    bool addFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Add a quadrilateral MeshFace to the MeshModel using indices
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with a MeshModel.
    ///
    /// \param i1,i2,i3,i4 the point indices of the quadrilateral face to add
    /// \param mid unique ID of face
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the name of the quadrilateral face to add
    /// \param pv1,pv2,pv3,pv4 (optional) the ParamVertex objects associated with the face points
    /// \param mapID whether to record the mapping of unique ID to name
    bool addFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);

    /// \brief Add a quadrilateral MeshFace to the MeshModel using reference
    ///
    /// MeshFaces belong to MeshSheets, however, adding
    /// them to the model is useful for access patterns
    /// when working with a MeshModel.
    ///
    /// \param ref the application-defined reference of the face to add
    /// \param mid unique ID of face
    /// \param aref the attribute reference ID (AttID)
    /// \param gref the geometry reference ID
    /// \param name the name of the quadrilateral face to add
    /// \param pv1,pv2,pv3,pv4 (optional) the ParamVertex objects associated with the face points
    /// \param mapID whether to record the mapping of unique ID to name
    bool addFace(
        std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);

    /// \brief Find a (triangular or quadrilateral) face in the MeshSheet associativity data
    ///
    /// \param i1,i2,i3 the point indices of the desired triangular or quadrilateral face
    /// \param i4 (optional) the index of the fourth point of the desired quadrilateral face
    MeshFace *findFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// \brief Delete an MeshFace from the MeshModel using indices
    ///
    /// Removes MeshFace from the MeshModel only.
    /// MeshSheet data is unchanged.
    ///
    /// \param i1,i2,i3 the point indices of the triangular or quadrilateral face to delete
    /// \param i4 (optional) the index of the fourth point of the quadrilateral face to delete
    void deleteFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// \brief Delete an MeshFace from the MeshModel using reference
    ///
    /// Removes MeshFace from the MeshModel only.
    /// MeshSheet data is unchanged.
    ///
    /// \param ref the application-defined reference of the face to delete
    void deleteFaceByRef(const std::string &ref);

    /// \brief Delete an MeshFace from the MeshModel using reference
    ///
    /// Removes MeshFace from the MeshModel only.
    /// MeshSheet data is unchanged.
    //
    /// \param iref the application-defined integer reference of the face to delete
    void deleteFaceByRef(MLINT iref);

    /// \brief Find a MeshFace in the MeshModel using name
    ///
    /// \param name the name of the desired face
    MeshFace * getMeshFaceByName(const std::string &name) const;

    /// \brief Find a MeshFace in the MeshModel using reference
    ///
    /// \param ref the application-defined reference string of the desired face
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
    ///
    /// \param meshString the MeshString to add
    /// \param mapID whether to map the ID of the MeshString to its unique name
    bool addMeshString(MeshString* meshString, bool mapID=false);

    /// \brief Find a MeshString by ID
    ///
    /// \param id the ID of the desired MeshString
    MeshString*     getMeshStringByID(MLINT id) const;

    /// \brief Find a MeshString by name
    ///
    /// \param name the unique name of the desired MeshString
    MeshString*     getMeshStringByName(const std::string &name) const;

    /// \brief Find a MeshString by reference
    ///
    /// \param ref the application-defined reference of the desired MeshString
    MeshString * getMeshStringByRef(const std::string &ref) const;

    /// \brief Add a MeshSheet to the MeshModel
    ///
    /// \param meshSheet the MeshSheet to add
    /// \param mapID whether to map the unique ID of the MeshSheet to its name
    bool addMeshSheet(MeshSheet* meshSheet, bool mapID=false);

    /// \brief Find a MeshSheet by ID
    ///
    /// \param id the unique ID of the desired MeshSheet
    MeshSheet*      getMeshSheetByID(MLINT id) const;

    /// \brief Find a MeshSheet by name
    ///
    /// \param name the unique name of the desired MeshSheet
    MeshSheet*      getMeshSheetByName(const std::string &name) const;

    /// \brief Find a MeshSheet by reference
    //
    /// \param ref the application-defined reference string of the desired MeshSheet
    MeshSheet * getMeshSheetByRef(const std::string &ref) const;

    /// \brief Return list of MeshEdge in the MeshModel
    ///
    ///  Return will include all MeshEdge objects from all MeshString objects in this model
    void getMeshEdges(std::vector<const MeshEdge *> &edges) const;

    /// \brief Return list of all MeshFace in the MeshModel
    void getMeshFaces(std::vector<const MeshFace *> &faces) const;

    /// \brief Return list of MeshString in the MeshModel
    ///
    /// Return will include all MeshString objects in this model
    void getMeshStrings(std::vector<MeshString *> &strings) const;

    /// \brief Return count of MeshString in the MeshModel
    size_t getMeshStringCount() const;

    /// \brief Return list of all MeshSheet in the MeshModel
    void getMeshSheets(std::vector<MeshSheet *> &sheets) const;

    /// \brief Return count of MeshSheets in the MeshModel
    size_t getMeshSheetCount() const;

    /// Destructor
    ~MeshModel();

    /// \brief Return the unique base name for this model
    virtual const std::string &getBaseName() const;
    /// \brief Return the current value of the unique base name counter
    virtual MLUINT &getNameCounter();

private:
    /// unique name counter, global for all MeshTopo object names
    static MLUINT nameCounter_;

    /// Hidden default constructor
    MeshModel() {};

    /// Map point index hash to MeshPoint
    std::map<pwiFnvHash::FNVHash, MeshPoint*> pointMap_; // not the owner
    /// Map MeshPoint name to unique ID
    MeshPointNameMap        meshPointNameMap_;  // owner
    /// Map MeshPoint unique ID to name
    MeshTopoIDToNameMap     meshPointIDToNameMap_;
    /// Map MeshPoint application-defined reference string to name
    MeshTopoRefToNameMap    meshPointRefToNameMap_;

    /// Map edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> edgeMap_; // not the owner
    /// Map MeshEdge name to unique ID
    MeshEdgeNameMap         meshEdgeNameMap_;  // owner
    /// Map MeshEdge name to unique ID
    MeshTopoIDToNameMap     meshEdgeIDToNameMap_;
    /// Map MeshEdge application-defined reference string to name
    MeshTopoRefToNameMap    meshEdgeRefToNameMap_;

    /// Map edge-point index hash to MeshPoint
    std::map<pwiFnvHash::FNVHash, MeshPoint*> edgePointMap_; // owner
    /// Map face-edge-point index hash to MeshPoint
    std::map<pwiFnvHash::FNVHash, MeshPoint*> faceEdgePointMap_; // owner


    /// Map face-edge index hash to MeshEdge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> faceEdgeMap_; // owner

    /// Map face index hash to MeshFace
    std::map<pwiFnvHash::FNVHash, MeshFace*> faceMap_; // not the owner
    /// Map MeshFace name to unique ID
    MeshFaceNameMap         meshFaceNameMap_;  // owner
    /// Map MeshFace unique ID to name
    MeshTopoIDToNameMap     meshFaceIDToNameMap_;
    /// Map MeshFace application-defined reference string to name
    MeshTopoRefToNameMap    meshFaceRefToNameMap_;

    /// Map MeshString name to unique ID
    MeshStringNameMap       meshStringNameMap_;
    /// Map MeshString unique ID to name
    MeshTopoIDToNameMap     meshStringIDToNameMap_;
    /// Map MeshString application-defined reference string to name
    MeshTopoRefToNameMap    meshStringRefToNameMap_;

    /// Map MeshSheet name to unique ID
    MeshSheetNameMap        meshSheetNameMap_;
    /// Map MeshSheet unique ID to name
    MeshTopoIDToNameMap     meshSheetIDToNameMap_;
    /// Map MeshSheet application-defined reference string to name
    MeshTopoRefToNameMap    meshSheetRefToNameMap_;
};

typedef std::map<std::string, MeshModel *> MeshModelNameMap;

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
