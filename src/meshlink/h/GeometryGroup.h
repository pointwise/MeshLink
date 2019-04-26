#ifndef GEOMETRY_GROUP_CLASS
#define GEOMETRY_GROUP_CLASS
#undef _DEBUG

#include "Types.h"

#include <set>
#include <vector>
#include <string>

class projectionData;
class MeshAssociativity;

/****************************************************************************
* GeometryGroup class
***************************************************************************/
/**
* \class GeometryGroup
*
* \brief Storage for geometry association by entity reference.
*
* Mesh entities reference geometry through a GREF attribute
* which is the integer ID of the geometry group associated with
* the mesh entity.
*
*/
class GeometryGroup {
public:
    GeometryGroup():
        gid_(-1),
        aref_(-1)
    {};
    ~GeometryGroup() {};

    /// \brief Set the name of the %GeometryGroup
    void setName(const char* name) ;

    /// \brief Get the name of the %GeometryGroup
    const std::string getName() const { return name_; }

    /// \brief Set the ID of the %GeometryGroup
    void setID(MLINT id) { gid_ = id; }

    /// \brief Get the ID of the %GeometryGroup
    MLINT getID() const { return gid_; }

    /// \brief Set the MeshLinkAttribute AttID referenced by the %GeometryGroup
    void setAref(MLINT aref) { aref_ = aref; }

    /// \brief Get the MeshLinkAttribute AttID referenced by the %GeometryGroup
    MLINT getAref() const { return aref_; }

    /// \brief Add a geometry entity's name to the %GeometryGroup
    void addEntityName(const char *name) {
        entity_names_.insert( name );
    }

    /// \brief Set the %GeometryGroup's geometry entity list
    void setEntityNames(const std::set<std::string> &names) { entity_names_ = names; }

    /// \brief Get a set of geometry entity names referenced by the %GeometryGroup
    const std::set<std::string> &getEntityNameSet() const {
        return entity_names_;
    }

    /// \brief Get a list of geometry entity names referenced by the %GeometryGroup
    const std::vector<std::string> getEntityNames() const {
        std::vector<std::string> names;
        std::set<std::string>::const_iterator iter;
        for (iter = entity_names_.begin(); iter != entity_names_.end(); ++iter) {
            names.push_back(*iter);
        }
        return names;
    }


    /// \brief Whether the MeshTopo has a MeshLinkAttribute AttID reference defined
    bool hasAref() const { return (aref_ != -1); }


    /// \brief Return the MeshLinkAttribute AttID referenced by the %GeometryGroup
    ///
    /// N.B. This method returns the %GeometryGroup's AREF only. The AREF could
    /// refer to a group of AttIDs.
    bool getArefID(MLINT *aref) const {
        if (!hasAref()) { return false; }
        *aref = aref_;
        return true;
    }

    /// \brief Return list of MeshLinkAttribute AttIDs referenced by the %GeometryGroup
    ///
    /// N.B. This method will expand the %GeometryGroup's AREF into a list
    /// of associated AttIDs.
    std::vector<MLINT> get_refAttIDs(const MeshAssociativity &meshAssoc) const;

private:
    std::string name_;
    MLINT gid_;
    MLINT aref_;
    std::set<std::string> entity_names_;  // the content
};


/****************************************************************************
* GeometryGroupManager class
***************************************************************************/
/**
* \class GeometryGroupManager
*
* \brief Storage for geometry groups.
*
* Supports lookup by group name and ID.
*
*/
class GeometryGroupManager {
public:
    bool addGroup(GeometryGroup &group);

    GeometryGroup * getByName(std::string name);

    GeometryGroup * getByID(MLINT id);

private:
    std::vector<GeometryGroup> geometry_groups_;
};

#endif
