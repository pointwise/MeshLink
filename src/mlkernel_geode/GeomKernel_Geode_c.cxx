//#include "MeshAssociativity.h"

#include "GeomKernel_Geode.h"
#include "GeomKernel_Geode_c.h"

int
ML_createGeometryKernelGeodeObj(GeometryKernelObj *geomKernelObj)
{
    if (geomKernelObj) {
        *geomKernelObj = (GeometryKernelObj) new GeometryKernelGeode;
        if (*geomKernelObj) {
            return 0;
        }
    }
    return 1;
}

void
ML_freeGeometryKernelGeodeObj(GeometryKernelObj *geomKernelObj)
{
    GeometryKernelGeode *geomKernel = (GeometryKernelGeode *)*geomKernelObj;
    if (geomKernel) {
        delete geomKernel;
        *geomKernelObj = NULL;
    }
}

