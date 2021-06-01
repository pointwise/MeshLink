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
const pwiFnvHash::FNVHash pwiFnvHash::fnv_num_octets_ = sizeof(void*);
#if defined(IS64BIT)
const pwiFnvHash::FNVHash pwiFnvHash::fnv_init_ = FNV1_64_INIT;
const pwiFnvHash::FNVHash pwiFnvHash::fnv_prime_ = FNV1_64_PRIME;
#else
const pwiFnvHash::FNVHash pwiFnvHash::fnv_init_ = FNV1_32_INIT;
const pwiFnvHash::FNVHash pwiFnvHash::fnv_prime_ = FNV1_32_PRIME;
#endif

pwiFnvHash::FNVHash
pwiFnvHash::hashInit() {
    return pwiFnvHash::fnv_init_;
}

//! Add an integer to the developing hash
pwiFnvHash::FNVHash
pwiFnvHash::hash(MLINT data, FNVHash hash) {
    unsigned char *bp = (unsigned char *)&data;
    unsigned char *be = bp + pwiFnvHash::fnv_num_octets_;
    while (bp < be) {
        hash ^= (pwiFnvHash::FNVHash) (*bp++);
        hash *= pwiFnvHash::fnv_prime_;
    }
    return hash;
}

/****************************************************************************
 * MeshTopo Base Class
 ***************************************************************************/

MLUINT MeshTopo::nameCounter_ = 0;

MeshTopo::MeshTopo() :
    mid_(MESH_TOPO_INVALID_REF),
    aref_(MESH_TOPO_INVALID_REF),
    gref_(MESH_TOPO_INVALID_REF)
{};

MeshTopo::MeshTopo(
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name) :
    mid_(mid),
    aref_(aref),
    gref_(gref),
    name_(name)
{
    if (name_.empty() && !ref_.empty()) {
        name_ = ref_;
    }
};

MeshTopo::MeshTopo(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name) :
    ref_(ref),
    mid_(mid),
    aref_(aref),
    gref_(gref),
    name_(name)
{
    if (name_.empty() && !ref_.empty()) {
        name_ = ref_;
    }
};

MeshTopo::~MeshTopo() {
    ParamVertVrefMap::iterator pviter;
    for (pviter = paramVertVrefMap_.begin(); pviter != paramVertVrefMap_.end(); ++pviter) {
        delete pviter->second;
    }
    paramVertVrefMap_.clear();
    paramVertIDToVrefMap_.clear();
};

std::string 
MeshTopo::getNextName()
{
    std::string basename = getBaseName();
    MLUINT &counter = getNameCounter();
    counter++;
    std::ostringstream s;  s << counter;
    std::string name = basename + s.str();
    return name;
}

const std::string &
MeshTopo::getBaseName() const
{ 
    static const std::string base("ml_topo-"); 
    return base;
};

MLUINT &
MeshTopo::getNameCounter() {
    return nameCounter_;
};


void 
MeshTopo::addParamVertex(ParamVertex *pv, bool mapID)
{
    if (pv) {
        paramVertVrefMap_[pv->getVref()] = pv;
        if (mapID) {
            paramVertIDToVrefMap_[pv->getID()] = pv->getVref();
        }
    }
}

ParamVertex *
MeshTopo::getParamVertByVref(const std::string & vref) const
{
    ParamVertVrefMap::const_iterator pviter;
    pviter = paramVertVrefMap_.find(vref);
    if (pviter != paramVertVrefMap_.end()) {
        return pviter->second;
    }
    return NULL;
}

ParamVertex *
MeshTopo::getParamVertByID(MLINT id) const
{
    ParamVertIDToVrefMap::const_iterator pviter;
    pviter = paramVertIDToVrefMap_.find(id);
    if (pviter == paramVertIDToVrefMap_.end()) {
        return NULL;
    }

    ParamVertVrefMap::const_iterator pviter2;
    pviter2 = paramVertVrefMap_.find(pviter->second);
    if (pviter2 != paramVertVrefMap_.end()) {
        return pviter2->second;
    }
    ML_assert(0 == "missing ParamVertex");
    return NULL;
}

MLINT 
MeshTopo::getNumParamVerts() const
{
    return (MLINT)paramVertVrefMap_.size();
}

void
MeshTopo::getParamVerts(std::vector<const ParamVertex *> &pv) const
{
    pv.resize(paramVertVrefMap_.size());
    ParamVertVrefMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = paramVertVrefMap_.begin(); iter != paramVertVrefMap_.end(); ++iter, ++i) {
        pv[i] = iter->second;
    }
}

