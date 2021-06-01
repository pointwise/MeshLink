/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "MeshAssociativity.h"
#include "MeshTopo.h"

#include <iostream>
#include <sstream>


/********************************************************************************
  Mesh Model Class
*********************************************************************************/
MLUINT MeshModel::nameCounter_ = 0;

const std::string &
MeshModel::getBaseName() const
{
    static const std::string base("ml_model-");
    return base;
};

MLUINT &
MeshModel::getNameCounter() {
    return nameCounter_;
};

MeshModel::MeshModel(
    std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name) :
    MeshTopo(ref, mid, aref, gref, name)
{
    setName(name);
};

MeshModel::~MeshModel()
{
    MeshPointNameMap::iterator piter;
    for (piter = meshPointNameMap_.begin(); piter != meshPointNameMap_.end(); ++piter) {
        delete piter->second;
    }

    // edge-points
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::iterator epiter;
    for (epiter = edgePointMap_.begin(); epiter != edgePointMap_.end(); ++epiter) {
        delete epiter->second;
    }

    // face-edge-points
    for (epiter = faceEdgePointMap_.begin(); epiter != faceEdgePointMap_.end(); ++epiter) {
        delete epiter->second;
    }


    MeshEdgeNameMap::iterator eiter;
    for (eiter = meshEdgeNameMap_.begin(); eiter != meshEdgeNameMap_.end(); ++eiter) {
        delete eiter->second;
    }

    // face-edges
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator feiter;
    for (feiter = faceEdgeMap_.begin(); feiter != faceEdgeMap_.end(); ++feiter) {
        delete feiter->second;
    }

    MeshFaceNameMap::iterator fiter;
    for (fiter = meshFaceNameMap_.begin(); fiter != meshFaceNameMap_.end(); ++fiter) {
        delete fiter->second;
    }

    MeshStringNameMap::iterator mstrIter;
    for (mstrIter = meshStringNameMap_.begin(); mstrIter != meshStringNameMap_.end(); ++mstrIter) {
        delete mstrIter->second;
    }
    meshStringNameMap_.clear();

    MeshSheetNameMap::iterator msIter;
    for (msIter = meshSheetNameMap_.begin(); msIter != meshSheetNameMap_.end(); ++msIter) {
        delete msIter->second;
    }
    meshSheetNameMap_.clear();
}


bool
MeshModel::addMeshString(MeshString* meshString, bool mapID)
{
    if (!meshString) { return false; }

    // ensure unique name for string
    meshString->setName(NULL);

    MeshString *existingString = getMeshStringByName(meshString->name_);
    if (existingString) {
        return false;
    }
    meshStringNameMap_[meshString->name_] = meshString;

    if (!meshString->getRef().empty()) {
        meshStringRefToNameMap_[meshString->getRef()] = meshString->getName();
    }

    if (mapID) {
        meshStringIDToNameMap_[meshString->mid_] = meshString->name_;
    }
    return true;
}

MeshString*
MeshModel::getMeshStringByID(MLINT id) const
{
    MeshTopoIDToNameMap::const_iterator it = meshStringIDToNameMap_.find(id);
    if (it == meshStringIDToNameMap_.end()) {
        return NULL;
    }
    return getMeshStringByName(it->second);
}

MeshString*
MeshModel::getMeshStringByName(const std::string &name) const
{
    MeshStringNameMap::const_iterator it = meshStringNameMap_.find(name);
    if (it == meshStringNameMap_.end()) {
        return NULL;
    }
    return it->second;
}

/// \brief Find a MeshString by reference
MeshString *
MeshModel::getMeshStringByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshStringRefToNameMap_.find(ref);
    if (iter == meshStringRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshStringByName(iter->second);
}


void
MeshModel::getMeshStrings(std::vector<MeshString *> &strings) const
{
    strings.clear();
    strings.resize(meshStringNameMap_.size());
    MeshStringNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshStringNameMap_.begin(); iter != meshStringNameMap_.end(); ++iter, ++i) {
        strings[i] = iter->second;
    }
    return;
}


size_t
MeshModel::getMeshStringCount() const
{
    return meshStringNameMap_.size();
}

