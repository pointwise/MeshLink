/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

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
    /// \brief Constructor with GeometryKernel
    KernelData(GeometryKernel *kernel);
    /// \brief Destructor
    ~KernelData();

    /// \brief Return the encapsulated opaque pointer
    KernelDataObj getData() { return data_; }

protected:
    /// The associated GeometryKernel
    GeometryKernel *kernel_;
    /// The opaque data pointer
    KernelDataObj data_;

private:
    /// \brief Hidden default constructor
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
    /// \brief Constructor with GeometryKernel
    ProjectionData(GeometryKernel *kernel);
    /// \brief Destructor
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
 * wrapped geometry kernels. The geometry kernel is application-defined,
 * and performs all geometric operations as provided by this interface.
 *
 */
class ML_STORAGE_CLASS GeometryKernel {
public:
    friend class ProjectionData;

    /// \brief Read the geometry data file.
    ///
    /// Defined by kernel implementation.
    ///
    /// \param filename the name (path) of the geometry file
    virtual bool read(const char* filename);

    /**
     * \brief Project a Cartesian point onto the Geometry group
     *
     * Defined by kernel implementation.
     *
     * Closest point projection of \b point onto the geometric
     * entities in the GeometryGroup. 
     * Data is returned in the ProjectionData object.
     *
     * \param group the GeometryGroup to project upon
     * \param point the point to project
     * \param[out] projectionData the projection result data
     *
     * \return true if the projection was successful
     */
    virtual bool projectPoint(const GeometryGroup *group, 
        const MLVector3D point, 
        ProjectionData &projectionData);

    /// \brief Return the projection hit Cartesian coordinates
    ///
    /// \param[in] projectionData the projection data from which to extract
    /// \param[out] point the projection coordinate result
    virtual bool getProjectionXYZ(ProjectionData &projectionData, MLVector3D point);

    /// \brief Return the projection hit entity parametric coordinates
    ///
    /// \param[in] projectionData the projection data from which to extract
    /// \param[out] UV the projection parametric coordinate result
    virtual bool getProjectionUV(ProjectionData &projectionData, MLVector2D UV);

    /// \brief Return the projection hit entity name
    ///
    /// \param[in] projectionData the projection data from which to extract
    /// \param[out] name the geometric entity name
    virtual bool getProjectionEntityName(ProjectionData &projectionData, std::string &name);

    /// \brief Return the projection distance
    ///
    /// \param[in] projectionData the projection data from which to extract
    /// \param[out] distance the distance between the original point and the projected point
    virtual bool getProjectionDistance(ProjectionData &projectionData, MLREAL &distance);

    /// \brief Return the projection tolerance
    ///
    /// Defined by kernel implementation, the projection tolerance typically indicates local
    /// error tolerance within the geometry or solid topology.
    ///
    /// \param[in] projectionData the projection data from which to extract
    /// \param[out] tolerance the maximum error tolerance of the projection result
    virtual bool getProjectionTolerance(ProjectionData &projectionData, MLREAL &tolerance);

    /// \brief Evaluate the Cartesian coordinates at the entity parametric coordinates
    ///
    /// Defined by kernel implementation.
    ///
    /// \param[in] UV the parametric coordinate location to evaluate
    /// \param[in] entityName the name of the geometric entity to evaluate
    /// \param[out] xyz the evaluated location in model space
    virtual bool evalXYZ(MLVector2D UV, const std::string &entityName, MLVector3D xyz);

    /// \brief Evaluate the radius of curvature at the entity parametric coordinates
    ///
    /// Defined by kernel implementation.
    ///
    /// For a curve entity, min and max are equal.
    ///
    /// \param[in] UV the parametric coordinate location to evaluate
    /// \param[in] entityName the name of the geometric entity to evaluate
    /// \param[out] minRadiusOfCurvature,maxRadiusOfCurvature the minimum and maximum radius of curvature at the given coordinate
    virtual bool evalRadiusOfCurvature(MLVector2D UV, const std::string &entityName, 
        MLREAL *minRadiusOfCurvature, MLREAL *maxRadiusOfCurvature );

    /// \brief Evaluate the curvature on a curve entity at the parametric coordinates
    ///
    /// Defined by kernel implementation.
    ///
    /// \param[in] UV the parametric coordinate (U) location to evaluate
    /// \param[in] entityName the name of the geometric entity to evaluate
    /// \param[out] XYZ the evaluated location in model space
    /// \param[out] Tangent the evaluated tangent vector
    /// \param[out] PrincipalNormal the evaluated principal normal vector
    /// \param[out] Binormal the binormal vector (Tangent X PrincipalNormal)
    /// \param[out] Curvature evaluated curvature in radians per unit length. This is ALWAYS
    ///     non-negative and in the direction of the principal normal vector. Radius of
    ///     curvature is computed as 1 / Curvature.
    /// \param[out] Linear true if curve is linear and has no unique normal vector
    virtual bool evalCurvatureOnCurve(
        MLVector2D UV,
        const std::string &entityName,
        MLVector3D             XYZ,
        MLVector3D         Tangent,
        MLVector3D PrincipalNormal,
        MLVector3D        Binormal,
        MLREAL  *Curvature,
        bool        *Linear
    );

