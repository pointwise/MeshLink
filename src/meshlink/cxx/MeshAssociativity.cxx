/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "GeometryKernel.h"
#include "MeshAssociativity.h"

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>
#include <istream>
#include <iterator>

GeometryKernelManager MeshAssociativity::geometry_kernel_manager_;


/****************************************************************************
*
* MeshAssociativity class
*
* A class for storing mesh topology and geometry associations.
*
***************************************************************************/
MeshAssociativity::~MeshAssociativity()
{
    {
        // Mesh Assoc owns the MeshModels
        MeshModelNameMap::iterator mIter;
        for (mIter = meshModelNameMap_.begin(); 
            mIter != meshModelNameMap_.end(); ++mIter) {
            delete mIter->second;
        }
        meshModelNameMap_.clear();
        meshModelIDToNameMap_.clear();
    }

    clearMeshElementLinkages();
}


bool
MeshAssociativity::addMeshModel(MeshModel *model, bool mapID) {
    if (!model) { return false; }
    if (model->name_.empty()) {
        MeshModel *existingModel = (MeshModel*)1;
        while (NULL != existingModel) {
            model->name_ = model->getNextName();
            existingModel = getMeshModelByName(model->name_.c_str());
        }
    }
    MeshModel *existingModel = getMeshModelByName(model->name_.c_str());
    if (existingModel) {
        return false;
    }
    meshModelNameMap_[model->name_] = model;

    if (!model->getRef().empty()) {
        meshModelRefToNameMap_[model->getRef()] = model->getName();
    }

    if (mapID) {
        meshModelIDToNameMap_[model->mid_] = model->name_;
    }
    return true;
}


MeshModel*
MeshAssociativity::getMeshModelByID(MLINT id) const
{
    return NULL;
}

MeshModel*
MeshAssociativity::getMeshModelByName(const std::string &name) const
{
    MeshModelNameMap::const_iterator mIter = meshModelNameMap_.find(name);
    if (mIter == meshModelNameMap_.end()) {
        return NULL;
    }
    return mIter->second;
}

MeshModel*
MeshAssociativity::getMeshModelByRef(const char * ref) const
{
    MeshTopoRefToNameMap::const_iterator mIter = meshModelRefToNameMap_.find(ref);
    if (mIter == meshModelRefToNameMap_.end()) {
        return NULL;
    }
    return getMeshModelByName(mIter->second);
}


const MeshLinkAttribute *
MeshAssociativity::getAttributeByID(const MLINT id) const
{
    MeshAttributeIDMap::const_iterator iter;
    iter = meshAttributeIDMap_.find(id);
    if (iter != meshAttributeIDMap_.end()) {
        const MeshLinkAttribute *att = &(iter->second);
        return &(iter->second);
    }
    return NULL;
}

void
MeshAssociativity::clearAttributes() {
    meshAttributeIDMap_.clear();
    meshAttributeNameToIDMap_.clear();
}

bool
MeshAssociativity::addAttribute(MeshLinkAttribute &att)
{
    if (NULL != getAttributeByID(att.attid_)) {
        // attid already in use
        return false;
    }
    meshAttributeIDMap_[att.attid_] = att;
    if (!att.name_.empty()) {
        meshAttributeNameToIDMap_[att.name_] = att.attid_;
    }
    return true;
}

/// \brief Return list of MeshLinkAttribute in the MeshAssociativity database
std::vector<const MeshLinkAttribute *> 
MeshAssociativity::getAttributes() const
{
    std::vector<const MeshLinkAttribute *> atts;
    atts.resize(meshAttributeIDMap_.size());
    MeshAttributeIDMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshAttributeIDMap_.begin();
        iter != meshAttributeIDMap_.end(); ++iter, ++i) {
        atts[i] = &(iter->second);
    }
    return atts;
}


///////////////////////////////////////////////////////////////////
// MeshLinkTransform Class
///////////////////////////////////////////////////////////////////

