/***********************************************************************
**  Refinement Queue Functions
**/

#if !defined(QUEUE_H_)
#define QUEUE_H_

#include "Types.h"
#include "surf_mesh.h"
#include "MeshAssociativity.h"


typedef std::vector<EdgeQual> EdgeRefineQueue;


void addNeighborsToQueue(
    SurfMesh &surfMesh,
    MeshAssociativity &meshAssoc,
    MeshModel &meshModel,
    GeometryKernel *geom_kernel,
    MLREAL qualityThreshold,
    EdgeRefineQueue &queue);

#endif /* QUEUE_H_ */
