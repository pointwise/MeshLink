/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

//#include "MeshAssociativity.h"

#include "GeometryGroup.h"
#include "GeomKernel_Geode.h"

#include <geom/Curve.h>
#include <geom/Database.h>
#include <geom/DictionaryAttribute.h>
#include <geom/EntityError.h>
#include <geom/IsectProjPoint.h>
#include <geom/Plane.h>
#include <geom/ProjectionBSPTree.h>
#include <geom/Surface.h>
#include <nmb/NativeTopologyReader.h>
#include <nmb/CurvedCoedge.h>
#include <nmb/CurvedEdge.h>
#include <nmb/CurvedFace.h>
#include <nmb/CurvedModel.h>
#include <nmb/CurvedSheet.h>
#include <nmb/CurvedVertex.h>
#include <nmb/TopologyProjectionBSPTreeWrapper.h>

#include <algorithm>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <cmath>


// Helper function to parse Geode entity attributes for a certain
// subClass and key
std::string getFirstValue(const GE::Entity *entity, const std::string &subClass,
    const std::string &key)
{
    // Iterate over the dictionary attributes until we find one that
    // matches the subClass and contains the key
    std::string result;
    if (entity) {
        const GE::AttributeRegistry *dictReg =
            GE::DictionaryAttribute::Singleton_AttributeRegistry();
        const GE::CharString subClassStr(subClass.c_str());
        const GE::CharString keyStr(key.c_str());
        GE::EntityList<GE::Attribute> attributes;
        entity->Inquire_Attributes(dictReg, &attributes);
        for (GE::Int32 i = 0; i < attributes.Size(); i++) {
            GE::DictionaryAttribute *da =
                GE::DictionaryAttribute::Downcast(attributes[i]);
            if (da && da->Inquire_SubClass() == subClassStr &&
                    da->Contains(keyStr)) {
                result = da->Value(keyStr).ConstData();
                break;
            }
        }
    }
    return result;
}


// Helper function to determine if Geode entity is a curve
bool isCurveEntity(GE::Entity *entity)
{
    // Entity is a curve if derived from GE::Curve
    return entity->Is_InClassID(GE::Curve::Static_ClassID());
}

bool
GeometryKernelGeode::projectPoint(
    const GeometryGroup *group,
        const MLVector3D vpoint, 
    ProjectionData &projectionData
)
{
    if (!projectionData.getData()) { return false; }
    // Single point projection
    GE::ProjectionBSPTree *projectionBSPTree = getBSPTree(group);
    if (!projectionBSPTree) return false;

    const GE::Vector3D point(vpoint[0],vpoint[1],vpoint[2]); 
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData.getData();
    if (NULL == projection) {
        return false;
    }
    else {

        GE::Error error = projectionBSPTree->Compute_CoordMinimumDistance(point,
            NULL, NULL, projection);
        if (error != GE::Error::No_errors) {
            return false;
        }
    }
    return true;
}

// Map entity name to Geode entity
void
GeometryKernelGeode::buildEntityMap()
{
    // Build a map of entity_name to entity_pointer for each usable entity
    MLUINT i;
    for (i = 0; i < usable_entities_.Size(); i++) {
        std::string name = getFirstValue(usable_entities_[(int)i], 
            "PW::Common", "name");
        if (name.empty()) {
            continue;
        }
        usable_entity_map_[name] = usable_entities_[(int)i];
    }
}

bool
GeometryKernelGeode::entityExists(const char* name) const
{ 
    std::map<std::string, GE::Entity*>::const_iterator iter;
    iter = usable_entity_map_.find(name);
    return (iter != usable_entity_map_.end());
}

GE::Entity*
GeometryKernelGeode::getEntity(const std::string &name) const
{
    std::map<std::string, GE::Entity*>::const_iterator iter;
    iter = usable_entity_map_.find(name);
    if (iter != usable_entity_map_.end()) {
        return iter->second;
    }
    return NULL;
}