/// Default constructor
MeshLinkTransform::MeshLinkTransform() :
    xid_(-1),
    aref_(MESH_TOPO_INVALID_REF),
    is_valid_(false)
{
    name_.clear();
}

/// Constructor for an transform with a unique transform ID (AttID) and name.
/// The transform's definition is given by its contents.
MeshLinkTransform::MeshLinkTransform(MLINT xid, 
    std::string &name, std::string &contents,
    MeshAssociativity &meshAssoc) :
    xid_(xid),
    aref_(MESH_TOPO_INVALID_REF),
    name_(name),
    contents_(contents)
{
    std::vector<MLREAL> r;
    std::stringstream ss(contents_);
    MLREAL f;
    while (r.size() < 17 && ss >> f) {
        r.push_back(f);
    }
    if (r.size() == 16) {
        int n = 0;
        // PW quaternion output is column indexing fastest
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                xform_[i][j] = r.at(n++);
            }
        }
        is_valid_ = true;
    }
}


void
MeshLinkTransform::getQuaternion(MLREAL xform_quaternion[4][4]) const
{
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            xform_quaternion[i][j] = xform_[i][j];
        }
    }
}


void
MeshLinkTransform::setAref(MLINT aref) { aref_ = aref; }

bool
MeshLinkTransform::hasAref() const {
    return (aref_ != MESH_TOPO_INVALID_REF);
}

MLINT
MeshLinkTransform::getAref() const
{
    if (hasAref()) {
        return aref_;
    }
    else {
        return MESH_TOPO_INVALID_REF;
    }
}

/// \brief Get the XID of this MeshLinkTransform
MLINT 
MeshLinkTransform::getXID() const
{
    return xid_;
}

/// \brief Get the name attribute of this MeshLinkTransform
const std::string & 
MeshLinkTransform::getName() const
{
    return name_;
}

/// \brief Get the contents of this MeshLinkTransform
const std::string & 
MeshLinkTransform::getContents() const
{
    return contents_;
}



const MeshLinkTransform *
MeshAssociativity::getTransformByID(const MLINT id) const
{
    MeshTransformIDMap::const_iterator iter;
    iter = meshTransformIDMap_.find(id);
    if (iter != meshTransformIDMap_.end()) {
        const MeshLinkTransform *xform = &(iter->second);
        return &(iter->second);
    }
    return NULL;
}

void
MeshAssociativity::clearTransforms() {
    meshTransformIDMap_.clear();
    meshTransformNameToIDMap_.clear();
}

bool
MeshAssociativity::addTransform(MeshLinkTransform &xform)
{
    if (NULL != getTransformByID(xform.xid_)) {
        // xid already in use
        return false;
    }
    meshTransformIDMap_[xform.xid_] = xform;

    if (!xform.name_.empty()) {
        meshTransformNameToIDMap_[xform.name_] = xform.xid_;
    }
    return true;
}


void
MeshAssociativity::getTransforms(std::vector<const MeshLinkTransform *> &xforms) const
{
    xforms.clear();
    xforms.resize(meshTransformIDMap_.size());
    MeshTransformIDMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshTransformIDMap_.begin();
        iter != meshTransformIDMap_.end(); ++iter, ++i) {
        xforms[i] = &(iter->second);
    }
    return;
}

size_t
MeshAssociativity::getTransformCount() const
{
    return meshTransformIDMap_.size();
}



///////////////////////////////////////////////////////////////////
// MeshElementLinkage Class
///////////////////////////////////////////////////////////////////

/// Default constructor
MeshElementLinkage::MeshElementLinkage() :
    aref_(MESH_TOPO_INVALID_REF),
    xref_(MESH_TOPO_INVALID_REF),
    is_valid_(false)
{}