bool
MeshModel::addMeshSheet(MeshSheet* meshSheet, bool mapID)
{
    if (!meshSheet) { return false; }

    // ensure unique name sheet
    meshSheet->setName(NULL);

    MeshSheet *existingSheet = getMeshSheetByName(meshSheet->name_);
    if (existingSheet) {
        return false;
    }
    meshSheetNameMap_[meshSheet->name_] = meshSheet;

    if (!meshSheet->getRef().empty()) {
        meshSheetRefToNameMap_[meshSheet->getRef()] = meshSheet->getName();
    }

    if (mapID) {
        meshSheetIDToNameMap_[meshSheet->mid_] = meshSheet->name_;
    }
    return true;
}

MeshSheet*
MeshModel::getMeshSheetByID(MLINT id) const
{
    MeshTopoIDToNameMap::const_iterator it = meshSheetIDToNameMap_.find(id);
    if (it == meshSheetIDToNameMap_.end()) {
        return NULL;
    }
    return getMeshSheetByName(it->second);
}

MeshSheet*
MeshModel::getMeshSheetByName(const std::string &name) const
{
    MeshSheetNameMap::const_iterator it = meshSheetNameMap_.find(name);
    if (it == meshSheetNameMap_.end()) {
        return NULL;
    }
    return it->second;
}

/// \brief Find a MeshSheet by reference
MeshSheet *
MeshModel::getMeshSheetByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshSheetRefToNameMap_.find(ref);
    if (iter == meshSheetRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshSheetByName(iter->second);
}


void
MeshModel::getMeshSheets(std::vector<MeshSheet *> &sheets) const
{
    sheets.clear();
    sheets.resize(meshSheetNameMap_.size());
    MeshSheetNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshSheetNameMap_.begin(); iter != meshSheetNameMap_.end(); ++iter, ++i) {
        sheets[i] = iter->second;
    }
    return;
}

size_t
MeshModel::getMeshSheetCount() const
{
    return meshSheetNameMap_.size();
}


/// \brief Find a MeshEdge by name
MeshEdge *
MeshModel::getMeshEdgeByName(const std::string &name) const
{
    MeshEdgeNameMap::const_iterator iter = meshEdgeNameMap_.find(name);
    if (iter == meshEdgeNameMap_.end()) {
        return NULL;
    }
    return iter->second;
}

/// \brief Find a MeshEdge by reference
MeshEdge *
MeshModel::getMeshEdgeByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshEdgeRefToNameMap_.find(ref);
    if (iter == meshEdgeRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshEdgeByName(iter->second);
}



/// \brief Find a MeshFace in the MeshModel using name
MeshFace *
MeshModel::getMeshFaceByName(const std::string &name) const
{
    MeshFaceNameMap::const_iterator iter = meshFaceNameMap_.find(name);
    if (iter == meshFaceNameMap_.end()) {
        return NULL;
    }
    return iter->second;
}

/// \brief Find a MeshFace in the MeshModel using reference
MeshFace *
MeshModel::getMeshFaceByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshFaceRefToNameMap_.find(ref);
    if (iter == meshFaceRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshFaceByName(iter->second);
}


/// \brief Return the number of edges in the MeshModel
//
///  Return will be the count of all MeshEdges from all MeshStrings
MLINT
MeshModel::getNumEdges() const
{
    return (MLINT)meshEdgeNameMap_.size();
}

/// \brief Return the number of faces in the MeshModel
//
///  Return will be the count of all MeshFaces from all MeshSheets
MLINT
MeshModel::getNumFaces() const
{
    return (MLINT)meshFaceNameMap_.size();
}



// Add an edge to the associativity data
bool
MeshModel::addPoint(MLINT i1,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, bool mapID)
{
    if (NULL != findPointByInd(i1)) {
        // point already stored
        return true;
    }

    MeshPoint *point = new MeshPoint(i1, mid, aref, gref, name, pv1);

    MeshTopo *existing = getMeshPointByName(point->name_);
    if (existing) {
        delete point;
        return false;
    }

    pointMap_[point->getHash()] = point; // not the owner
    meshPointNameMap_[point->getName()] = point;  // owner

    if (mapID) {
        meshPointIDToNameMap_[mid] = point->getName();
    }
    return true;
}

