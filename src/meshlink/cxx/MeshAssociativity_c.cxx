#include "GeometryKernel.h"
#include "MeshAssociativity.h"
#include "MeshLinkParser.h"
#include "Types.h"

#include "MeshAssociativity_c.h"

#include <string.h>

static bool
copyString(char *c_str, MLINT bufLen, const std::string &source)
{
    strncpy(c_str, source.c_str(), bufLen);
    c_str[bufLen - 1] = '\0';
    return true;
}



int
ML_createMeshAssociativityObj(MeshAssociativityObj *meshAssocObj)
{
    *meshAssocObj = (MeshAssociativityObj) new MeshAssociativity;
    if (NULL != *meshAssocObj) {
        MeshAssociativity *meshAssoc = (MeshAssociativity *)*meshAssocObj;
        return 0;
    }
    return 1;
}

void
ML_freeMeshAssociativityObj(MeshAssociativityObj *meshAssocObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)*meshAssocObj;
    if (meshAssoc) {
        delete meshAssoc;
        *meshAssocObj = NULL;
    }
}


int
ML_parserValidateFile(
    MeshLinkParserObj parseObj,
    const char *meshlinkFilename,
    const char *schemaFilename
)
{
    MeshLinkParser *parser = (MeshLinkParser *)parseObj;
    if (parser) {
        std::string ml_fname = meshlinkFilename;
        std::string schema;
        if (schemaFilename) {
            schema = schemaFilename;
        }
        if (ml_fname.empty()) {
            return 1;
        }
        if (parser->validate(ml_fname, schema)) {
            return 0;
        }
    }
    return 1;
}

int
ML_parserReadMeshLinkFile(
    MeshLinkParserObj parseObj,
    const char *meshlink_fname,
    MeshAssociativityObj meshAssocObj
)
{
    MeshLinkParser *parser = (MeshLinkParser *)parseObj;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (parser && meshAssoc) {
        if (!parser->parseMeshLinkFile(meshlink_fname, meshAssoc)) {
            printf("Error parsing geometry-mesh associativity\n");
            return (1);
        }
    }
    else {
        return 1;
    }
    return 0;
}


int ML_getMeshModelByName(MeshAssociativityObj meshAssocObj, 
    const char *modelname,
    MeshModelObj *meshModelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && modelname && meshModelObj) {
        std::string strName = modelname;
        *meshModelObj = meshAssoc->getMeshModelByName(strName);
        if (NULL != *meshModelObj) {
            return 0;
        }
    }
    return 1;
}


int ML_addGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (meshAssoc && geomKernel) {
        meshAssoc->addGeometryKernel(geomKernel);
    }
    else {
        return 1;
    }
    return 0;
}

int ML_setActiveGeometryKernelByName(
    MeshAssociativityObj meshAssocObj, const char *kernelname)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && kernelname) {
        if (!meshAssoc->setActiveGeometryKernelByName(kernelname)) {
            return 1;
        }
    }
    else {
        return 1;
    }
    return 0;
}


int ML_getActiveGeometryKernel(
    MeshAssociativityObj meshAssocObj,
    GeometryKernelObj *geomKernelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && geomKernelObj) {
        *geomKernelObj = meshAssoc->getActiveGeometryKernel();
        if (*geomKernelObj) {
            return 0;
        }
    }
    return 1;
}


MLINT ML_getNumGeometryFiles(MeshAssociativityObj meshAssocObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        return meshAssoc->getNumGeometryFiles();
    }
    return 0;  // return zero count
}

int ML_getGeometryFileObj(MeshAssociativityObj meshAssocObj,
    MLINT index,
    MeshLinkFileConstObj *fileObj)
{
    *fileObj = NULL;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        const std::vector<GeometryFile> &geomFiles = meshAssoc->getGeometryFiles();
        if (index >= 0 && index < (MLINT)geomFiles.size()) {
            *fileObj = &(geomFiles[index]);
            return 0;
        }
        else {
            return 1;
        }
    }
    return 1;
}


int ML_getFilename( MeshLinkFileConstObj fileObj, char *filenameBuf, MLINT filenameBufLen)
{
    filenameBuf[0] = '\0';
    MeshLinkFile *mlFile = (MeshLinkFile *)fileObj;
    if (mlFile) {
        copyString(filenameBuf, filenameBufLen, mlFile->getFilename());
        return 0;
    }
    return 1;
}