// Get a projection BSPTree for geometry group
// Create if necessary and add to map.
GE::ProjectionBSPTree *
GeometryKernelGeode::getBSPTree(const GeometryGroup *group) 
{
    std::map<const GeometryGroup *, GE::ProjectionBSPTree *>::iterator treeIter;
    treeIter = BSPTreeMap_.find(group);
    if (treeIter != BSPTreeMap_.end()) {
        return treeIter->second;
    }

    // Create new BSPTree
    GE::ProjectionBSPTree *projectionBSPTree = new GE::ProjectionBSPTree;
    bool status = true;
    MLINT numAdded = 0;
    std::map<std::string, GE::Entity*>::iterator mapIter;
    std::vector<std::string>::const_iterator nameIter;
    const std::vector<std::string> &entity_names = group->getEntityNames();
    for (nameIter = entity_names.begin(); status &&
        nameIter != entity_names.end(); ++nameIter) {
        mapIter = usable_entity_map_.find(*nameIter);
        if (mapIter == usable_entity_map_.end()) {
            // entity missing
            printf("missing geometry group entity: %s\n", (*nameIter).c_str());
            status = false;
        }
        else {
            /* Add the entity to the BSPTree */
            GE::Entity *entity = mapIter->second;
            if (GE::TopologyProjectionBSPTreeWrapper::Add_Entity(projectionBSPTree,
                entity) != GE::Error::No_errors) {
                status = false;
            }
            else {
                ++numAdded;
            }
        }
    }
    ML_assert(numAdded == entity_names.size());

    // Build the BSP tree
    if (status) {
        GE::Int32 maxLevel = 0; // choose based on contents
        GE::Int32 maxObjPerCell = 0; // choose based on contents
        if (projectionBSPTree->Build_BSPTree(maxLevel, maxObjPerCell) !=
            GE::Error::No_errors) {
            status = false;
        }
        else {
            BSPTreeMap_[group] = projectionBSPTree;
        }
    }

    if (!status) {
        delete projectionBSPTree; projectionBSPTree = NULL;
    }
    return projectionBSPTree;
}



ProjectionDataObj
GeometryKernelGeode::getProjectionDataObject()
{
    GE::IsectProjPoint *projection = new GE::IsectProjPoint();
    return projection;
}


void
GeometryKernelGeode::deleteProjectionDataObject(ProjectionDataObj projectionData)
{
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData;
    if (projection) {
        delete projection;
    }
    projectionData = NULL;
}

bool
GeometryKernelGeode::getProjectionXYZ(ProjectionData &projectionData, MLVector3D point)
{
    if (!projectionData.getData()) { return false; }
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData.getData();
    GE::Vector3D xyz = projection->End1.P;  // projection XYZ
    point[0] = xyz.X();
    point[1] = xyz.Y();
    point[2] = xyz.Z();
    return true;
}

bool
GeometryKernelGeode::getProjectionUV(ProjectionData &projectionData, MLVector2D UV)
{
    if (!projectionData.getData()) { return false; }
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData.getData();
    GE::Vector2D uv = projection->End1.s;   // projection UV
    UV[0] = uv.U();
    UV[1] = uv.V();
    return true;
}


bool 
GeometryKernelGeode::getProjectionEntityName(ProjectionData &projectionData, std::string &name)
{
    if (!projectionData.getData()) { return false; }
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData.getData();
    GE::Entity *hit_entity = projection->End1.entity; // projection entity
    name = getFirstValue(hit_entity, "PW::Common", "name");
    if (name.empty()) {
        return false;
    }
    return true;
}