bool
MeshModel::addPoint(
    std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, bool mapID)
{
    MeshPoint *point = new MeshPoint(ref, mid, aref, gref, name, pv1);
    MeshTopo *existing = getMeshPointByName(point->name_);
    if (existing) {
        delete point;
        return false;
    }
    meshPointNameMap_[point->getName()] = point;  // owner

    meshPointRefToNameMap_[point->getRef()] = point->getName();

    if (mapID) {
        meshPointIDToNameMap_[mid] = point->getName();
    }
    return true;
}


MeshPoint*
MeshModel::getMeshPointByID(MLINT id) const
{
    MeshTopoIDToNameMap::const_iterator it = meshPointIDToNameMap_.find(id);
    if (it == meshPointIDToNameMap_.end()) {
        return NULL;
    }
    return getMeshPointByName(it->second);
}


MeshPoint *
MeshModel::getMeshPointByName(const std::string &name) const
{
    MeshPointNameMap::const_iterator iter = meshPointNameMap_.find(name);
    if (iter == meshPointNameMap_.end()) {
        return NULL;
    }
    return iter->second;
}


MeshPoint *
MeshModel::getMeshPointByRef(const std::string &ref) const
{
    MeshTopoRefToNameMap::const_iterator iter = meshPointRefToNameMap_.find(ref);
    if (iter == meshPointRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshPointByName(iter->second);
}


StringArray
MeshModel::getMeshPointRefs() const
{
    StringArray refs;
    refs.reserve(meshPointRefToNameMap_.size());
    MeshTopoRefToNameMap::const_iterator iter = meshPointRefToNameMap_.begin();
    for (; iter != meshPointRefToNameMap_.end(); ++iter) {
        refs.push_back(iter->first);
    }
    return refs;
}


// Add an edge to the associativity data
bool
MeshModel::addEdge(MLINT i1, MLINT i2,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, bool mapID)
{
    if (NULL != findEdgeByInds(i1, i2)) {
        // edge already stored
        return true;
    }

    MeshEdge *edge = new MeshEdge(i1, i2, mid, aref, gref, name, pv1, pv2);

    MeshTopo *existing = getMeshEdgeByName(edge->name_);
    if (existing) {
        delete edge;
        return false;
    }

    edgeMap_[edge->getHash()] = edge; // not the owner
    meshEdgeNameMap_[edge->getName()] = edge;  // owner

    if (mapID) {
        meshEdgeIDToNameMap_[mid] = edge->getName();
    }
    return true;
}

bool
MeshModel::addEdge(
    std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, bool mapID)
{
    MeshEdge *edge = new MeshEdge(ref, mid, aref, gref, name, pv1, pv2);
    MeshTopo *existing = getMeshEdgeByName(edge->name_);
    if (existing) {
        delete edge;
        return false;
    }
    meshEdgeNameMap_[edge->getName()] = edge;  // owner

    meshEdgeRefToNameMap_[edge->getRef()] = edge->getName();

    if (mapID) {
        meshEdgeIDToNameMap_[mid] = edge->getName();
    }
    return true;
}

void
MeshModel::getMeshEdges(std::vector<const MeshEdge *> &edges) const
{
    edges.clear();
    edges.resize(meshEdgeNameMap_.size());
    MeshEdgeNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshEdgeNameMap_.begin(); iter != meshEdgeNameMap_.end(); ++iter, ++i) {
        edges[i] = iter->second;
    }
    return;
}


void
MeshModel::addEdgePoint(MLINT i1,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    ParamVertex *pv1)
{
    if (NULL != findEdgePointByInd(i1)) {
        // edge-point already stored
        return;
    }
    // edge-points don't have names
    std::string name;
    MeshPoint *point = new MeshPoint(i1, mid, aref, gref, name, pv1);
    edgePointMap_[point->getHash()] = point;
}

void
MeshModel::addFaceEdgePoint(MLINT i1,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    ParamVertex *pv1)
{
    if (NULL != findFaceEdgePointByInd(i1)) {
        // face-edge-point already stored
        return;
    }
    // edge-points don't have names
    std::string name;
    MeshPoint *point = new MeshPoint(i1, mid, aref, gref, name, pv1);
    faceEdgePointMap_[point->getHash()] = point;
}