int ML_readGeomFile(
    GeometryKernelObj geomKernelObj,
    const char *geomFilename)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (!geomKernel->read(geomFilename)) {
            return (1);
        }
        return 0;
    }
    return 1;
}

int ML_getFileAttIDs(MeshAssociativityObj meshAssocObj,
    MeshLinkFileConstObj fileObj,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs
)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshLinkFile *mlFile = (MeshLinkFile *)fileObj;
    if (meshAssoc && mlFile && attIDs && numAttIDs) {
        std::vector<MLINT> attIDvec = mlFile->getAttributeIDs(*meshAssoc);
        *numAttIDs = (MLINT)attIDvec.size();
        if (*numAttIDs <= sizeAttIDs ) {
            MLINT iAtt;
            for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                attIDs[iAtt] = attIDvec[iAtt];
            }
            return 0;
        }
        else {
            /* insufficient space */
            return 1;
        }
    }
    return 1;
}

int ML_getAttribute(MeshAssociativityObj meshAssocObj,
    MLINT attID,
    char *attNameBuf, MLINT attNameBufLen,
    char *attValueBuf, MLINT attValueBufLen
)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        const char *name, *value;
        if (meshAssoc->getAttribute(attID, &name, &value)) {
            copyString(attNameBuf, attNameBufLen, name);
            copyString(attValueBuf, attValueBufLen, value);
            return 0;
        }
        else {
            /* bad attID */
            return 1;
        }
    }
    return 1;
}

int ML_findHighestTopoPointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
        *meshPointObj = meshModel->findHighestTopoPointByInd(pointIndex);
        if (*meshPointObj) {
            return 0;
        }
    }
    return 1;
}


int ML_findLowestTopoPointByInd(MeshModelObj meshModelObj, 
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
         *meshPointObj = meshModel->findLowestTopoPointByInd(pointIndex);
        if (*meshPointObj) {
            return 0;
        }
    }
    return 1;
}


int ML_findMeshEdgePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
        *meshPointObj = meshModel->findEdgePointByInd(pointIndex);
        if (*meshPointObj) {
            return 0;
        }
    }
    return 1;
}

int ML_findMeshFacePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
        *meshPointObj = meshModel->findFaceEdgePointByInd(pointIndex);
        if (*meshPointObj) {
            return 0;
        }
    }
    return 1;
}


int ML_findLowestTopoEdgeByInds(MeshModelObj meshModelObj,
    MLINT *indices, MLINT numIndices,
    MeshEdgeObj *meshEdgeObj)
{
    if (numIndices != 2) {
        return 1;
    }
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshEdgeObj) {
        *meshEdgeObj = meshModel->findLowestTopoEdgeByInds(indices[0], indices[1]);
        if (*meshEdgeObj) {
            return 0;
        }
    }
    return 1;
}


int ML_getParamVerts(MeshTopoConstObj meshTopoObj, 
    ParamVertexConstObj pvObjsArr[],  /* array of ParamVertexConstObj */
    MLINT pvObjsArrLen,               /* length of array */
    MLINT *num_pvObjs
)
{
    ParamVertex *paramVert = NULL;
    MeshTopo *meshTopo = (MeshTopo *)meshTopoObj;
    MeshPoint *meshPoint = dynamic_cast<MeshPoint *>(meshTopo);
    MeshEdge *meshEdge = dynamic_cast<MeshEdge *>(meshTopo);
    MeshFace *meshFace = dynamic_cast<MeshFace *>(meshTopo);

    ParamVertex *const* pvs = NULL;
    MLINT i;
    *num_pvObjs = 0;
    if (NULL == pvObjsArr || NULL == num_pvObjs) {
        return 1;
    }

    if (NULL != meshPoint) {
        *num_pvObjs = meshPoint->getParamVerts(&pvs);
        if (*num_pvObjs > pvObjsArrLen) {
            return 1;
        }
    }
    else if (NULL != meshEdge) {
        *num_pvObjs = meshEdge->getParamVerts(&pvs);
        if (*num_pvObjs > pvObjsArrLen) {
            return 1;
        }
    }
    else if (NULL != meshFace) {
        *num_pvObjs = meshFace->getParamVerts(&pvs);
        if (*num_pvObjs > pvObjsArrLen) {
            return 1;
        }
    }

    if (pvs == NULL) return 1;

    for (i = 0; i < *num_pvObjs; ++i) {
        pvObjsArr[i] = pvs[i];
    }

    return 0;
}

