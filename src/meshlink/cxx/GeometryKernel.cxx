/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "GeometryGroup.h"
#include "GeometryKernel.h"


// Read the geometry data file
bool 
GeometryKernel::read(const char* filename) { return false; }

bool
GeometryKernel::entityExists(const char* name) { return false; }


const char *
GeometryKernel::getName() const
{
    return NULL;
}


bool
GeometryKernel::projectPoint(const GeometryGroup *group,
    const MLVector3D vpoint, ProjectionData &projectionData)
{
    return false;
}


ProjectionDataObj
GeometryKernel::getProjectionDataObject()
{
    return NULL;
}


void
GeometryKernel::deleteProjectionDataObject(ProjectionDataObj projectionData)
{}


bool 
GeometryKernel::getProjectionXYZ(ProjectionData &projectionData, MLVector3D point)
{
    return false;
}

bool
GeometryKernel::getProjectionUV(ProjectionData &projectionData, MLVector2D UV)
{
    return false;
}

bool
GeometryKernel::getProjectionEntityName(ProjectionData &projectionData, std::string &name)
{
    return false;
}

bool
GeometryKernel::getProjectionDistance(ProjectionData &projectionData, MLREAL &distance)
{
    return false;
}

bool
GeometryKernel::getProjectionTolerance(ProjectionData &projectionData, MLREAL &tolerance)
{
    return false;
}

bool 
GeometryKernel::evalXYZ(MLVector2D UV, const std::string &entityName, MLVector3D xyz)
{
    return false;
}

bool
GeometryKernel::evalRadiusOfCurvature(MLVector2D UV, const std::string &entityName,
    MLREAL *minRadiusOfCurvature, MLREAL *maxRadiusOfCurvature)
{
    return false;
}

bool 
GeometryKernel::evalCurvatureOnCurve(
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
)
{
    return false;
}


bool
GeometryKernel::evalDerivativesOnCurve(
    MLVector2D UV,
    const std::string &entityName,
    MLVector3D        XYZ,        // Evaluated location on curve
    MLVector3D        dXYZdU,     // First derivative
    MLVector3D        d2XYZdU2    // Second derivative
)
{
    return false;
}

/// \brief Evaluate the curvature on a surface entity at the parametric coordinates
///
/// 
bool 
GeometryKernel::evalCurvatureOnSurface(
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
    return false;
}


/// \brief Evaluate the model assembly tolerance on a surface entity
bool 
GeometryKernel::evalSurfaceTolerance(
    const std::string &entityName,
    MLREAL            &minTolerance,
    MLREAL            &maxTolerance)
{
    return false;
}



/// Determine entity type 
MLTYPE
GeometryKernel::entityType(const char* name)
{
    return 0;
}

/// Set the model size of the geometry
// Used to define tolerances within the geometry kernel
void 
GeometryKernel::setModelSize(MLREAL size)
{
    modelsize_ = size;
}

/// Return the geometry model size
MLREAL 
GeometryKernel::getModelSize() const
{
    return modelsize_;
}



KernelData::KernelData(GeometryKernel *kernel) :
    kernel_(kernel),
    data_(0)
{
};

KernelData::~KernelData() {
    data_ = NULL;
};

ProjectionData::ProjectionData(GeometryKernel *kernel) :
    KernelData(kernel)
{
    if (kernel) {
        data_ = kernel->getProjectionDataObject();
    }
};

ProjectionData::~ProjectionData() {
    if (data_) {
        if (kernel_) {
            kernel_->deleteProjectionDataObject(data_);
            data_ = NULL;
        }
        else {
            ML_assert(0 == "data but no kernel");
        }
    }
    data_ = NULL;
};

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