// Delete a point from the associativity data
void
MeshModel::deletePointByInd(MLINT i1)
{
    pwiFnvHash::FNVHash hash = MeshPoint::computeHash(i1);
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::iterator iter;
    iter = pointMap_.find(hash);
    MeshPoint *point = NULL;
    if (iter != pointMap_.end()) {
        // found Point
        point = iter->second;
        pointMap_.erase(iter);
    }
    // Look in MeshEdgePoint list
    iter = edgePointMap_.find(hash);
    if (iter != edgePointMap_.end()) {
        // found EdgePoint
        point = iter->second;
        edgePointMap_.erase(iter);
    }
    // Look in MeshFaceEdgePoint list
    iter = faceEdgePointMap_.find(hash);
    if (iter != faceEdgePointMap_.end()) {
        // found FaceEdgePoint
        point = iter->second;
        faceEdgePointMap_.erase(iter);
    }
    if (NULL == point) {
        // not found in point map
        return;
    }

    MeshTopoRefToNameMap::iterator refIter = meshPointRefToNameMap_.find(point->getRef());
    if (refIter != meshPointRefToNameMap_.end()) {
        meshPointRefToNameMap_.erase(refIter);
    }

    MeshTopoIDToNameMap::iterator idIter = meshPointIDToNameMap_.find(point->getID());
    if (idIter != meshPointIDToNameMap_.end()) {
        meshPointIDToNameMap_.erase(idIter);
    }

    // nameMap is the owner
    MeshPointNameMap::iterator nameIter = meshPointNameMap_.find(point->getName());
    if (nameIter != meshPointNameMap_.end()) {
        delete nameIter->second;
        meshPointNameMap_.erase(nameIter);
    }
}

void MeshModel::deletePointByRef(MLINT iref)
{
    std::stringstream ss;
    ss << iref;
    if (!ss.str().empty()) {
        deletePointByRef(ss.str());
    }
}

void
MeshModel::deletePointByRef(const std::string &ref)
{
    MeshPoint *point = NULL;
    std::string name;
    MeshTopoRefToNameMap::iterator refIter = meshPointRefToNameMap_.find(ref);
    if (refIter != meshPointRefToNameMap_.end()) {
        name = refIter->second;
        meshPointRefToNameMap_.erase(refIter);

        // nameMap is the owner
        MeshPointNameMap::iterator nameIter = meshPointNameMap_.find(name);
        if (nameIter != meshPointNameMap_.end()) {
            point = nameIter->second;
            meshPointNameMap_.erase(nameIter);
        }

        if (point) {
            pwiFnvHash::FNVHash hash = point->getHash();
            std::map<pwiFnvHash::FNVHash, MeshPoint*>::iterator iter;
            iter = pointMap_.find(hash);
            if (iter != pointMap_.end()) {
                // found Point
                pointMap_.erase(iter);
            }
            // Look in MeshEdgePoint list
            iter = edgePointMap_.find(hash);
            if (iter != edgePointMap_.end()) {
                // found EdgePoint
                edgePointMap_.erase(iter);
            }
            // Look in MeshFaceEdgePoint list
            iter = faceEdgePointMap_.find(hash);
            if (iter != faceEdgePointMap_.end()) {
                // found FaceEdgePoint
                faceEdgePointMap_.erase(iter);
            }

            MeshTopoIDToNameMap::iterator idIter = meshPointIDToNameMap_.find(point->getID());
            if (idIter != meshPointIDToNameMap_.end()) {
                meshPointIDToNameMap_.erase(idIter);
            }

            delete point;
        }
    }
}



// Find point belonging to MeshEdge, MeshFace, or MeshPointReference in the associativity data
// In that order.
MeshPoint *
MeshModel::findLowestTopoPointByInd(MLINT i1) const
{
    MeshPoint *point = NULL;

    // Look in MeshEdge list (MeshString)
    point = findEdgePointByInd(i1);
    if (NULL != point) return point;

    // Look in MeshFaceEdge list (MeshSheet)
    point = findFaceEdgePointByInd(i1);
    if (NULL != point) return point;

    // Look in MeshPoint list last (MeshModel)
    point = findPointByInd(i1);
    if (NULL != point) return point;

    return point;
}