const ParamVertVrefMap & 
MeshTopo::getParamVertVrefMap() const {
    return paramVertVrefMap_;
}


MLINT
MeshTopo::getID() const
{
    return mid_;
}

MLINT
MeshTopo::getGref() const
{
    return gref_;
}

MLINT
MeshTopo::getAref() const
{
    return aref_;
}

void
MeshTopo::getName(const char **name) const
{
    *name = name_.c_str();
}

const std::string &
MeshTopo::getName() const
{
    return name_;
}

const std::string & 
MeshTopo::getRef() const
{ 
    return ref_; 
}


void
MeshTopo::setID( MLINT id ) { mid_ = id;  }
void
MeshTopo::setGref( MLINT gref ) { gref_ = gref;  }
void
MeshTopo::setAref( MLINT aref ) { aref_ = aref; }
void
MeshTopo::setName(const std::string &name) { 
    if (name.empty()) {
        if (name_.empty()) {
            // generate new unique name
            name_ = getNextName();
        }
    }
    else {
        name_ = name;
    }
}
void
MeshTopo::setName(const char *nameCstr) {
    std::string name;
    if (nameCstr) {
        name = nameCstr;
    }
    setName(name);
}

void
MeshTopo::setRef(const char *ref) { ref_ = ref; }

bool
MeshTopo::hasID() const {
    return (mid_ != MESH_TOPO_INVALID_REF);
}
bool
MeshTopo::hasGref() const {
    return (gref_ != MESH_TOPO_INVALID_REF);
}
bool 
MeshTopo::hasAref() const { 
    return (aref_ != MESH_TOPO_INVALID_REF); 
}

std::vector<MLINT> 
MeshTopo::getAttributeIDs(const MeshAssociativity &meshAssoc) const
{
    std::vector<MLINT> ref_attids;
    if (!hasAref()) {
        return ref_attids;
    }
    MLINT aref = getAref();
    const MeshLinkAttribute *att = meshAssoc.getAttributeByID(aref);
    if (NULL == att) { return ref_attids; }

    if (att->isGroup()) {
        ref_attids = att->getAttributeIDs();
    }
    else {
        ref_attids.push_back(aref);
    }
    return ref_attids;
}

void 
MeshTopo::setOrderCounter(MLUINT counter)
{
    orderCounter_ = counter;
}

MLUINT
MeshTopo::getOrderCounter() const
{
    return orderCounter_;
}

bool 
MeshTopo::OrderCompare(const MeshTopo *topo1, const MeshTopo *topo2)
{
    return (topo1->getOrderCounter() < topo2->getOrderCounter());
}


/***************************************************************************
 * Mesh Point Class
 ***************************************************************************/
MLUINT MeshPoint::nameCounter_ = 0;

const std::string &
MeshPoint::getBaseName() const
{
    static const std::string base("ml_point-");
    return base;
};

MLUINT &
MeshPoint::getNameCounter() {
    return nameCounter_;
};

MeshPoint::MeshPoint(MLINT i1,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1
) :
    MeshTopo(mid, aref, gref, name),
    i1_(i1)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    paramVert_ = NULL;
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVert_ = new ParamVertex(*pv1);
    }
}

MeshPoint::MeshPoint(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1
) :
    MeshTopo(ref, mid, aref, gref, name),
    i1_(MESH_TOPO_INDEX_UNUSED)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    paramVert_ = NULL;
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVert_ = new ParamVertex(*pv1);
    }
}

MeshPoint::~MeshPoint()
{
    if (NULL != paramVert_) {
        delete paramVert_; paramVert_ = NULL;
    }
}

pwiFnvHash::FNVHash 
MeshPoint::computeHash(MLINT i1) {
    // hash equals the point index
    pwiFnvHash::FNVHash hash = (pwiFnvHash::FNVHash)i1;
    return hash;
}

pwiFnvHash::FNVHash 
MeshPoint::getHash() const {
    return computeHash(i1_);
}


/****************************************************************************
 * Mesh Edge Class
 ***************************************************************************/
MLUINT MeshEdge::nameCounter_ = 0;

const std::string &
MeshEdge::getBaseName() const
{
    static const std::string base("ml_edge-");
    return base;
};

MLUINT &
MeshEdge::getNameCounter() {
    return nameCounter_;
};

