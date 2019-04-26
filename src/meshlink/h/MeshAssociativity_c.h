#ifndef MESH_ASSOCIATIVITY_C_API
#define MESH_ASSOCIATIVITY_C_API

#include "Types.h"

#ifndef SWIGx
#endif
#ifndef SWIGx
#endif
;


/* Allocate a new MeshAssociativityObj */
 ML_EXTERN ML_STORAGE_CLASS int ML_createMeshAssociativityObj( MeshAssociativityObj *meshAssocObj ) ;


/* Free MeshAssociativityObj memory */
 ML_EXTERN ML_STORAGE_CLASS
    void   ML_freeMeshAssociativityObj(MeshAssociativityObj *meshAssocObj) ;


ML_EXTERN ML_STORAGE_CLASS
    int    ML_parserValidateFile(
        MeshLinkParserObj parseObj,
        const char *meshlinkFilename,
        const char *schemaFilename);

ML_EXTERN ML_STORAGE_CLASS
    int    ML_parserReadMeshLinkFile(
        MeshLinkParserObj parseObj,
        const char *meshlinkFilename,
        MeshAssociativityObj meshAssocObj);


ML_EXTERN ML_STORAGE_CLASS
int ML_getMeshModelByName(MeshAssociativityObj meshAssocObj, const char *modelname, MeshModelObj *meshModel);

ML_EXTERN ML_STORAGE_CLASS
int ML_addGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj);

ML_EXTERN ML_STORAGE_CLASS
int ML_setActiveGeometryKernelByName(MeshAssociativityObj meshAssocObj, const char *kernelname);

ML_EXTERN ML_STORAGE_CLASS
int ML_getActiveGeometryKernel(
        MeshAssociativityObj meshAssocObj,
        GeometryKernelObj *geomKernelObj);


ML_EXTERN ML_STORAGE_CLASS
MLINT ML_getNumGeometryFiles(MeshAssociativityObj meshAssocObj);

ML_EXTERN ML_STORAGE_CLASS
int ML_getGeometryFileObj(MeshAssociativityObj meshAssocObj,
    MLINT index,
    MeshLinkFileConstObj *fileObj);

ML_EXTERN ML_STORAGE_CLASS
int ML_getFilename( MeshLinkFileConstObj fileObj, char *filenameBuf, MLINT filenameBufLen );

/* Read geometry file */
ML_EXTERN ML_STORAGE_CLASS
    int ML_readGeomFile(
        GeometryKernelObj geomKernelObj,
        const char *geomFilename);

ML_EXTERN ML_STORAGE_CLASS
    int ML_getFileAttIDs(MeshAssociativityObj meshAssocObj,
        MeshLinkFileConstObj fileObj,
        MLINT attIDs[],
        MLINT sizeAttIDs,
        MLINT *numAttIDs  );

ML_EXTERN ML_STORAGE_CLASS
    int ML_getAttribute(MeshAssociativityObj meshAssocObj,
        MLINT attID,
        char *attNameBuf, MLINT attNameBufLen,
        char *attValueBuf, MLINT attValueBufLen
    );

ML_EXTERN ML_STORAGE_CLASS
    int ML_findLowestTopoPointByInd(MeshModelObj meshModelObj,
        MLINT pointIndex, MeshPointObj* meshPointObj);

ML_EXTERN ML_STORAGE_CLASS
int ML_findHighestTopoPointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex, MeshPointObj* meshPointObj);

ML_EXTERN ML_STORAGE_CLASS
int ML_findMeshEdgePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex, MeshPointObj* meshPointObj);

ML_EXTERN ML_STORAGE_CLASS
int ML_findMeshFacePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex, MeshPointObj* meshPointObj);

ML_EXTERN ML_STORAGE_CLASS
    int ML_findLowestTopoEdgeByInds(MeshModelObj meshModelObj,
        MLINT *indices, MLINT numIndices,  // python wrapper depends on these names
        MeshEdgeObj *meshEdgeObj);