// Find point belonging to MeshPointReference, MeshFace, or MeshEdge in the associativity data
// In that order.
MeshPoint *
MeshModel::findHighestTopoPointByInd(MLINT i1) const
{
    MeshPoint *point = NULL;
    // Look in MeshPoint list last (MeshModel)
    point = findPointByInd(i1);
    if (NULL != point) return point;

    // Look in MeshFaceEdge list (MeshSheet)
    point = findFaceEdgePointByInd(i1);
    if (NULL != point) return point;

    // Look in MeshEdge list (MeshString)
    point = findEdgePointByInd(i1);
    if (NULL != point) return point;

    return point;
}

// Find an point in the associativity data
// MeshPoint and MeshPointReference (MeshModel topology level)
MeshPoint *
MeshModel::findPointByInd(MLINT i1) const
{
    pwiFnvHash::FNVHash hash = MeshPoint::computeHash(i1);
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::const_iterator iter;

    // Look in MeshPoint list
    iter = pointMap_.find(hash);
    if (iter != pointMap_.end()) {
        return iter->second;
    }

    // Currently, we really only allow MeshPoint creation by ref
    // so attempt a lookup using the index as ref
    std::ostringstream s;
    s << i1;
    const std::string ref(s.str());
    return getMeshPointByRef(ref);
}


// Find a edge-point in the associativity data
// MeshString topology level
MeshPoint *
MeshModel::findEdgePointByInd(MLINT i1) const
{
    pwiFnvHash::FNVHash hash = MeshPoint::computeHash(i1);
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::const_iterator iter;

    // Look in MeshEdgePoint list
    iter = edgePointMap_.find(hash);
    if (iter != edgePointMap_.end()) {
        return iter->second;
    }
    return NULL;
}

// Find a face-edge-point in the associativity data
// MeshSheet topology level
MeshPoint *
MeshModel::findFaceEdgePointByInd(MLINT i1) const
{
    pwiFnvHash::FNVHash hash = MeshPoint::computeHash(i1);
    std::map<pwiFnvHash::FNVHash, MeshPoint*>::const_iterator iter;

    // Look in MeshFaceEdgePoint list
    iter = faceEdgePointMap_.find(hash);
    if (iter != faceEdgePointMap_.end()) {
        return iter->second;
    }
    return NULL;
}



// Add a face edge to the associativity data
void
MeshModel::addFaceEdge(MLINT i1, MLINT i2,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    ParamVertex *pv1, ParamVertex *pv2)
{
    if (NULL != findFaceEdgeByInds(i1, i2)) {
        // face-edge already stored
        return;
    }
    // face edges don't have names
    std::string name;
    MeshEdge *edge = new MeshEdge(i1, i2, mid, aref, gref, name, pv1, pv2);
    faceEdgeMap_[edge->getHash()] = edge;
}


// Find edge belonging to MeshString or MeshFace in the associativity data
MeshEdge *
MeshModel::findLowestTopoEdgeByInds(MLINT i1, MLINT i2) const
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::const_iterator iter;

    // Look in MeshEdge list (MeshString) first
    MeshEdge *edge = findEdgeByInds(i1, i2);
    if (NULL != edge) return edge;

    // Look in MeshFaceEdge list (MeshSheet)
    return findFaceEdgeByInds(i1, i2);
}

// Find an edge in the associativity data
MeshEdge *
MeshModel::findEdgeByInds(MLINT i1, MLINT i2) const
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::const_iterator iter;

    // Look in MeshEdge list first
    iter = edgeMap_.find(hash);
    if (iter != edgeMap_.end()) {
        return iter->second;
    }
    return NULL;
}

// Find a face-edge in the associativity data
MeshEdge *
MeshModel::findFaceEdgeByInds(MLINT i1, MLINT i2) const
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::const_iterator iter;

    // Look in MeshFaceEdge list
    iter = faceEdgeMap_.find(hash);
    if (iter != faceEdgeMap_.end()) {
        return iter->second;
    }
    return NULL;
}