MeshEdge::MeshEdge(MLINT i1, MLINT i2,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2) :
    MeshTopo(mid, aref, gref, name),
    i1_(i1),
    i2_(i2)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    int n;
    for (n = 0; n < 2; ++n) {
        paramVerts_[n] = NULL;
    }
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVerts_[0] = new ParamVertex(*pv1);
    }
    if (pv2) {
        paramVerts_[1] = new ParamVertex(*pv2);
    }
}


MeshEdge::MeshEdge(const MeshEdge &other)
{
    i1_ = other.i1_;
    i2_ = other.i2_;
    mid_ = other.mid_;
    aref_ = other.aref_;
    gref_ = other.gref_;
    name_ = other.name_;
    int n;
    for (n = 0; n < 2; ++n) {
        paramVerts_[n] = NULL;
        // make our own copy of the PV data in case owner goes away
        if (other.paramVerts_[n]) {
            paramVerts_[n] = new ParamVertex(*(other.paramVerts_[n]));
        }
    }
}

MeshEdge & 
MeshEdge::operator=(const MeshEdge &other)
{
    i1_ = other.i1_;
    i2_ = other.i2_;
    mid_ = other.mid_;
    aref_ = other.aref_;
    gref_ = other.gref_;
    name_ = other.name_;
    int n;
    for (n = 0; n < 2; ++n) {
        paramVerts_[n] = NULL;
        // make our own copy of the PV data in case owner goes away
        if (other.paramVerts_[n]) {
            paramVerts_[n] = new ParamVertex(*(other.paramVerts_[n]));
        }
    }
    return *this;
}

MeshEdge::MeshEdge(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2) :
    MeshTopo(ref, mid, aref, gref, name),
    i1_(MESH_TOPO_INDEX_UNUSED),
    i2_(MESH_TOPO_INDEX_UNUSED)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    int n;
    for (n = 0; n < 2; ++n) {
        paramVerts_[n] = NULL;
    }
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVerts_[0] = new ParamVertex(*pv1);
    }
    if (pv2) {
        paramVerts_[1] = new ParamVertex(*pv2);
    }
}

MeshEdge::~MeshEdge()
{
    for (int n = 0; n < 2; ++n) {
        if (NULL != paramVerts_[n]) {
            delete paramVerts_[n]; paramVerts_[n] = NULL;
        }
    }
}

void MeshEdge::getInds(MLINT *inds, MLINT *numInds) const {
    inds[0] = i1_;
    inds[1] = i2_;
    *numInds = 0;
    if (i1_ != MESH_TOPO_INDEX_UNUSED) ++(*numInds);
    if (i2_ != MESH_TOPO_INDEX_UNUSED) ++(*numInds);
}

pwiFnvHash::FNVHash 
MeshEdge::getHash() const {
    return computeHash(i1_, i2_);
}

pwiFnvHash::FNVHash 
MeshEdge::computeHash(MLINT i1, MLINT i2) {
    pwiFnvHash::FNVHash hash = pwiFnvHash::hashInit();
    if (i1 < i2) {
        hash = pwiFnvHash::hash(i1, hash);
        hash = pwiFnvHash::hash(i2, hash);
    }
    else {
        hash = pwiFnvHash::hash(i2, hash);
        hash = pwiFnvHash::hash(i1, hash);
    }
    return hash;
}



/****************************************************************************
 * Mesh Face Class
 ***************************************************************************/
MLUINT MeshFace::nameCounter_ = 0;

const std::string &
MeshFace::getBaseName() const
{
    static const std::string base("ml_face-");
    return base;
};

MLUINT &
MeshFace::getNameCounter() {
    return nameCounter_;
};

MeshFace::MeshFace(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2,
    ParamVertex *pv3) :
    MeshTopo(ref, mid, aref, gref, name),
    i1_(MESH_TOPO_INDEX_UNUSED),
    i2_(MESH_TOPO_INDEX_UNUSED),
    i3_(MESH_TOPO_INDEX_UNUSED),
    i4_(MESH_TOPO_INDEX_UNUSED)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    int n;
    for (n = 0; n < 4; ++n) {
        paramVerts_[n] = NULL;
    }
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVerts_[0] = new ParamVertex(*pv1);
    }
    if (pv2) {
        paramVerts_[1] = new ParamVertex(*pv2);
    }
    if (pv3) {
        paramVerts_[2] = new ParamVertex(*pv3);
    }
}

