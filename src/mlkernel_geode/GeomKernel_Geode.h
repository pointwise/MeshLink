#ifndef GEOM_KERNEL_GEODE
#define GEOM_KERNEL_GEODE

#include "Types.h"
#include "GeometryKernel.h"

#include <geom/Curve.h>

#include <map>
#include <set> 
#include <string> 

namespace GE {
    class CharString;
    class Entity;
    class EntityError;
    class IsectProjPoint;
    class ProjectionBSPTree;
    class Vector3D;
}

#include <geom/EntityError.h>
#include <geom/Database.h>
#include <nmb/NativeTopologyReader.h>
#include <geom/Plane.h>
#include <nmb/CurvedFace.h>
#include <nmb/CurvedModel.h>


/****************************************************************************
 *
 * GEErrorHandler class
 *
 * A custom handler for Geode messages.
 *
 ***************************************************************************/
class GeodeErrorHandler : public GE::ErrorHandler {
public:
    virtual void HandleDebug(const GE::EntityError &e) {}
    virtual void HandleDebug(const GE::CharString &errorMessage) {}
    virtual void HandleInfo(const GE::EntityError &e) {}
    virtual void HandleInfo(const GE::CharString &errorMessage) {}
    virtual void HandleWarning(const GE::EntityError &e) {}
    virtual void HandleWarning(const GE::CharString &errorMessage) {}
    virtual void HandleError(const GE::EntityError &e) {}
    virtual void HandleError(const GE::CharString &errorMessage) {}
};

/****************************************************************************
* GeometryKernelGeode class
***************************************************************************/
/**
* \class GeometryKernelGeode
*
* \brief Provide computational geometry access using the Geode kernel
*
*/

class GeometryKernelGeode : public GeometryKernel {
public:
    friend class ProjectionData;
    GeometryKernelGeode();

    ~GeometryKernelGeode();

    // Read the geometry data file
    bool read(const char *filename);

    const char * getName() const
    {
        return "Geode";
    }

    // Project a point onto the Geometry group
    bool projectPoint(const GeometryGroup *group,
        const MLVector3D point,
        ProjectionData &projectionData);

    bool getProjectionXYZ(ProjectionData &projectionData, MLVector3D point);

    bool getProjectionUV(ProjectionData &projectionData, MLVector2D UV);

    bool getProjectionEntityName(ProjectionData &projectionData, std::string &name);

    bool evalXYZ(MLVector3D UV, const std::string &entityName, MLVector3D xyz);

    bool evalRadiusOfCurvature(MLVector2D UV, const std::string &entityName, 
        MLREAL *minRadiusOfCurvature, MLREAL *maxRadiusOfCurvature );

    bool evalCurvatureOnCurve(
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
        );

    bool evalCurvatureOnSurface(
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
            MLREAL          *gauss         //  Gaussian curvature
        );

    /// Determine entity type (0 = unknown, 1 = curve, 2 = surface)
    MLINT entityType(const char* name);

    bool entityExists(const char* name) const;


private:
    /// Construct a point projection data object for use by the geometry kernel
    ProjectionDataObj getProjectionDataObject();

    /// Delete (and free) a point projection data object
    void deleteProjectionDataObject(ProjectionDataObj projectionData);

    GE::Entity* getEntity(const std::string &name) const;

    /// Construct map of entity names to Geode entities
    void buildEntityMap();

    /**
    * Get a projection BSPTree for the geometry group
    *
    * Create if necessary and add to map.
    */
    GE::ProjectionBSPTree *getBSPTree(const GeometryGroup *group);

    GeodeErrorHandler  errorHandler_;
    GE::Database database_;
    GE::EntityList<GE::Entity>  usable_entities_;
    std::map<std::string, GE::Entity*> usable_entity_map_;
    std::map<const GeometryGroup *, GE::ProjectionBSPTree *> BSPTreeMap_; 
};
#endif