ML_EXTERN ML_STORAGE_CLASS
    int ML_getParamVerts(MeshTopoConstObj meshTopoObj,
        ParamVertexConstObj pvObjsArr[],  /* array of ParamVertexConstObj */
        MLINT pvObjsArrLen,               /* length of array */
        MLINT *num_pvObjs );

ML_EXTERN ML_STORAGE_CLASS
    int ML_getParamVertInfo(ParamVertexConstObj pvObj,
        char *vrefBuf, MLINT vrefBufLen,
        MLINT *gref,
        MLINT *mid,
        MLVector2D UV);


ML_EXTERN ML_STORAGE_CLASS
    int ML_getMeshTopoGref(
        MeshTopoObj meshTopoObj,
        MLINT *gref);

ML_EXTERN ML_STORAGE_CLASS
    int ML_getMeshTopoInfo(
        MeshAssociativityObj meshAssocObj,
        MeshTopoObj meshTopoObj,
        char *refBuf, 
        MLINT refBufLen,
        char *nameBuf, 
        MLINT nameBufLen,
        MLINT *gref,
        MLINT *mid,
        MLINT attIDs[],
        MLINT sizeAttIDs,
        MLINT *numAttIDs );


ML_EXTERN ML_STORAGE_CLASS
int ML_getMeshPointInfo(
    MeshAssociativityObj meshAssocObj,
    MeshPointObj meshPointObj,
    char *refBuf, MLINT refBufLen,
    char *nameBuf, MLINT nameBufLen,
    MLINT *gref,
    MLINT *mid,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs,
    ParamVertexConstObj *pvObj);


ML_EXTERN ML_STORAGE_CLASS
    int ML_getMeshEdgeInfo(
    MeshAssociativityObj meshAssocObj,
    MeshEdgeObj meshEdgeObj,
    char *refBuf, MLINT refBufLen,
    char *nameBuf, MLINT nameBufLen,
    MLINT *gref,
    MLINT *mid,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs,
    ParamVertexConstObj pvObjs[],
    MLINT sizepvObjs,
    MLINT *numpvObjs) ;


ML_EXTERN ML_STORAGE_CLASS
    int ML_getGeometryGroupByID(
        MeshAssociativityObj meshAssocObj,
        MLINT gid,
        GeometryGroupObj *geomGroupObj
    );

ML_EXTERN ML_STORAGE_CLASS
    int ML_getEntityNames(
        GeometryGroupObj geomGroupObj,
        char *entityNamesBufArr,      /* array of entity name buffers */
        MLINT entityNamesArrLen,   /* length of names array */
        MLINT entityNameBufLen,    /* buffer length of a each name */
        MLINT *num_entityNames
    );


ML_EXTERN ML_STORAGE_CLASS
    int ML_evalXYZ(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,
    const char *entityName,
    MLVector3D XYZ);

ML_EXTERN ML_STORAGE_CLASS
    int ML_evalRadiusOfCurvature(
        GeometryKernelObj geomKernelObj,
        MLVector2D UV,
        const char *entityName,
        MLREAL *minRradOfCurvature,
        MLREAL *maxRradOfCurvature);


ML_EXTERN ML_STORAGE_CLASS
    int ML_createProjectionDataObj(
        GeometryKernelObj geomKernelObj,
        ProjectionDataObj *projectionDataObj
    );

ML_EXTERN ML_STORAGE_CLASS
    void ML_freeProjectionDataObj(ProjectionDataObj *projectionDataObj);


ML_EXTERN ML_STORAGE_CLASS
    int ML_projectPoint(
    GeometryKernelObj geomKernelObj,
    GeometryGroupObj geomGroupObj,
    MLVector3D point,
    ProjectionDataObj projectionDataObj
);

ML_EXTERN ML_STORAGE_CLASS
    int ML_getProjectionInfo(
    GeometryKernelObj geomKernelObj,
    ProjectionDataObj projectionDataObj,
    MLVector3D xyz,
    MLVector2D UV,
    char *entityNameBuf, MLINT entityNameBufLen
);


#endif
