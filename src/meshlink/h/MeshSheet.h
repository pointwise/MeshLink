/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

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

    /// \brief Constructor with no application-defined reference data
    ///
    /// \param mid unique ID of the mesh sheet
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh sheet
    MeshSheet(
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name);

    /// \brief Constructor with application-defined reference data
    ///
    /// \param ref application-defined reference data for the mesh sheet
    /// \param mid unique ID of the mesh sheet
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh sheet
    MeshSheet(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name);

    /// \brief Add a triangular MeshFace to the MeshSheet using indices
    ///
    /// \param i1,i2,i3 the point indices of the triangular face
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2,pv3 (optional) the ParamVertex objects associated with the points
    /// \param mapID whether to map the unique ID to the entity name
    virtual bool addFace(MLINT i1, MLINT i2, MLINT i3,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Find a MeshFace in the MeshSheet associativity data
    ///
    /// \param i1,i2,i3 the indices of the points in the desired triangular or quadrilateral face
    /// \param i4 (optinal) the index of the fourth point in the desired quadrilateral face
    virtual MeshFace *findFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED) const;

    /// \brief Delete a MeshFace from the MeshSheet associativity data
    ///
    /// \param i1,i2,i3 the point indices of the triangular or quadrilateral face to delete
    /// \param i4 (optinal) the index of the fourth point in the quadrilateral face to delete
    virtual void deleteFaceByInds(MLINT i1, MLINT i2, MLINT i3,
        MLINT i4 = MESH_TOPO_INDEX_UNUSED);

    /// \brief Add a triangular MeshFace to the MeshSheet using reference
    ///
    /// \param ref the application-defined reference data for the face
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2,pv3 (optional) the ParamVertex objects associated with the points
    /// \param mapID whether to map the unique ID to the entity name
    virtual bool addFace(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, bool mapID);

    /// \brief Add a quadrilateral MeshFace to the MeshSheet using indices
    ///
    /// \param i1,i2,i3,i4 the point indices of the face
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2,pv3,pv4 (optional) the ParamVertex objects associated with the face points
    /// \param mapID whether to map the unique ID to the entity name
    virtual bool addFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);

    /// \brief Add a quadrilateral MeshFace to the MeshSheet using reference
    ///
    /// \param ref the application-defined reference data for the face
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2,pv3,pv4 (optional) the ParamVertex objects associated with the face points
    /// \param mapID whether to map the unique ID to the entity name
    virtual bool addFace(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2,
        ParamVertex *pv3, ParamVertex *pv4, bool mapID);


    /// \brief Find a MeshFace by name
    ///
    /// \param name the name of the desired face
    virtual MeshFace * getMeshFaceByName(const std::string &name) const;

    /// \brief Find a MeshFace by reference
    ///
    /// \param ref the application-defined reference string of the desired face
    virtual MeshFace * getMeshFaceByRef(const std::string &ref) const;

    /// \brief Add a face-edge to the MeshSheet
    ///
    /// Face-edges are a MeshEdge using the MeshFace's
    /// associativity data.  This is useful when
    /// performing MeshEdge operations with MeshFace constraints.
    ///
    /// \param i1,i2 the point indices for the face-edge
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param pv1,pv2 (optional) the ParamVertex objects associated with the points
    virtual void addFaceEdge(MLINT i1, MLINT i2,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        ParamVertex *pv1 = NULL, ParamVertex *pv2 = NULL);

    /// \brief Find a face-edge in the associativity data
    ///
    /// \param i1,i2 the indices of the points in the desired face-edge
    virtual MeshEdge *findFaceEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Delete a face-edge from the associativity data
    ///
    /// \param i1,i2 the indices of the points in the face-edge to delete
    virtual void deleteFaceEdgeByInds(MLINT i1, MLINT i2);

    /// \brief Return the number of face-edges in the MeshSheet
    virtual MLINT getNumFaceEdges() const;

    /// \brief Return the number of MeshFaces in the MeshSheet
    virtual MLINT getNumFaces() const;

    /// \brief Return array of face-edges in the MeshSheet
    ///
    /// Face-edges are returned in creation order.
    virtual void getFaceEdges(std::vector<const MeshEdge *> &faceEdges) const;

    /// \brief Return array of MeshFaces in the MeshSheet
    ///
    /// Faces are returned in creation order.
    virtual void getMeshFaces(std::vector<const MeshFace *> &faces) const;

    /// Default constructor
    MeshSheet();
    /// Destructor
    ~MeshSheet();

    /// Return the base name used for generating unique names for sheets
    virtual const std::string &getBaseName() const;
    /// Return the current value used for generating unique names for sheets
    virtual MLUINT &getNameCounter();

private:
    /// The unique name counter for sheets
    static MLUINT nameCounter_;

    /// Map point hash values to points
    std::map<pwiFnvHash::FNVHash, MeshPoint*> pointMap_; // not the owner
    /// Map point names to points
    MeshPointNameMap        meshPointNameMap_;  // owner
    /// Map point IDs to point names
    MeshTopoIDToNameMap     meshPointIDToNameMap_;

    /// Map face edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> faceEdgeMap_;

    /// Map face indices hash to face
    std::map<pwiFnvHash::FNVHash, MeshFace*> faceMap_; // not the owner

    /// Map face names to face
    MeshFaceNameMap         meshFaceNameMap_;  // owner

    /// Map face IDs to face names
    MeshTopoIDToNameMap     meshFaceIDToNameMap_;

    /// Map application-defined face reference string to face name
    MeshTopoRefToNameMap    meshFaceRefToNameMap_;

    /// Serial counter as faceEdges are added to the sheet (zero-based)
    MLUINT faceEdgeCounter_;

    /// Serial counter as faces are added to the sheet (zero-based)
    MLUINT faceCounter_;
};

typedef std::map<std::string, MeshSheet *> MeshSheetNameMap;


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