    /// \brief Evaluate the parametric derivatives on a curve entity at the parametric coordinates
    ///
    /// Defined by kernel implementation.
    ///
    /// \param[in] UV the parametric coordinate (U) location to evaluate
    /// \param[in] entityName the name of the geometric entity to evaluate
    /// \param[out] XYZ the evaluated location in model space
    /// \param[out] dXYZdU first derivative
    /// \param[out] d2XYZdU2 second derivative
    virtual bool evalDerivativesOnCurve(
        MLVector2D UV,
        const std::string &entityName,
        MLVector3D        XYZ,
        MLVector3D        dXYZdU,
        MLVector3D        d2XYZdU2
    );

    /// \brief Evaluate the curvature on a surface entity at the parametric coordinates
    ///
    /// Defined by kernel implementation.
    ///
    /// \param[in] UV the parametric coordinate (UV) location to evaluate
    /// \param[in] entityName the name of the geometric entity to evaluate
    /// \param[out] XYZ the evaluated location in model space
    /// \param[out] dXYZdU,dXYZdV first partial derivatives
    /// \param[out] d2XYZdU2,d2XYZdUdV,d2XYZdV2 second partial derivatives
    /// \param[out] surfaceNormal normalized surface normal vector (unit vector)
    /// \param[out] principalV Unit vector tangent to surface where curvature = min;
    ///     surfaceNormal cross principalV yields the direction where curvature = max;
    ///     if the surface is locally planar (min and max are 0.0) or if the 
    ///     surface is locally spherical (min and max are equal), 
    ///     this will be an arbitrary vector tangent to the surface
    /// \param[out] minCurvature,maxCurvature minimum and maximum curvature, in radians
    ///     per unit length; defined so that positive values indicate the surface bends 
    ///     in the direction of surfaceNormal, and negative values indicate 
    ///     the surface bends away from surfaceNormal
    /// \param[out] avg average curvature; average or mean curvature is defined
    ///     as : avg = (min + max) / 2
    /// \param[out] gauss Guassian curvature; Gaussian curvature is defined as :
    ///     gauss = min * max
    /// \param[out] orientation orientation of the surface in the model
    virtual bool evalCurvatureOnSurface(
        MLVector2D UV,
        const std::string &entityName,
        MLVector3D        XYZ,
        MLVector3D        dXYZdU,
        MLVector3D        dXYZdV,
        MLVector3D        d2XYZdU2,
        MLVector3D        d2XYZdUdV,
        MLVector3D        d2XYZdV2,
        MLVector3D        surfaceNormal,
        MLVector3D        principalV,
        MLREAL            *minCurvature,
        MLREAL            *maxCurvature,
        MLREAL            *avg,
        MLREAL            *gauss,
        MLORIENT          *orientation
    );



    /// \brief Evaluate the model assembly tolerance on a surface entity
    ///
    /// Defined by kernel implementation, but in general:
    /// If the surface name given matches a model face, then the face's 
    /// boundary edges and vertices will be querried for the assembly
    /// tolerance used to close the model.
    ///
    /// \param[in] entityName the name of the geometric entity to evaluate
    /// \param[out] minTolerance,maxTolerance minimum and maximum model assembly
    ///     tolerance used to make the model watertight at the shared 
    ///     boundary vertices and edges.
    /// \return true if model assembly tolerance is available for the surface
    virtual bool evalSurfaceTolerance(
        const std::string &entityName,
        MLREAL            &minTolerance,
        MLREAL            &maxTolerance);

    /// \brief Determine entity type 
    ///
    /// \param[in] name the name of the entity
    virtual MLTYPE entityType(const char* name);

    /// \brief Determine if an entity exists in the geometry kernel database
    ///
    /// \param[in] name the name of the entity
    virtual bool entityExists(const char* name);

    /// \brief Return the name of the geometry kernel
    virtual const char * getName() const;

    /// \brief Set the model size of the geometry
    ///
    /// Used to define tolerances within the geometry kernel
    ///
    /// \param[in] size the model size to set
    virtual void setModelSize(MLREAL size);

    /// \brief Return the geometry model size
    virtual MLREAL getModelSize() const;

    /// Default constructor sets model size to 1000.0
    GeometryKernel()
    {
        modelsize_ = 1000.0;
    }

    /// Destructor
    ~GeometryKernel()
    {
    }

private:
    /// model size
    MLREAL  modelsize_;

    /// \brief Construct a point projection data object for use by the geometry kernel
    virtual ProjectionDataObj getProjectionDataObject();

    /// \brief Delete (and free) a kernel point projection data object
    virtual void deleteProjectionDataObject(ProjectionDataObj projectionData);
};

#endif

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
