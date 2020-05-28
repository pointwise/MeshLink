/****************************************************************************
 *
 * Copyright (c) 2019-2020 Pointwise, Inc.
 * All rights reserved.
 *
 * This sample Pointwise source code is not supported by Pointwise, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/


#include "GeometryGroup.h"
#include "MeshAssociativity.h"

#include <stdio.h>

//ML_EXTERN ML_STORAGE_CLASS 
void
GeometryGroup::setName(const char* name)
{ 
    name_ = name; 
}

/****************************************************************************
*
* GeometryGroupManager class
*
* Storage for geometry groups.
* Supports lookup by group name and ID.
*
***************************************************************************/
bool
GeometryGroupManager::addGroup(GeometryGroup &group)
{
    GeometryGroup *matchID = getByID(group.getID());
    GeometryGroup *matchName = getByName(group.getName());
    if (matchID) {
        printf("\nError: Geometry group ID collision: %" MLINT_FORMAT "\n",
            group.getID());
        return false;
    }
    if (matchName) {
        printf("\nError: Geometry group name collision:\n");
        printf("  %s with ID %" MLINT_FORMAT "\n",
            group.getName().c_str(),
            group.getID());
        printf("  %s with ID %" MLINT_FORMAT "\n",
            matchName->getName().c_str(),
            matchName->getID());
        return false;
    }
    geometry_groups_.push_back(group);
    return true;
}


GeometryGroup *
GeometryGroupManager::getByName(std::string name)
{
    GeometryGroup *rtn = NULL;
    if (name.empty()) return NULL;
    for (MLUINT i = 0; i < geometry_groups_.size(); ++i) {
        if (geometry_groups_[i].getName().empty()) continue;
        if (0 == name.compare(geometry_groups_[i].getName())) {
            return &(geometry_groups_[i]);
        }
    }
    return rtn;
}


GeometryGroup *
GeometryGroupManager::getByID(MLINT id)
{
    GeometryGroup *rtn = NULL;
    for (MLUINT i = 0; i < geometry_groups_.size(); ++i) {
        if (id == geometry_groups_[i].getID()) {
            return &(geometry_groups_[i]);
        }
    }
    return rtn;
}


GeometryGroup::GeometryGroup() :
    gid_(MESH_TOPO_INVALID_REF),
    aref_(MESH_TOPO_INVALID_REF),
    groupID_(MESH_TOPO_INVALID_REF)
{};

const std::string &
GeometryGroup::getName() const {
    return name_;
}

/// \brief Set the ID of the %GeometryGroup
void 
GeometryGroup::setID(MLINT id) {
    gid_ = id;
}

/// \brief Get the ID of the %GeometryGroup
MLINT 
GeometryGroup::getID() const {
    return gid_;
}

/// \brief If this GeometryGroup is a GeometryReference in the
/// schema sense, then this ID is the GeometryGroup (schema sense)
/// ID this GeometryReference is a part of
void 
GeometryGroup::setGroupID(MLINT id) {
    groupID_ = id;
}

bool 
GeometryGroup::getGroupID(MLINT &id) {
    if (MESH_TOPO_INVALID_REF == groupID_) {
        return false;
    }
    id = groupID_;
    return true;
}

/// \brief Add GeometryGroup that is part of this group
void 
GeometryGroup::addGID(MLINT gid) {
    groupIDs_.push_back(gid);
}

/// \brief Return the GeometryGroup IDs that make up this group
const std::vector<MLINT> &
GeometryGroup::getGIDs() {
    return groupIDs_;
}

/// \brief Set the MeshLinkAttribute AttID referenced by the %GeometryGroup
void 
GeometryGroup::setAref(MLINT aref) {
    aref_ = aref;
}

/// \brief Get the MeshLinkAttribute AttID referenced by the %GeometryGroup
MLINT 
GeometryGroup::getAref() const {
    return aref_;
}

/// \brief Add a geometry entity's name to the %GeometryGroup
void 
GeometryGroup::addEntityName(const char *name) {
    entity_names_.insert(name);
}

/// \brief Set the %GeometryGroup's geometry entity list
void 
GeometryGroup::setEntityNames(const std::set<std::string> &names) {
    entity_names_ = names;
}

/// \brief Get a set of geometry entity names referenced by the %GeometryGroup
const std::set<std::string> &
GeometryGroup::getEntityNameSet() const {
    return entity_names_;
}

/// \brief Get a list of geometry entity names referenced by the %GeometryGroup
const std::vector<std::string> 
GeometryGroup::getEntityNames() const {
    std::vector<std::string> names;
    std::set<std::string>::const_iterator iter;
    for (iter = entity_names_.begin(); iter != entity_names_.end(); ++iter) {
        names.push_back(*iter);
    }
    return names;
}

bool 
GeometryGroup::hasAref() const { 
    return (aref_ != MESH_TOPO_INVALID_REF); 
}

bool 
GeometryGroup::getArefID(MLINT *aref) const {
    if (!hasAref()) { return false; }
    *aref = aref_;
    return true;
}


std::vector<MLINT>
GeometryGroup::get_refAttIDs(const MeshAssociativity &meshAssoc) const {
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






/****************************************************************************
 *
 * DISCLAIMER:
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, POINTWISE DISCLAIMS
 * ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, WITH REGARD TO THIS SCRIPT. TO THE MAXIMUM EXTENT PERMITTED
 * BY APPLICABLE LAW, IN NO EVENT SHALL POINTWISE BE LIABLE TO ANY PARTY
 * FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE
 * USE OF OR INABILITY TO USE THIS SCRIPT EVEN IF POINTWISE HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES AND REGARDLESS OF THE
 * FAULT OR NEGLIGENCE OF POINTWISE.
 *
 ***************************************************************************/