MeshFace::MeshFace(MLINT i1, MLINT i2, MLINT i3,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2,
    ParamVertex *pv3) :
    MeshTopo(mid, aref, gref, name),
    i1_(i1),
    i2_(i2),
    i3_(i3),
    i4_(MESH_TOPO_INDEX_UNUSED)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    int n;
    for (n = 0; n < 4; ++n) {
        paramVerts_[n] = NULL;
    }
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVerts_[0] = new ParamVertex(*pv1);
    }
    if (pv2) {
        paramVerts_[1] = new ParamVertex(*pv2);
    }
    if (pv3) {
        paramVerts_[2] = new ParamVertex(*pv3);
    }
}

MeshFace::MeshFace(MLINT i1, MLINT i2, MLINT i3, MLINT i4,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2,
    ParamVertex *pv3, ParamVertex *pv4) :
    MeshTopo(mid, aref, gref, name),
    i1_(i1),
    i2_(i2),
    i3_(i3),
    i4_(i4)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    int n;
    for (n = 0; n < 4; ++n) {
        paramVerts_[n] = NULL;
    }
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVerts_[0] = new ParamVertex(*pv1);
    }
    if (pv2) {
        paramVerts_[1] = new ParamVertex(*pv2);
    }
    if (pv3) {
        paramVerts_[2] = new ParamVertex(*pv3);
    }
    if (pv4) {
        paramVerts_[3] = new ParamVertex(*pv4);
    }
}

MeshFace::MeshFace(
    const std::string &ref,
    MLINT mid,
    MLINT aref,
    MLINT gref,
    const std::string &name,
    ParamVertex *pv1, ParamVertex *pv2,
    ParamVertex *pv3, ParamVertex *pv4) :
    MeshTopo(ref, mid, aref, gref, name),
    i1_(MESH_TOPO_INDEX_UNUSED),
    i2_(MESH_TOPO_INDEX_UNUSED),
    i3_(MESH_TOPO_INDEX_UNUSED),
    i4_(MESH_TOPO_INDEX_UNUSED)
{
    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    int n;
    for (n = 0; n < 4; ++n) {
        paramVerts_[n] = NULL;
    }
    // make our own copy of the PV data in case owner goes away
    if (pv1) {
        paramVerts_[0] = new ParamVertex(*pv1);
    }
    if (pv2) {
        paramVerts_[1] = new ParamVertex(*pv2);
    }
    if (pv3) {
        paramVerts_[2] = new ParamVertex(*pv3);
    }
    if (pv4) {
        paramVerts_[3] = new ParamVertex(*pv4);
    }
}

MeshFace::~MeshFace()
{
    for (int n = 0; n < 4; ++n) {
        if (NULL != paramVerts_[n]) {
            delete paramVerts_[n]; paramVerts_[n] = NULL;
        }
    }
}

void 
MeshFace::getInds(MLINT *inds, MLINT *numInds) const {
    inds[0] = i1_;
    inds[1] = i2_;
    inds[2] = i3_;
    inds[3] = i4_;
    *numInds = 0;
    if (i1_ != MESH_TOPO_INDEX_UNUSED) ++(*numInds);
    if (i2_ != MESH_TOPO_INDEX_UNUSED) ++(*numInds);
    if (i3_ != MESH_TOPO_INDEX_UNUSED) ++(*numInds);
    if (i4_ != MESH_TOPO_INDEX_UNUSED) ++(*numInds);
}

pwiFnvHash::FNVHash 
MeshFace::getHash() const {
    return computeHash(i1_, i2_, i3_, i4_);
}

pwiFnvHash::FNVHash 
MeshFace::computeHash(MLINT i1, MLINT i2, MLINT i3, MLINT i4) {
    // bubble sort the indices
    if (i2 < i1) std::swap(i2, i1);
    if (i3 < i2) std::swap(i3, i2);
    if (i4 < i3) std::swap(i4, i3);
    if (i2 < i1) std::swap(i2, i1);
    if (i3 < i2) std::swap(i3, i2);
    if (i2 < i1) std::swap(i2, i1);

    pwiFnvHash::FNVHash hash = pwiFnvHash::hashInit();
    if (i1 != MESH_TOPO_INDEX_UNUSED) {
        hash = pwiFnvHash::hash(i1, hash);
    }
    hash = pwiFnvHash::hash(i2, hash);
    hash = pwiFnvHash::hash(i3, hash);
    hash = pwiFnvHash::hash(i4, hash);
    return hash;
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
