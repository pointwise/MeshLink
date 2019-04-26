
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
    MLREAL          *gauss          //  Gaussian curvature
)
{
    return false;
}

/// Determine entity type (0 = unknown, 1 = curve, 2 = surface)
MLINT 
GeometryKernel::entityType(const char* name)
{
    return 0;
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
            ml_assert(0 == "data but no kernel");
        }
    }
    data_ = NULL;
};




