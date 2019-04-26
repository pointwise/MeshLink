#ifndef GEOM_KERNEL_GEODE_C_API
#define GEOM_KERNEL_GEODE_C_API

#include "Types.h"


/* Allocate a new GeometryKernelGeodeObj */
ML_EXTERN ML_STORAGE_CLASS
    int  ML_createGeometryKernelGeodeObj(GeometryKernelObj *geomKernelObj);

/* Free GeometryKernelGeodeObj memory */
ML_EXTERN ML_STORAGE_CLASS
    void   ML_freeGeometryKernelGeodeObj(GeometryKernelObj *geomKernelObj);


#endif
