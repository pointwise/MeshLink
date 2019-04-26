
#include "GeometryGroup.h"
#include "MeshAssociativity.h"

#include <stdio.h>

ML_EXTERN ML_STORAGE_CLASS void
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