bool
GeometryKernelGeode::getProjectionTolerance(ProjectionData &projectionData, MLREAL &tolerance)
{
    if (!projectionData.getData()) { return false; }
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData.getData();
    if (NULL == projection) {
        return false;
    }
    tolerance = GE::Tolerance::GetSamePoint();
    const GE::CurvedVertex *cvert = NULL;
    const GE::CurvedCoedge *ccoedge = NULL;
    const GE::CurvedEdge *cedge = NULL;
    const GE::CurvedFace *cface = NULL;
    if (NULL != (cvert = GE::CurvedVertex::Downcast(projection->End1.subEntity))) {
        GE::Real64 vtol = 0.0;
        if (GE::Error::No_errors == cvert->Inquire_Tolerance(&vtol)) {
            tolerance = (std::max)(tolerance, vtol);
        }
    }
    else if (NULL != (ccoedge = GE::CurvedCoedge::Downcast(projection->End1.subEntity))) {
        GE::Real64 etol = 0.0;
        if (GE::Error::No_errors == ccoedge->Inquire_Edge()->Inquire_Tolerance(&etol)) {
            tolerance = (std::max)(tolerance, etol);
        }
    }
    else if (NULL != (cedge = GE::CurvedEdge::Downcast(projection->End1.entity)) ||
            NULL != (cedge = GE::CurvedEdge::Downcast(projection->End1.subEntity))) {
        GE::Real64 etol = 0.0;
        if (GE::Error::No_errors == cedge->Inquire_Tolerance(&etol)) {
            tolerance = (std::max)(tolerance, etol);
        }
    }
    else if (NULL != (cface = GE::CurvedFace::Downcast(projection->End1.entity)) ||
            NULL != (cface = GE::CurvedFace::Downcast(projection->End1.subEntity))) {
        tolerance = (std::max)(tolerance, cface->Inquire_Surface()->Inquire_Tolerance());
    }
    return true;
}


/// \brief Evaluate the model assembly tolerance on a surface entity
bool
GeometryKernelGeode::evalSurfaceTolerance(
    const std::string &entityName,
    MLREAL            &minTolerance,
    MLREAL            &maxTolerance)
{
    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return false;
    }

    MLREAL samePtTol = GE::Tolerance::GetSamePoint();
    minTolerance = 1e30;
    maxTolerance = samePtTol;
    
    GE::Real64 tol = 0.0;
    const GE::CurvedFace *cface = NULL;
    if (NULL != (cface = GE::CurvedFace::Downcast(entity))) {
        // surface tolerance
        tol = (std::max)(samePtTol, cface->Inquire_Surface()->Inquire_Tolerance());
        minTolerance = (std::min)(minTolerance, tol);
        maxTolerance = (std::max)(maxTolerance, tol);

        // coedge tolerance
        GE::EntityList<GE::CurvedCoedge> coedgeList;
        GE::Int32 i,numCoedges;
        cface->Inquire_Coedges(&coedgeList, NULL);
        numCoedges = coedgeList.Size();
        for (i = 0; i < numCoedges; ++i) {
            if (GE::Error::No_errors == coedgeList[i]->Inquire_Edge()->Inquire_Tolerance(&tol)) {
                minTolerance = (std::min)(minTolerance, tol);
                maxTolerance = (std::max)(maxTolerance, tol);
            }
        }

        // vertex tolerance
        GE::EntityList<GE::CurvedVertex> vertexList;
        GE::Int32 numVertices;
        cface->Inquire_Vertices(&vertexList, NULL);
        numVertices = vertexList.Size();
        for (i = 0; i < numVertices; ++i) {
            if (GE::Error::No_errors == vertexList[i]->Inquire_Tolerance(&tol)) {
                minTolerance = (std::min)(minTolerance, tol);
                maxTolerance = (std::max)(maxTolerance, tol);
            }
        }
        minTolerance = (std::max)(samePtTol, minTolerance);
        return true;
    }

    return false;
}


bool
GeometryKernelGeode::getProjectionDistance(ProjectionData &projectionData, MLREAL &distance)
{
    if (!projectionData.getData()) { return false; }
    GE::IsectProjPoint *projection = (GE::IsectProjPoint *)projectionData.getData();
    if (NULL == projection) {
        return false;
    }
    distance = projection->Distance;
    return true;
}