// Delete an edge from the associativity data
void
MeshModel::deleteEdgeByInds(MLINT i1, MLINT i2)
{
    pwiFnvHash::FNVHash hash = MeshEdge::computeHash(i1, i2);
    std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator iter;
    iter = edgeMap_.find(hash);
    MeshEdge *edge = NULL;
    if (iter != edgeMap_.end()) {
        // found Edge
        edge = iter->second;
        edgeMap_.erase(iter);
    }
    // Look in MeshFaceEdge list
    iter = faceEdgeMap_.find(hash);
    if (iter != faceEdgeMap_.end()) {
        // found FaceEdge
        edge = iter->second;
        faceEdgeMap_.erase(iter);
    }
    if (NULL == edge) {
        // not found in either edge map
        return;
    }

    MeshTopoRefToNameMap::iterator refIter = meshEdgeRefToNameMap_.find(edge->getRef());
    if (refIter != meshEdgeRefToNameMap_.end()) {
        meshEdgeRefToNameMap_.erase(refIter);
    }

    MeshTopoIDToNameMap::iterator idIter = meshEdgeIDToNameMap_.find(edge->getID());
    if (idIter != meshEdgeIDToNameMap_.end()) {
        meshEdgeIDToNameMap_.erase(idIter);
    }

    // nameMap is the owner
    MeshEdgeNameMap::iterator nameIter = meshEdgeNameMap_.find(edge->getName());
    if (nameIter != meshEdgeNameMap_.end()) {
        delete nameIter->second;
        meshEdgeNameMap_.erase(nameIter);
    }
}


void
MeshModel::deleteEdgeByRef(MLINT iref)
{
    std::stringstream ss;
    ss << iref;
    if (!ss.str().empty()) {
        deleteEdgeByRef(ss.str());
    }
}


void
MeshModel::deleteEdgeByRef(const std::string &ref)
{
    MeshEdge *edge = NULL;
    std::string name;
    MeshTopoRefToNameMap::iterator refIter = meshEdgeRefToNameMap_.find(ref);
    if (refIter != meshEdgeRefToNameMap_.end()) {
        name = refIter->second;
        meshEdgeRefToNameMap_.erase(refIter);

        // nameMap is the owner
        MeshEdgeNameMap::iterator nameIter = meshEdgeNameMap_.find(name);
        if (nameIter != meshEdgeNameMap_.end()) {
            edge = nameIter->second;
            meshEdgeNameMap_.erase(nameIter);
        }

        if (edge) {
            pwiFnvHash::FNVHash hash = edge->getHash();
            std::map<pwiFnvHash::FNVHash, MeshEdge*>::iterator iter;
            iter = edgeMap_.find(hash);
            if (iter != edgeMap_.end()) {
                // found Edge
                edgeMap_.erase(iter);
            }
            // Look in MeshFaceEdge list
            iter = faceEdgeMap_.find(hash);
            if (iter != faceEdgeMap_.end()) {
                // found FaceEdge
                faceEdgeMap_.erase(iter);
            }

            MeshTopoIDToNameMap::iterator idIter = meshEdgeIDToNameMap_.find(edge->getID());
            if (idIter != meshEdgeIDToNameMap_.end()) {
                meshEdgeIDToNameMap_.erase(idIter);
            }

            delete edge;
        }
    }
}


// Add a face to the associativity data
bool
MeshModel::addFace(MLINT i1, MLINT i2, MLINT i3,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, bool mapID)
{
    if (NULL != findFaceByInds(i1, i2, i3)) {
        // face already exists
        return true;
    }
    MeshFace *face = new MeshFace(i1, i2, i3, mid, aref,
        gref, name, pv1, pv2, pv3);

    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    faceMap_[face->getHash()] = face; // not the owner
    meshFaceNameMap_[face->getName()] = face;  // owner
    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }
    return true;
}

bool
MeshModel::addFace(
    std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, bool mapID)
{
    MeshFace *face = new MeshFace(ref, mid, aref,
        gref, name, pv1, pv2, pv3);

    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    meshFaceNameMap_[face->getName()] = face;  // owner

    if (!face->getRef().empty()) {
        meshFaceRefToNameMap_[face->getRef()] = face->getName();
    }

    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }
    return true;
}


bool
MeshModel::addFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, ParamVertex *pv4,
    bool mapID)
{
    if (NULL != findFaceByInds(i1, i2, i3, i4)) {
        // face already exists
        return true;
    }

    MeshFace *face = new MeshFace(i1, i2, i3, i4, mid, aref,
        gref, name, pv1, pv2, pv3, pv4);
    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    faceMap_[face->getHash()] = face; // not the owner
    meshFaceNameMap_[face->getName()] = face;  // owner
    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }
    return true;
}