int ML_getParamVertInfo(ParamVertexConstObj pvObj, 
    char *vrefBuf, MLINT vrefBufLen,
    MLINT *gref, 
    MLINT *mid, 
    MLVector2D UV)
{
    ParamVertex *paramVert = (ParamVertex *)pvObj;

    copyString(vrefBuf, vrefBufLen, "");
    *gref = -1;
    *mid = -1;

    if (paramVert) {
        const std::string &vrefStr = paramVert->getVref();
        copyString(vrefBuf, vrefBufLen, vrefStr);
        *gref = paramVert->getGref();
        *mid = paramVert->getID();
        paramVert->getUV(&(UV[0]), &(UV[1]));
        return 0;
    }
    return 1;
}


int ML_getMeshTopoGref(
    MeshTopoObj meshTopoObj,
    MLINT *gref)
{
    int status = 0;
    MeshTopo *meshTopo = (MeshTopo *)(meshTopoObj);
    if (meshTopo && gref) {
        *gref = meshTopo->getGref();
    }
    else {
        status = 1;
    }
    return status;
}

int ML_getMeshTopoInfo(
    MeshAssociativityObj meshAssocObj,
    MeshTopoObj meshTopoObj,
    char *refBuf, MLINT refBufLen,
    char *nameBuf, MLINT nameBufLen,
    MLINT *gref,
    MLINT *mid,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs)
{
    int status = 0;
    *numAttIDs = 0;

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshTopo *meshTopo = (MeshTopo *)(meshTopoObj);
    if (meshAssoc && meshTopo && attIDs && numAttIDs) {
        const std::string &refStr = meshTopo->getRef();
        copyString(refBuf, refBufLen, refStr);

        const std::string &nameStr = meshTopo->getName();
        copyString(nameBuf, nameBufLen, nameStr);

        *gref = meshTopo->getGref();
        *mid = meshTopo->getID();

        if (0 == status) {
            std::vector<MLINT> attIDvec = meshTopo->getAttributeIDs(*meshAssoc);
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = 0;
            }
            else {
                /* insufficient space */
                status = 1;
                *numAttIDs = 0;
            }
        }
    }
    return status;
}


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
    ParamVertexConstObj *pvObj)
{
    int status = 0;
    *numAttIDs = 0;
    *pvObj = NULL;

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshPoint *meshPoint = (MeshPoint *)(meshPointObj);
    if (meshAssoc && meshPoint && attIDs && numAttIDs) {

        status = ML_getMeshTopoInfo(meshAssocObj, meshPointObj,
            refBuf, refBufLen,
            nameBuf, nameBufLen,
            gref, mid, attIDs, sizeAttIDs, numAttIDs);

        if (0 == status) {
            *pvObj = meshPoint->getParamVert();
        }
    }
    return status;
}


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
    MLINT *numpvObjs
    )
{
    int status = 0;
    *numAttIDs = 0;
    MLINT ipv;
    for (ipv = 0; ipv < sizepvObjs; ++ipv) {
        pvObjs[ipv] = NULL;
    }

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshEdge *meshEdge = (MeshEdge *)(meshEdgeObj);
    if (meshAssoc && meshEdge && attIDs && numAttIDs) {
        const std::string &refStr = meshEdge->getRef();
        copyString(refBuf, refBufLen, refStr);

        const std::string &nameStr = meshEdge->getName();
        copyString(nameBuf, nameBufLen, nameStr);

        *gref = meshEdge->getGref();
        *mid = meshEdge->getID();

        if (0 == status) {
            std::vector<MLINT> attIDvec = meshEdge->getAttributeIDs(*meshAssoc);
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = 0;
            }
            else {
                /* insufficient space */
                status = 1;
                *numAttIDs = 0;
            }
        }

        if (0 == status) {
            std::vector<ParamVertex*> PVvec = meshEdge->getParamVerts();
            *numpvObjs = (MLINT)PVvec.size();
            if (*numpvObjs <= sizeAttIDs) {
                for (ipv = 0; ipv < *numpvObjs; ++ipv) {
                    pvObjs[ipv] = PVvec[ipv];
                }
                status = 0;
            }
            else {
                /* insufficient space */
                status = 1;
                *numpvObjs = 0;
            }
        }
    }
    return status;
}