/// Constructor for an transform with a unique transform ID (AttID) and name.
/// The transform's definition is given by its contents.
MeshElementLinkage::MeshElementLinkage(
    std::string &name, 
    std::string &sourceEntityRef,
    std::string &targetEntityRef,
    MeshAssociativity &meshAssoc) :
    aref_(MESH_TOPO_INVALID_REF),
    xref_(MESH_TOPO_INVALID_REF),
    sourceEntityRef_(sourceEntityRef),
    targetEntityRef_(targetEntityRef)
{
    MeshModel *model;
    MeshSheet *sheet;
    MeshString *string;

    if (!meshAssoc.getMeshSheetByName(sourceEntityRef, &model, &sheet) &&
        !meshAssoc.getMeshStringByName(sourceEntityRef, &model, &string)) {
        // source doesn't map to sheet or string
        is_valid_ = false;
        return;
    }

    if (!meshAssoc.getMeshSheetByName(targetEntityRef, &model, &sheet) &&
        !meshAssoc.getMeshStringByName(targetEntityRef, &model, &string)) {
        // target doesn't map to sheet or string
        is_valid_ = false;
        return;
    }

    // name arg is allowed to be empty, setName ensures a unique name
    setName(name);

    is_valid_ = true;
}

void
MeshElementLinkage::setAref(MLINT aref) { aref_ = aref; }

bool
MeshElementLinkage::hasAref() const {
    return (aref_ != MESH_TOPO_INVALID_REF);
}

MLINT
MeshElementLinkage::getAref() const
{
    if (hasAref()) {
        return aref_;
    }
    else {
        return MESH_TOPO_INVALID_REF;
    }
}

bool
MeshElementLinkage::setXref(MLINT xref,
    MeshAssociativity &meshAssoc) {
    if (NULL == meshAssoc.getTransformByID(xref)) return false;
    xref_ = xref; 
    return true;
}

bool
MeshElementLinkage::hasXref() const {
    return (xref_ != MESH_TOPO_INVALID_REF);
}

/// \brief Get the Transform XID referenced by this MeshElementLinkage
///
/// \param[out] xref the transform reference ID (XID) for this linkage
bool 
MeshElementLinkage::getXref(MLINT *xref) const
{
    if (hasXref()) {
        *xref = xref_;
        return true;
    }
    return false;
}

/// \brief Get the Transform referenced by this MeshElementLinkage
///
/// returns NULL if XREF is unset or invalid
const MeshLinkTransform * 
MeshElementLinkage::getTransform(const MeshAssociativity &meshAssoc) const
{
    MLINT xref;
    if (!getXref(&xref)) return NULL;
    return meshAssoc.getTransformByID(xref);
}

/// \brief Return the Entity references linked by this MeshElementLinkage
///
/// \param[out] sourceEntityRef reference to the source entity
/// \param[out] targetEntityRef reference to the target entity
void 
MeshElementLinkage::getEntityRefs(
    std::string &sourceEntityRef,
    std::string &targetEntityRef) const

{
    sourceEntityRef = sourceEntityRef_;
    targetEntityRef = targetEntityRef_;
}

void
MeshElementLinkage::getName(const char **name) const
{
    *name = name_.c_str();
}

const std::string &
MeshElementLinkage::getName() const
{
    return name_;
}


