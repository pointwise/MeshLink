/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

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

    /// \brief Constructor with no application-defined reference data
    ///
    /// \param mid unique ID of the mesh string
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh string
    MeshString(
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name);

    /// \brief Constructor with application-defined reference data
    ///
    /// \param ref application-defined reference data for the mesh string
    /// \param mid unique ID of the mesh string
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh string
    MeshString(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name);

    /// \brief Add a MeshEdge to the MeshString using indices
    ///
    /// \param i1,i2 the point indices of the string edge
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2 (optional) the ParamVertex objects associated with the points
    /// \param mapID whether to map the unique ID to the entity name
    virtual bool addEdge(MLINT i1, MLINT i2, 
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2, bool mapID);

    /// \brief Add a MeshEdge to the MeshString using reference
    ///
    /// \param ref the application-defined reference of the mesh entity
    /// \param mid unique ID of the mesh entity
    /// \param aref the attribute reference ID (AttID) (optional)
    /// \param gref the geometry reference ID
    /// \param name the name of the mesh entity
    /// \param pv1,pv2 (optional) the ParamVertex objects associated with the points
    /// \param mapID whether to map the unique ID to the entity name
    virtual bool addEdge(
        const std::string &ref,
        MLINT mid,
        MLINT aref,
        MLINT gref,
        const std::string &name,
        ParamVertex *pv1, ParamVertex *pv2, bool mapID);

    /// \brief Find a MeshEdge in the MeshString associativity data
    ///
    /// \param i1,i2 the point indices of the desired string edge
    virtual MeshEdge *findEdgeByInds(MLINT i1, MLINT i2) const;

    /// \brief Delete a MeshEdge from the MeshString associativity data
    ///
    /// \param i1,i2 the point indices of the string edge to delete
    virtual void deleteEdgeByInds(MLINT i1, MLINT i2);

    /// \brief Find a MeshEdge by name
    ///
    /// \param name the name of the desired string edge
    virtual MeshEdge * getMeshEdgeByName(const std::string &name) const;

    /// \brief Find a MeshEdge by reference
    //
    /// \param ref the application-defined reference string of the desired string edge
    virtual MeshEdge * getMeshEdgeByRef(const std::string &ref) const;

    /// \brief Return the number of MeshEdges in the MeshString
    virtual MLINT getNumEdges() const;

    /// \brief Return array of MeshEdges in the MeshString
    ///
    /// Edges are returned in creation order.
    virtual void getMeshEdges(std::vector<const MeshEdge *> &edges) const;

    /// Default constructor
    MeshString();
    /// Destructor
    ~MeshString();

    /// Return the base name used for generating unique names for strings
    virtual const std::string &getBaseName() const;
    /// Return the current value used for generating unique names for strings
    virtual MLUINT &getNameCounter();

private:
    /// The unique name counter for strings
    static MLUINT nameCounter_;

    /// Map point index hash to point
    std::map<pwiFnvHash::FNVHash, MeshPoint*> pointMap_;  // not the owner
    /// Map point name to point
    MeshPointNameMap        meshPointNameMap_;  // owner
    /// Map point ID to point name
    MeshTopoIDToNameMap     meshPointIDToNameMap_;

    /// Map edge indices hash to edge
    std::map<pwiFnvHash::FNVHash, MeshEdge*> edgeMap_; // not the owner
    /// Map edge name to edge
    MeshEdgeNameMap         meshEdgeNameMap_;  // owner
    /// Map edge unique ID to edge name
    MeshTopoIDToNameMap     meshEdgeIDToNameMap_;
    /// Map edge application-defined reference string th edge name
    MeshTopoRefToNameMap    meshEdgeRefToNameMap_;

    /// Serial counter as edges are added to the string (zero-based)
    MLUINT edgeCounter_;
};

typedef std::map<std::string, MeshString *> MeshStringNameMap;

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