bool
GeometryKernelGeode::evalXYZ(MLVector2D UV, const std::string &entityName, MLVector3D xyz)
{
    GE::Vector2D uv(UV[0], UV[1]);
    GE::Vector3D P;

    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return false;
    }
    GE::Error error = GE::Error::General_unknown;

    bool isCurve = isCurveEntity(entity);
    if (isCurve) {
        // 1D curve
        GE::Curve *curve = GE::Curve::Downcast(entity);
        if (curve) {
            error = curve->Evaluate(uv.U(), P);
        }
    }
    else {
        // 2D surface
        GE::Surface *surface = NULL;
        GE::CurvedFace *cface = GE::CurvedFace::Downcast(entity);
        if (cface) {
            // get the surface supporting the curved face
            surface = cface->Inquire_Surface();
        }
        else {
            surface = GE::Surface::Downcast(entity);
        }
        if (surface) {
            error = surface->Evaluate(uv,P);
        }
    }

    if (error == GE::Error::No_errors) {
        for (int n = 0; n < 3; ++n) {
            xyz[n] = P[n];
        }
    }
    else {
        return false;
    }

    return true;
}

/// Determine entity type 
MLTYPE 
GeometryKernelGeode::entityType(const char* entityName)
{
    MLTYPE type = ML_TYPE_UNKNOWN;
    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return type;
    }
    if (isCurveEntity(entity)) {
        // a curve
        type = ML_TYPE_CURVE;
    }
    else {
        // potentially a surface
        GE::Surface *surface = NULL;
        GE::CurvedFace *cface = GE::CurvedFace::Downcast(entity);
        if (cface) {
            surface = cface->Inquire_Surface();
            // proceed with surface as before
        }
        else {
            surface = GE::Surface::Downcast(entity);
        }
        if (surface) {
            type = ML_TYPE_SURFACE;
        }
    }

    return type;
}

static const MLREAL FLAT_CURVATURE = 1e30;


bool
GeometryKernelGeode::evalRadiusOfCurvature(
    MLVector2D UV, 
    const std::string &entityName, 
    MLREAL *minRadiusOfCurvature, 
    MLREAL *maxRadiusOfCurvature)
{
    *minRadiusOfCurvature = FLAT_CURVATURE;   // flat
    *maxRadiusOfCurvature = FLAT_CURVATURE;
    GE::Vector2D uv(UV[0], UV[1]);

    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return false;
    }
    GE::Error error;

    bool isCurve = isCurveEntity(entity);
    if (isCurve) {
        // 1D curve
        GE::Curve *curve = GE::Curve::Downcast(entity);
        if (curve) {
            GE::Vector3D Tangent;
            GE::Vector3D PrincipalNormal;
            GE::Vector3D        Binormal;
            GE::Real64          Curvature;
            bool        Linear;
            error = curve->Evaluate_Curvature(uv.U(), Tangent, PrincipalNormal,
                Binormal, &Curvature, &Linear);
            if (error == GE::Error::No_errors) {
                if (!Linear) {
                    *minRadiusOfCurvature = 1.0 / Curvature;
                    *maxRadiusOfCurvature = *minRadiusOfCurvature;
                }
            }
        }
    }
    else {
        // 2D surface
        GE::Surface *surface = NULL;
        GE::CurvedFace *cface = GE::CurvedFace::Downcast(entity);
        if (cface) {
            surface = cface->Inquire_Surface();
            // proceed with surface as before
        }
        else {
            surface = GE::Surface::Downcast(entity);
        }
        if (surface) {
            GE::Vector3D        P;
            GE::Vector3D        dPdU;
            GE::Vector3D        dPdV;
            GE::Vector3D        d2PdU2;
            GE::Vector3D        d2PdUdV;
            GE::Vector3D        d2PdV2;
            GE::Vector3D        principalV;
            GE::Vector3D        N;
            GE::Real64          avg;
            GE::Real64          gauss;
            GE::Real64          min;
            GE::Real64          max;
            error = surface->Evaluate_Curvature(uv, P, dPdU, dPdV,
                d2PdU2, d2PdUdV, d2PdV2, principalV, N,
                &avg, &gauss, &min, &max);
            min = (std::fabs(min));
            max = (std::fabs(max));
            min = (std::max)(min, 1e-9);
            max = (std::max)(max, 1e-9);
            if (error == GE::Error::No_errors) {

                // The minimum radius of curvature is defined as :
                //    MIN(| 1 / min | , | 1 / max | )
                // The maximum radius of curvature is defined as :
                //    MAX(| 1 / min | , | 1 / max | )

                *minRadiusOfCurvature = (std::min)(1.0 / min, 1.0 / max);
                *maxRadiusOfCurvature = (std::max)(1.0 / min, 1.0 / max);
            }
        }
    }
    return true;
}

