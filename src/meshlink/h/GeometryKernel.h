#ifndef GEOMETRY_KERNEL_CLASS
#define GEOMETRY_KERNEL_CLASS

#include "Types.h"

#include <string>
#include <vector>

class GeometryKernel;
class GeometryGroup;




/****************************************************************************
* KernelData class
***************************************************************************/
/**
* \class KernelData
*
* \brief Opaque container for geometry kernel specific data
*
* Provides a neutral interface for transfering information to/from
* wrapped geometry kernels.
*
*/
class ML_STORAGE_CLASS KernelData {
public:
    KernelData(GeometryKernel *kernel);
    ~KernelData();

    KernelDataObj getData() { return data_; }

protected:
    GeometryKernel *kernel_;
    KernelDataObj data_;

private:
    KernelData();
};

/****************************************************************************
* ProjectionData class
***************************************************************************/
/**
* \class ProjectionData
*
* \brief Opaque container for geometry kernel point projection specific data
*
* Provides a neutral interface for transfering information to/from
* wrapped geometry kernels.
*
*/
class ML_STORAGE_CLASS ProjectionData : public KernelData {
public:
    ProjectionData(GeometryKernel *kernel);
    ~ProjectionData();
};


/****************************************************************************
* GeometryKernel class
***************************************************************************/
/**
* \class GeometryKernel
*
* \brief Base class for geometry kernel interface
*
* Provides a neutral interface for interaction with geometry stored in
* wrapped geometry kernels.
*
*/
class ML_STORAGE_CLASS GeometryKernel {
public:
    friend class ProjectionData;

    /// Read the geometry data file
    virtual bool read(const char* filename);

    /**
    * \brief Project a Cartesian point onto the Geometry group
    *
    * Closest point projection of \b point onto the geometric
    * entities in the GeometryGroup. 
    * Data is returned in the ProjectionData object.
    */
    virtual bool projectPoint(const GeometryGroup *group, 
        const MLVector3D point, 
        ProjectionData &projectionData);

    /// Return the projection hit Cartesian coordinates
    virtual bool getProjectionXYZ(ProjectionData &projectionData, MLVector3D point);

    /// Return the projection hit entity parametric coordinates
    virtual bool getProjectionUV(ProjectionData &projectionData, MLVector2D UV);

    /// Return the projection hit entity name
    virtual bool getProjectionEntityName(ProjectionData &projectionData, std::string &name);

    /// Evaluate the Cartesian coordinates at the entity parametric coordinates
    virtual bool evalXYZ(MLVector2D UV, const std::string &entityName, MLVector3D xyz);

    /// \brief Evaluate the radius of curvature at the entity parametric coordinates
    ///
    /// For a curve entity, min and max are equal.
    virtual bool evalRadiusOfCurvature(MLVector2D UV, const std::string &entityName, 
        MLREAL *minRadiusOfCurvature, MLREAL *maxRadiusOfCurvature );

    /// \brief Evaluate the curvature on a curve entity at the parametric coordinates
    ///
    /// 
    virtual bool evalCurvatureOnCurve(
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
        bool        *Linear           // If true, the curve is linear and has no unique normal
    );

    /// \brief Evaluate the curvature on a surface entity at the parametric coordinates
    ///
    /// 
    virtual bool evalCurvatureOnSurface(
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
        MLREAL          *gauss          //  Gaussian curvature
    );

    /// Determine entity type (0 = unknown, 1 = curve, 2 = surface)
    virtual MLINT entityType(const char* name);

    /// Determine if an entity exists in the geometry kernel database
    virtual bool entityExists(const char* name);


    /// Return the name of the geometry kernel
    virtual const char * getName() const;

    GeometryKernel()
    {
    }

    ~GeometryKernel()
    {
    }

private:
    /// Construct a point projection data object for use by the geometry kernel
    virtual ProjectionDataObj getProjectionDataObject();

    /// Delete (and free) a kernel point projection data object
    virtual void deleteProjectionDataObject(ProjectionDataObj projectionData);
};


#endif