bool
MeshModel::addFace(
    std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    std::string &name,
    ParamVertex *pv1, ParamVertex *pv2, ParamVertex *pv3, ParamVertex *pv4,
    bool mapID)
{
    MeshFace *face = new MeshFace(ref, mid, aref,
        gref, name, pv1, pv2, pv3, pv4);
    MeshTopo *existing = getMeshFaceByName(face->name_);
    if (existing) {
        delete face;
        return false;
    }

    meshFaceNameMap_[face->getName()] = face;  // owner

    if (!face->getRef().empty()) {
        meshFaceRefToNameMap_[face->getRef()] = face->getName();
    }

    if (mapID) {
        meshFaceIDToNameMap_[mid] = face->getName();
    }
    return true;
}

void
MeshModel::getMeshFaces(std::vector<const MeshFace *> &faces) const
{
    faces.clear();
    faces.resize(meshFaceNameMap_.size());
    MeshFaceNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshFaceNameMap_.begin(); iter != meshFaceNameMap_.end(); ++iter, ++i) {
        faces[i] = iter->second;
    }
    return;
}


// Find a face in the associativity data
MeshFace *
MeshModel::findFaceByInds(MLINT i1, MLINT i2, MLINT i3, MLINT i4)
{
    pwiFnvHash::FNVHash hash = MeshFace::computeHash(i1, i2, i3, i4);
    std::map<pwiFnvHash::FNVHash, MeshFace*>::iterator iter;
    iter = faceMap_.find(hash);
    if (iter != faceMap_.end()) {
        return iter->second;
    }
    // unfound
    return NULL;
}


// Delete a face from the associativity data
void
MeshModel::deleteFaceByInds(MLINT i1, MLINT i2, MLINT i3, MLINT i4)
{
    pwiFnvHash::FNVHash hash = MeshFace::computeHash(i1, i2, i3, i4);
    std::map<pwiFnvHash::FNVHash, MeshFace*>::iterator iter;
    iter = faceMap_.find(hash);
    if (iter == faceMap_.end()) return;
    MeshFace *face = iter->second;
    faceMap_.erase(iter);

    MeshTopoRefToNameMap::iterator refIter = meshFaceRefToNameMap_.find(face->getRef());
    if (refIter != meshFaceRefToNameMap_.end()) {
        meshFaceRefToNameMap_.erase(refIter);
    }

    MeshTopoIDToNameMap::iterator idIter = meshFaceIDToNameMap_.find(face->getID());
    if (idIter != meshFaceIDToNameMap_.end()) {
        meshFaceIDToNameMap_.erase(idIter);
    }

    // nameMap is the owner
    MeshFaceNameMap::iterator nameIter = meshFaceNameMap_.find(face->getName());
    if (nameIter != meshFaceNameMap_.end()) {
        delete nameIter->second;
        meshFaceNameMap_.erase(nameIter);
    }
}

void
MeshModel::deleteFaceByRef(MLINT iref)
{
    std::stringstream ss;
    ss << iref;
    if (!ss.str().empty()) {
        deleteFaceByRef(ss.str());
    }
}

void
MeshModel::deleteFaceByRef(const std::string &ref)
{
    std::string name;
    MeshTopoRefToNameMap::iterator refIter = meshFaceRefToNameMap_.find(ref);
    if (refIter != meshFaceRefToNameMap_.end()) {
        name = refIter->second;
        meshFaceRefToNameMap_.erase(refIter);
    }

    // nameMap is the owner
    MeshFace *face = NULL;
    MeshFaceNameMap::iterator nameIter = meshFaceNameMap_.find(name);
    if (nameIter != meshFaceNameMap_.end()) {
        face = nameIter->second;
        meshFaceNameMap_.erase(nameIter);
    }

    if (face) {
        std::map<pwiFnvHash::FNVHash, MeshFace*>::iterator iter;
        iter = faceMap_.find(face->getHash());
        if (iter != faceMap_.end()) {
            faceMap_.erase(iter);
        }

        MeshTopoIDToNameMap::iterator idIter = meshFaceIDToNameMap_.find(face->getID());
        if (idIter != meshFaceIDToNameMap_.end()) {
            meshFaceIDToNameMap_.erase(idIter);
        }

        delete face;
    }
}

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