void
MeshElementLinkage::setName(const std::string &name) {
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
MeshElementLinkage::setName(const char *nameCstr) {
    std::string name;
    if (nameCstr) {
        name = nameCstr;
    }
    setName(name);
}

std::string
MeshElementLinkage::getNextName()
{
    std::string basename = getBaseName();
    MLUINT &counter = getNameCounter();
    counter++;
    std::ostringstream s;  s << counter;
    std::string name = basename + s.str();
    return name;
}

const std::string &
MeshElementLinkage::getBaseName() const
{
    static const std::string base("ml_elemlink-");
    return base;
};

MLUINT MeshElementLinkage::nameCounter_ = 0;

MLUINT &
MeshElementLinkage::getNameCounter() {
    return nameCounter_;
};



void
MeshAssociativity::clearMeshElementLinkages() {
    // Mesh Assoc owns the MeshElementLinkages
    MeshElementLinkageNameMap::iterator mIter;
    for (mIter = meshElementLinkageNameMap_.begin();
        mIter != meshElementLinkageNameMap_.end(); ++mIter) {
        delete mIter->second;
    }
    meshElementLinkageNameMap_.clear();
}



bool
MeshAssociativity::addMeshElementLinkage(MeshElementLinkage *link) {
    if (!link) { return false; }
    if (link->name_.empty()) {
        MeshElementLinkage *existing = (MeshElementLinkage*)1;
        while (NULL != existing) {
            link->name_ = link->getNextName();
            existing = getMeshElementLinkageByName(link->name_.c_str());
        }
    }
    MeshElementLinkage *existing = getMeshElementLinkageByName(link->name_.c_str());
    if (existing) {
        return false;
    }
    meshElementLinkageNameMap_[link->name_] = link;

    return true;
}


MeshElementLinkage*
MeshAssociativity::getMeshElementLinkageByName(const std::string &name) const
{
    MeshElementLinkageNameMap::const_iterator mIter = meshElementLinkageNameMap_.find(name);
    if (mIter == meshElementLinkageNameMap_.end()) {
        return NULL;
    }
    return mIter->second;
}


void
MeshAssociativity::getMeshElementLinkages(std::vector<MeshElementLinkage *> &links) const
{
    links.clear();
    links.resize(meshElementLinkageNameMap_.size());
    MeshElementLinkageNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshElementLinkageNameMap_.begin(); 
        iter != meshElementLinkageNameMap_.end(); ++iter, ++i) {
        links[i] = iter->second;
    }
    return;
}

size_t
MeshAssociativity::getMeshElementLinkageCount() const
{
    return meshElementLinkageNameMap_.size();
}




/// \brief Get MeshSheet by name
///
/// \param[in] name the name of the desired sheet
/// \param[out] model containing the desired sheet
/// \param[out] the desired sheet
/// \return true if found
bool 
MeshAssociativity::getMeshSheetByName(const std::string &name,
    MeshModel **model, MeshSheet **sheet) const
{
    MeshModelNameMap::const_iterator iter;
    for (iter = meshModelNameMap_.begin(); iter != meshModelNameMap_.end(); ++iter) {
        *model = iter->second;
        if (NULL != (*sheet = (*model)->getMeshSheetByName(name))) {
            return true;
        }
    }
    *model = NULL;
    *sheet = NULL;
    return false;
}

/// \brief Get MeshString by name
///
/// \param[in] name the name of the desired string
/// \param[out] model containing the desired string
/// \param[out] the desired string
/// \return true if found
bool 
MeshAssociativity::getMeshStringByName(const std::string &name,
    MeshModel **model, MeshString **string) const
{
    MeshModelNameMap::const_iterator iter;
    for (iter = meshModelNameMap_.begin(); iter != meshModelNameMap_.end(); ++iter) {
        *model = iter->second;
        if (NULL != (*string = (*model)->getMeshStringByName(name))) {
            return true;
        }
    }
    *model = NULL;
    *string = NULL;
    return false;
}


void
MeshAssociativity::addGeometryFile(GeometryFile &file) {
    geometryFiles_.push_back(file);
}

void
MeshAssociativity::addMeshFile(MeshFile &file) {
    meshFiles_.push_back(file);
}

const std::vector<GeometryFile> &
MeshAssociativity::getGeometryFiles() const
{
    return geometryFiles_;
}

const std::vector<MeshFile> &
MeshAssociativity::getMeshFiles() const
{
    return meshFiles_;
}

MLINT
MeshAssociativity::getNumGeometryFiles() const
{
    return (MLINT)geometryFiles_.size();
}

MLINT
MeshAssociativity::getNumMeshFiles() const
{
    return (MLINT)meshFiles_.size();
}