bool
GeometryKernelGeode::evalCurvatureOnCurve(
    MLVector2D UV,
    const std::string &entityName,
    MLVector3D             XYZ,   // Evaluated location on curve
    MLVector3D         Tangent,   // tangent to curve
    MLVector3D PrincipalNormal,   // principal normal (pointing towards the center of curvature)
    MLVector3D        Binormal,   // binormal (tangent x principal normal)
    // curvature in radians per unit length 
    // ALWAYS non-negative and in the direction of the principal normal 
    // Radius of curvature = 1 / Curvature
    MLREAL  *Curvature,
    bool        *Linear              // If true, the curve is linear and has no unique normal
)
{
    GE::Vector2D uv(UV[0], UV[1]);

    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return false;
    }
    GE::Error error;

    bool isCurve = isCurveEntity(entity);
    if (isCurve) {
        // 1D curve
        GE::Curve *curve = GE::Curve::Downcast(entity);
        if (curve) {
            GE::Vector3D        ge_XYZ;           // Evaluated location on curve
            error = curve->Evaluate(uv.U(), ge_XYZ);
            if (error != GE::Error::No_errors) {
                return false;
            }

            // Frenet frame
            GE::Vector3D ge_Tangent;           // tangent to curve
            GE::Vector3D ge_PrincipalNormal;   // principal normal (pointing towards the center of curvature)
            GE::Vector3D        ge_Binormal;   // binormal (tangent x principal normal)
            // curvature in radians per unit length 
            // ALWAYS non-negative and in the direction of the principal normal 
            // Radius of curvature = 1 / Curvature
            GE::Real64  ge_Curvature;
            error = curve->Evaluate_Curvature(uv.U(), ge_Tangent, ge_PrincipalNormal,
                ge_Binormal, &ge_Curvature, Linear);
            if (error == GE::Error::No_errors) {

                for (int n = 0; n < 3; ++n) {
                    XYZ[n] = ge_XYZ[n];
                    Tangent[n] = ge_Tangent[n];
                    PrincipalNormal[n] = ge_PrincipalNormal[n];
                    Binormal[n] = ge_Binormal[n];
                }

                *Curvature = ge_Curvature;

                return true;
            }
        }
    }
    else {
        return false;
    }
    return true;
}

bool
GeometryKernelGeode::evalDerivativesOnCurve(
    MLVector2D UV,                // Parametric location
    const std::string &entityName,
    MLVector3D        XYZ,        // Evaluated location on curve
    MLVector3D        dXYZdU,     // First derivative
    MLVector3D        d2XYZdU2    // Second derivative
)
{
    GE::Vector2D uv(UV[0], UV[1]);

    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return false;
    }
    GE::Error error;

    bool isCurve = isCurveEntity(entity);
    if (isCurve) {
        // 1D curve
        GE::Curve *curve = GE::Curve::Downcast(entity);
        if (curve) {
            GE::Vector3D        ge_XYZ;           // Evaluated location on curve
            GE::Vector3D        ge_dXYZdU;        // First derivative
            GE::Vector3D        ge_d2XYZdU2;      // Second derivative

            error = curve->Evaluate_2ndDerivative(uv.U(), ge_XYZ,
                ge_dXYZdU, ge_d2XYZdU2);
            if (error == GE::Error::No_errors) {
                for (int n = 0; n < 3; ++n) {
                    XYZ[n] = ge_XYZ[n];
                    dXYZdU[n] = ge_dXYZdU[n];
                    d2XYZdU2[n] = ge_d2XYZdU2[n];
                }
                return true;
            }
        }
    }
    else {
        return false;
    }
    return true;
}

