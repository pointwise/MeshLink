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
    /// \brief Default constructor
    GeometryGroup();
    /// \brief Destructor
    ~GeometryGroup() {};

    /// \brief Set the name of the %GeometryGroup
    ///
    /// \param[in] name the new name of this group
    void setName(const char* name);

    /// \brief Get the name of this %GeometryGroup
    const std::string &getName() const;

    /// \brief Set the ID of this %GeometryGroup
    ///
    /// \param[in] id the unique ID of the group
    void setID(MLINT id);

    /// \brief Get the ID of this %GeometryGroup
    MLINT getID() const;

    /// \brief Set the ID of the group containing this group.
    void setGroupID(MLINT id);

    /// \brief Get the ID of the group containing this group.
    //
    /// \param[out] id the unique ID of this group
    /// \return true if the group is contained in another group
    bool getGroupID(MLINT &id);

    /// \brief Add GeometryGroup that is part of this group
    ///
    /// \param[in] gid the ID of the group to add to this group
    void addGID(MLINT gid);

    /// \brief Return the GeometryGroup IDs that make up this group
    const std::vector<MLINT> &getGIDs();

    /// \brief Set the MeshLinkAttribute AttID referenced by the %GeometryGroup
    ///
    /// \param[in] aref the unique attribute ID (AttID) to set
    void setAref(MLINT aref);

    /// \brief Get the MeshLinkAttribute AttID referenced by the %GeometryGroup
    MLINT getAref() const;

    /// \brief Add a geometry entity's name to the %GeometryGroup
    ///
    /// \param[in] name the name of the geometry entity to add to this group
    void addEntityName(const char *name);

    /// \brief Set the %GeometryGroup's geometry entity list
    ///
    /// \param[in] names the list of names to add to this group
    void setEntityNames(const std::set<std::string> &names);

    /// \brief Get a set of geometry entity names referenced by the %GeometryGroup
    const std::set<std::string> &getEntityNameSet() const;

    /// \brief Get a list of geometry entity names referenced by the %GeometryGroup
    const std::vector<std::string> getEntityNames() const;

    /// \brief Whether the %GeometryGroup has a MeshLinkAttribute AttID reference defined
    bool hasAref() const;

    /// \brief Return the MeshLinkAttribute AttID referenced by the %GeometryGroup
    ///
    /// N.B. This method returns the %GeometryGroup's AREF only. The AREF could
    /// refer to a group of AttIDs.
    ///
    /// \param[out] aref the unique id (AttID) of the associated MeshLinkAttribute
    /// \return true if an attribute has been assigned to this group
    bool getArefID(MLINT *aref) const;

    /// \brief Return list of MeshLinkAttribute AttIDs referenced by the %GeometryGroup
    ///
    /// N.B. This method will expand the %GeometryGroup's AREF into a list
    /// of associated AttIDs.
    ///
    /// \param[in] meshAssoc the MeshAssociativity to query for attributes
    /// \return list of unique attribute IDs (AttID) that are associated with this group
    std::vector<MLINT> get_refAttIDs(const MeshAssociativity &meshAssoc) const;

private:
    /// Group name
    std::string name_;
    /// Group unique ID
    MLINT gid_;
    /// Group attribute ID
    MLINT aref_;
    /// Group ID of containing group
    MLINT groupID_;
    /// IDs of contained geometry groups
    std::vector<MLINT> groupIDs_;  // could be empty if this is a
                                   // GeometryReference and not a
                                   // GeometryGroup (see schema)
    /// Names of contained geometry entities
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

    /// Return array of GeometryGroup IDs
    void            getIDs(std::vector<MLINT> &idList) const;

    /// Return count of GeometryGroups
    MLINT           getCount() const;

private:
    std::vector<GeometryGroup> geometry_groups_;
};

#endif

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