bool
MeshAssociativity::getAttribute(MLINT attID,
    const char **name,
    const char **value
) const {
    const MeshLinkAttribute *att = getAttributeByID(attID);
    if (NULL == att) {
        // no attribute for that attID
        return false;
    }
    if (att->isGroup()) {
        // can't return name-value for group
        std::cout << "Accessing group for export purposes." << std::endl;
        // return false;
    }
    *name = att->name_.c_str();
    *value = att->contents_.c_str();
    return true;
}


bool
MeshAssociativity::addGeometryGroup(GeometryGroup &geometry_group)
{
    return geometry_group_manager_.addGroup(geometry_group);
}


// Get geometry group by name - returns NULL if not found
GeometryGroup *
MeshAssociativity::getGeometryGroupByName(const char* name)
{
    return geometry_group_manager_.getByName(name);
}


MLINT
MeshAssociativity::getGeometryGroupCount() const
{
    return geometry_group_manager_.getCount();
}

void
MeshAssociativity::getGeometryGroupIDs(std::vector<MLINT> &gids) const
{
    geometry_group_manager_.getIDs(gids);
}


// Get geometry group by ID - returns NULL if not found
GeometryGroup *
MeshAssociativity::getGeometryGroupByID(MLINT id)
{
    return geometry_group_manager_.getByID(id);
}


void
MeshAssociativity::addGeometryKernel(GeometryKernel *kernel)
{
    geometry_kernel_manager_.addKernel(kernel);
}

void
MeshAssociativity::removeGeometryKernel(GeometryKernel *kernel)
{
    geometry_kernel_manager_.removeKernel(kernel);
}

GeometryKernel *
MeshAssociativity::getGeometryKernelByName(const char *name)
{
    return geometry_kernel_manager_.getByName(name);
}

GeometryKernel *
MeshAssociativity::getActiveGeometryKernel()
{
    return geometry_kernel_manager_.getActive();
}

bool
MeshAssociativity::setActiveGeometryKernelByName(const char *name)
{
    return geometry_kernel_manager_.setActiveByName(name);
}

void
MeshAssociativity::getMeshModels(std::vector<MeshModel *> &models) const
{
    models.clear();
    models.resize(meshModelNameMap_.size());
    MeshModelNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshModelNameMap_.begin(); iter != meshModelNameMap_.end(); ++iter, ++i) {
        models[i] = iter->second;
    }
    return;
}

size_t
MeshAssociativity::getMeshModelCount() const
{
    return meshModelNameMap_.size();
}

void
GeometryKernelManager::addKernel(GeometryKernel *kernel)
{
    if (kernel) {
        geometry_kernels_.push_back(kernel);
    }
    if (geometry_kernels_.size() == 1) {
        active_ = geometry_kernels_[0];
    }
}

void
GeometryKernelManager::removeKernel(GeometryKernel *kernel)
{
    if (kernel) {
        GeometryKernelArray::iterator it =
            std::find(geometry_kernels_.begin(), geometry_kernels_.end(), kernel);
        if (it != geometry_kernels_.end()) {
            geometry_kernels_.erase(it);
        }
    }
    if (geometry_kernels_.size() == 1) {
        active_ = geometry_kernels_[0];
    }
}

GeometryKernel *
GeometryKernelManager::getByName(const char *name)
{
    GeometryKernelArray::iterator iter = geometry_kernels_.begin();
    const char * kernel_name;
    while (iter != geometry_kernels_.end()) {
        GeometryKernel *kernel = *iter;
        if (nullptr != kernel &&
                nullptr != (kernel_name = kernel->getName())) {
            if (0 == strcmp(name, kernel_name)) {
                return kernel;
            }
        }
        ++iter;
    }
    return NULL;
}

GeometryKernel *
GeometryKernelManager::getActive()
{
    return active_;
}

bool
GeometryKernelManager::setActiveByName(const char *name)
{
    GeometryKernel *kernel = getByName(name);
    if (kernel) {
        active_ = kernel;
        return true;
    }
    return false;
}




