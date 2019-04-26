#include "GeometryKernel.h"
#include "MeshAssociativity.h"

#include <iostream>
#include <sstream>
#include <string.h>
#include <stdio.h>

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
    MeshModelNameMap::iterator mIter;
    for (mIter = meshModelNameMap_.begin(); mIter != meshModelNameMap_.end(); ++mIter) {
        delete mIter->second;
    }
    meshModelNameMap_.clear();
    meshModelIDToNameMap_.clear();
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
    if (NULL == att || att->isGroup()) {
        // can't return name-value for group
        return false;
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

std::vector<MeshModel *>
MeshAssociativity::getMeshModels() const
{
    std::vector<MeshModel *> models;
    models.resize(meshModelNameMap_.size());
    MeshModelNameMap::const_iterator iter;
    size_t i;
    for (i = 0, iter = meshModelNameMap_.begin(); iter != meshModelNameMap_.end(); ++iter, ++i) {
        models[i] = iter->second;
    }
    return models;
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

GeometryKernel *
GeometryKernelManager::getByName(const char *name)
{
    size_t num = geometry_kernels_.size();
    size_t i;
    const char * kernel_name;
    for (i = 0; i < num; ++i) {
        if (nullptr != (kernel_name = geometry_kernels_[i]->getName())) {
            if (0 == strcmp(name, kernel_name)) {
                return geometry_kernels_[i];
            }
        }
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

#include <sstream>
#include <istream>
#include <iterator>



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