bool
GeometryKernelGeode::evalCurvatureOnSurface(
    MLVector2D UV,
    const std::string &entityName,

    MLVector3D        XYZ,           // Evaluated location on surface
    MLVector3D        dXYZdU,        // First partial derivative
    MLVector3D        dXYZdV,        // First partial derivative
    MLVector3D        d2XYZdU2,      // Second partial derivative
    MLVector3D        d2XYZdUdV,     // Second partial derivative
    MLVector3D        d2XYZdV2,      // Second partial derivative
    MLVector3D        surfaceNormal, // Surface normal - unit vector
    // Unit vector tangent to surface where curvature = min 
    // surfaceNormal cross principalV yields the direction where curvature = max 
    // if the surface is locally planar (min and max are 0.0) or if the 
    // surface is locally spherical (min and max are equal), 
    // this will be an arbitrary vector tangent to the surface
    MLVector3D        principalV,
    // Minimum and maximum curvature, in radians per unit length
    // Defined so that positive values indicate the surface bends 
    // in the direction of surfaceNormal, and negative values indicate 
    // the surface bends away from surfaceNormal
    MLREAL          *minCurvature,
    MLREAL          *maxCurvature,

    // The average or mean curvature is defined as :
    //    avg = (min + max) / 2
    // The Gaussian curvature is defined as :
    //    gauss = min * max
    MLREAL          *avg,           // Average curvature
    MLREAL          *gauss,         //  Gaussian curvature
    MLORIENT        *orientation    // Orientation of surface in model

)
{
    GE::Vector2D uv(UV[0], UV[1]);

    GE::Entity *entity = getEntity(entityName);
    if (NULL == entity) {
        return false;
    }
    GE::Error error;

    // 2D surface
    GE::Surface *surface = NULL;
    GE::CurvedFace *cface = GE::CurvedFace::Downcast(entity);
    if (cface) {
        // get the surface supporting the curved face
        surface = cface->Inquire_Surface();
    }
    else {
        surface = GE::Surface::Downcast(entity);
    }
    if (surface) {
        GE::Vector3D        ge_XYZ;           // Evaluated location on surface
        GE::Vector3D        ge_dXYZdU;        // First partial derivative
        GE::Vector3D        ge_dXYZdV;        // First partial derivative
        GE::Vector3D        ge_d2XYZdU2;      // Second partial derivative
        GE::Vector3D        ge_d2XYZdUdV;     // Second partial derivative
        GE::Vector3D        ge_d2XYZdV2;      // Second partial derivative
        GE::Vector3D        ge_surfaceNormal; // Surface normal - unit vector
        // Unit vector tangent to surface where curvature = min 
        // surfaceNormal cross principalV yields the direction where curvature = max 
        // if the surface is locally planar (min and max are 0.0) or if the 
        // surface is locally spherical (min and max are equal), 
        // this will be an arbitrary vector tangent to the surface
        GE::Vector3D        ge_principalV;
        // Minimum and maximum curvature, in radians per unit length
        // Defined so that positive values indicate the surface bends 
        // in the direction of surfaceNormal, and negative values indicate 
        // the surface bends away from surfaceNormal
        GE::Real64          ge_minCurvature;
        GE::Real64          ge_maxCurvature;

        // The average or mean curvature is defined as :
        //    avg = (min + max) / 2
        // The Gaussian curvature is defined as :
        //    gauss = min * max
        GE::Real64          ge_avg;           // Average curvature
        GE::Real64          ge_gauss;         //  Gaussian curvature

        // The minimum radius of curvature is defined as :
        //    MIN(| 1 / min | , | 1 / max | )
        // The maximum radius of curvature is defined as :
        //    MAX(| 1 / min | , | 1 / max | )

        error = surface->Evaluate_Curvature(uv, ge_XYZ, ge_dXYZdU, ge_dXYZdV,
            ge_d2XYZdU2, ge_d2XYZdUdV, ge_d2XYZdV2, ge_principalV, ge_surfaceNormal,
            &ge_avg, &ge_gauss, &ge_minCurvature, &ge_maxCurvature);
        if (error == GE::Error::No_errors) {
            for (int n = 0; n < 3; ++n) {
                XYZ[n] = ge_XYZ[n];
                dXYZdU[n] = ge_dXYZdU[n];
                dXYZdV[n] = ge_dXYZdV[n];
                d2XYZdU2[n] = ge_d2XYZdU2[n];
                d2XYZdUdV[n] = ge_d2XYZdUdV[n];
                d2XYZdV2[n] = ge_d2XYZdV2[n];
                principalV[n] = ge_principalV[n];
                surfaceNormal[n] = ge_surfaceNormal[n];
            }

            *avg = ge_avg;
            *gauss = ge_gauss;
            *minCurvature = ge_minCurvature;
            *maxCurvature = ge_maxCurvature;
            *orientation = ML_ORIENT_SAME;

            if (cface) {
                GE::Orientation orient;
                cface->Inquire_Surface(&orient);
                if (orient != GE::Orientation_same) {
                    *orientation = ML_ORIENT_OPPOSITE;
                }
            }

            return true;
        }
    }
    else {
        return false;
    }
    return true;
}