// Return list of attribute IDs referenced by the MeshLinkFile
std::vector<MLINT>
MeshLinkFile::getAttributeIDs(const MeshAssociativity &meshAssoc) const {
    std::vector<MLINT> ref_attids;
    if (!hasAref()) {
        return ref_attids;
    }
    MLINT aref;
    if (!getArefID(&aref)) { return ref_attids; }

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


bool
MeshLinkAttribute::buildGroupArefs(MeshAssociativity &meshAssoc) {
    group_arefs_.clear();
    if (is_group_) {
        // The content string is a list of Arefs, which may themselves refer to a AttGroup.
        // At the end of the parsing below, group_arefs should be a vector
        // of unique and existing attribute refIds.

        // construct a stream from the string
        std::stringstream strstr(contents_);

        // use stream iterators to copy the stream to the vector as whitespace separated strings
        std::istream_iterator<std::string> it(strstr);
        std::istream_iterator<std::string> end;
        std::vector<std::string> arefs(it, end);

        std::set<MLINT> arefSet;

        size_t i;
        for (i = 0; i < arefs.size(); ++i) {
            std::string &aref = arefs[i];
            int arefID = -1;
                        if (1 != sscanf(aref.c_str(), "%d", &arefID)) {
                std::cout << "AttributeGroup invalid content \"" << aref << "\"" << std::endl;
                group_arefs_.clear();
                return false;
            }
            const MeshLinkAttribute *att = meshAssoc.getAttributeByID((MLINT)arefID);
            if (NULL != att) {
                if (att->isGroup()) {
                    std::vector<MLINT> gArefs = att->getAttributeIDs();
                    std::vector<MLINT>::iterator groupIter;
                    for (groupIter = gArefs.begin(); groupIter != gArefs.end(); ++groupIter) {
                        arefSet.insert(*groupIter);
                    }
                }
                else {
                    arefSet.insert(att->attid_);
                }
            }
            else {
                std::cout << "AttributeGroup unknown attid in content \"" << arefID << "\"" << std::endl;
                group_arefs_.clear();
                return false;
            }
        }

        std::set<MLINT>::iterator iter = arefSet.begin();
        while (iter != arefSet.end()) {
            group_arefs_.push_back(*iter);
            ++iter;
        }
    }
    else {
        // Not an attribute group, add the attid to the group list for convenience
        group_arefs_.push_back(attid_);
    }
    return true;
}


/// Default constructor
MeshLinkAttribute::MeshLinkAttribute() :
    attid_(-1),
    is_group_(false),
    is_valid_(true)
{}

/// Constructor for an attribute with a unique attribute ID (AttID) and name.
/// The attribute's definition is given by its contents.
MeshLinkAttribute::MeshLinkAttribute(MLINT attid, std::string &name, std::string &contents,
    bool is_group, MeshAssociativity &meshAssoc) :
    attid_(attid),
    name_(name),
    contents_(contents),
    is_group_(is_group)
{
    if (is_group) {
        is_valid_ = buildGroupArefs(meshAssoc);
    }
    else {
        is_valid_ = true;
    }
}

/// \brief Get the AttID of this attribute.
MLINT 
MeshLinkAttribute::getAttID() const
{
    return attid_;
}

/// Whether this attribute is a group of other MeshLinkAttribute s.
bool 
MeshLinkAttribute::isGroup() const { return is_group_; }

/// Whether this attribute is valid. An attribute may be invalid if it is a group
/// of other attribute IDs, and any one of which do not exist.
bool 
MeshLinkAttribute::isValid() const { return is_valid_; }

/// \brief Get the AttIDs referenced by this attribute.
///
/// If the %MeshLinkAttribute is a group, the AttIDs of the group
/// members are returned, otherwise, this attribute's AttID is returned
const std::vector<MLINT> & 
MeshLinkAttribute::getAttributeIDs() const {
    return group_arefs_;
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