int ML_getGeometryGroupByID(
    MeshAssociativityObj meshAssocObj,
    MLINT gid,
    GeometryGroupObj *geomGroupObj
)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && geomGroupObj) {
        *geomGroupObj = meshAssoc->getGeometryGroupByID(gid);
        if (*geomGroupObj != NULL) {
            return 0;
        }
    }
    return 1;
}


int ML_getEntityNames(
    GeometryGroupObj geomGroupObj,
    char *entityNamesBufArr,   /* array of entity name buffers */
    MLINT entityNamesArrLen,   /* length of names array */
    MLINT entityNameBufLen,    /* buffer length of a each name */
    MLINT *num_entityNames
)
{
    int status = 0;
    *num_entityNames = 0;
    MLINT i;

    GeometryGroup *geom_group = (GeometryGroup *)geomGroupObj;
    if (geom_group) {
        const std::set<std::string> &entity_names = geom_group->getEntityNameSet();
        *num_entityNames = entity_names.size();
        if (*num_entityNames > entityNamesArrLen) {
            *num_entityNames = 0;
            return 1;
        }
      

        std::set<std::string>::const_iterator iter;
        for (i = 0, iter = entity_names.begin(); iter != entity_names.end(); ++i, ++iter) {
            copyString( &(entityNamesBufArr[i*entityNameBufLen]), entityNameBufLen, (*iter) );
        }
        return 0;
    }
    return 1;
}


int ML_evalXYZ(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,
    const char *entityName,
    MLVector3D XYZ)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->evalXYZ(UV, entityName, XYZ)) {
            return 0;
        }
    }
    return 1;
}

int ML_evalRadiusOfCurvature(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,
    const char *entityName,
    MLREAL *minRadOfCurvature,
    MLREAL *maxRadOfCurvature)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->evalRadiusOfCurvature(UV, entityName, 
            minRadOfCurvature, maxRadOfCurvature )) {
            return 0;
        }
    }
    return 1;
}


int ML_projectPoint(
    GeometryKernelObj geomKernelObj,
    GeometryGroupObj geomGroupObj,
    MLVector3D point,
    ProjectionDataObj projectionDataObj
)
{
    GeometryKernel *geom_kernel = (GeometryKernel *)geomKernelObj;
    GeometryGroup *geom_group = (GeometryGroup *)geomGroupObj;
    ProjectionData *projectionData = (ProjectionData *)projectionDataObj;
    if (geom_kernel && geom_group && projectionData) {
        if (!geom_kernel->projectPoint(geom_group, point, *projectionData)) {
            return 1;
        }
        return 0;
    }
    return 1;
}


int ML_getProjectionInfo(
    GeometryKernelObj geomKernelObj,
    ProjectionDataObj projectionDataObj,
    MLVector3D xyz,
    MLVector2D UV,
    char *entityNameBuf, MLINT entityNameBufLen
)
{
    GeometryKernel *geom_kernel = (GeometryKernel *)geomKernelObj;
    ProjectionData *projectionData = (ProjectionData *)projectionDataObj;
    if (geom_kernel && projectionData) {
        std::string name;
        if (!geom_kernel->getProjectionXYZ(*projectionData, xyz) ||
            !geom_kernel->getProjectionUV(*projectionData, UV) ||
            !geom_kernel->getProjectionEntityName(*projectionData, name)) {
            return 1;
        }
        copyString(entityNameBuf, entityNameBufLen, name);
        return 0;
    }
    return 1;
}

int ML_createProjectionDataObj(
    GeometryKernelObj geomKernelObj,
    ProjectionDataObj *projectionDataObj
)
{
    GeometryKernel *geom_kernel = (GeometryKernel *)geomKernelObj;
    if (geom_kernel && projectionDataObj) {
        *projectionDataObj = (ProjectionDataObj) new ProjectionData(geom_kernel);
        if (*projectionDataObj) {
            return 0;
        }
    }
    return 1;
}

void
ML_freeProjectionDataObj(ProjectionDataObj *projectionDataObj)
{
    ProjectionData *PData = (ProjectionData *)*projectionDataObj;
    if (PData) {
        delete PData;
        *projectionDataObj = NULL;
    }
}