GeometryKernelGeode::GeometryKernelGeode()
{
    // Register a custom Geode error message handler
    GE::ErrorLogger::Register_ErrorHandler(&errorHandler_);
}

GeometryKernelGeode::~GeometryKernelGeode()
{
    std::map<const GeometryGroup *, GE::ProjectionBSPTree *>::iterator iter;
    for (iter = BSPTreeMap_.begin(); iter != BSPTreeMap_.end(); ++iter) {
        delete iter->second;
    }
    BSPTreeMap_.clear();
}

/// Set the model size of the geometry
// Used to define tolerances within the geometry kernel
void
GeometryKernelGeode::setModelSize(MLREAL size)
{
    GE::Tolerance::SetModelSize(size);
}

/// Return the geometry model size
MLREAL
GeometryKernelGeode::getModelSize() const
{
    return GE::Tolerance::GetModelSize();
}

// Read the geometry data file
bool
GeometryKernelGeode::read(const char* filename) 
{ 
    bool status = true;

    /* Read NMB geometry file into Geode database */
    GE::Database &database = database_;
    GE::Error err;
    printf("\nReading Geometry File: %s\n", filename);
    if ((err = GE::NativeTopologyReader::Read(filename, &database)) !=
        GE::Error::No_errors) {
        printf("  error reading %s\n", filename);
        return (false);
    }

    // Loop through "top-level" database entities to gather the
    // entities usable for projection (those matching associativity
    // geometry groups).  Usable entities are of type CurvedFace and type
    // Geometry except for infinite planes
    GE::EntityList<GE::Entity> entities;
    database.Inquire_Entities(entities, true);
    GE::Int32 i;
    GE::EntityList<GE::Entity> &usable_entities = usable_entities_;
    for (i = 0; i < entities.Size(); i++) {
        if (entities[i]->Is_InClassID(GE::CurvedModel::Static_ClassID())) {
            // Need to get the CurvedFaces by extracting them from the
            // CurvedModels, because a CurvedFace is not a top level
            // entity, but a CurvedModel is.
            GE::CurvedModel *model = GE::CurvedModel::Downcast(entities[i]);
            if (model) {
                GE::EntityList<GE::CurvedFace> faces;
                model->Inquire_Faces(&faces);
                usable_entities += faces;
            }
        }
        else if (entities[i]->Is_InClassID(GE::Geometry::Static_ClassID())) {
            usable_entities += entities[i];
        }
    }
    if (usable_entities.Size() == 0) {
        printf("  No usable Entities!\n");
        return (false);
    }
    buildEntityMap();
    printf("  NMB contains %d usable named entities\n", (int)usable_entity_map_.size());

    return status;
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
