/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "GeometryKernel.h"
#include "MeshAssociativity.h"
#include "MeshLinkParser.h"
#include "MeshLinkWriter.h"
#include "Types.h"

#include "MeshAssociativity_c.h"

#include <string.h>

/* snapshot of library data sizes at compile */
static const size_t lib_size_of_MLINT = sizeof(MLINT);
static const size_t lib_size_of_MLINT32 = sizeof(MLINT32);
static const size_t lib_size_of_MLFLOAT = sizeof(MLFLOAT);
static const size_t lib_size_of_MLREAL = sizeof(MLREAL);

static bool
copyString(char *c_str, MLINT bufLen, const std::string &source)
{
    strncpy(c_str, source.c_str(), bufLen);
    c_str[bufLen - 1] = '\0';
    return true;
}

MLSTATUS
ML_checkDataSizes(
    size_t size_of_MLINT,
    size_t size_of_MLINT32,
    size_t size_of_MLFLOAT,
    size_t size_of_MLREAL
)
{
    MLSTATUS status = ML_STATUS_OK;
    if (size_of_MLINT != lib_size_of_MLINT) {
        printf("\nML_checkDataSizes\n");
        printf("  ERROR: library MLINT is %" MLSIZE_T_FORMAT
            " bytes, but called with %" MLSIZE_T_FORMAT " bytes\n",
            lib_size_of_MLINT, size_of_MLINT);
        status = ML_STATUS_ERROR;
    }
    if (size_of_MLINT32 != lib_size_of_MLINT32) {
        printf("\nML_checkDataSizes\n");
        printf("  ERROR: library MLINT32 is %" MLSIZE_T_FORMAT
            " bytes, but called with %" MLSIZE_T_FORMAT " bytes\n",
            lib_size_of_MLINT32, size_of_MLINT32);
        status = ML_STATUS_ERROR;
    }
    if (size_of_MLFLOAT != lib_size_of_MLFLOAT) {
        printf("\nML_checkDataSizes\n");
        printf("  ERROR: library MLFLOAT is %" MLSIZE_T_FORMAT
            " bytes, but called with %" MLSIZE_T_FORMAT " bytes\n",
            lib_size_of_MLFLOAT, size_of_MLFLOAT);
        status = ML_STATUS_ERROR;
    }
    if (size_of_MLREAL != lib_size_of_MLREAL) {
        printf("\nML_checkDataSizes\n");
        printf("  ERROR: library MLREAL is %" MLSIZE_T_FORMAT
            " bytes, but called with %" MLSIZE_T_FORMAT " bytes\n",
            lib_size_of_MLREAL, size_of_MLREAL);
        status = ML_STATUS_ERROR;
    }
    if (ML_STATUS_OK == status) {
        printf("\nML_checkDataSizes: OK\n");
    }

    return status;
}

MLSTATUS
ML_createMeshAssociativityObj(MeshAssociativityObj *meshAssocObj)
{
    *meshAssocObj = (MeshAssociativityObj) new MeshAssociativity;
    if (NULL != *meshAssocObj) {
        MeshAssociativity *meshAssoc = (MeshAssociativity *)*meshAssocObj;
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
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


MLSTATUS
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
            return ML_STATUS_ERROR;
        }
        if (parser->validate(ml_fname, schema)) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS
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
            return (ML_STATUS_ERROR);
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return ML_STATUS_OK;
}


MLSTATUS
ML_parserGetMeshLinkAttributes(
    MeshLinkParserObj parseObj,
    char *xmlns, MLINT xmlnsBufLen,
    char *xmlns_xsi, MLINT xmlns_xsiBufLen,
    char *schemaLocation, MLINT schemaBufLen)
{
    std::string nameSpace;
    std::string typeSpace;
    std::string location;
    MeshLinkParser *parser = (MeshLinkParser *)parseObj;
    if (parser) {
        parser->getMeshLinkAttributes(nameSpace, typeSpace, location);
        copyString(xmlns, xmlnsBufLen, nameSpace);
        copyString(xmlns_xsi, xmlns_xsiBufLen, typeSpace);
        copyString(schemaLocation, schemaBufLen, location);
    }
    else {
        return ML_STATUS_ERROR;
    }
    return ML_STATUS_OK;
}


MLSTATUS
ML_writerWriteMeshLinkFile(
    MeshLinkWriterObj writeObj,
    MeshAssociativityObj meshAssocObj,
    const char *xmlns,
    const char *xmlns_xsi,
    const char *schemaLocation,
    const char *meshlink_fname)
{
    MeshLinkWriter *writer = (MeshLinkWriter *)writeObj;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (writer && meshAssoc) {
        writer->setMeshLinkAttributes(xmlns, xmlns_xsi, schemaLocation);
        if (!writer->writeMeshLinkFile(meshlink_fname, meshAssoc)) {
            printf("Error writing geometry-mesh associativity\n");
            return ML_STATUS_ERROR;
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return ML_STATUS_OK;
}


MLSTATUS ML_getMeshModelByName(MeshAssociativityObj meshAssocObj,
    const char *modelname,
    MeshModelObj *meshModelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && modelname && meshModelObj) {
        std::string strName = modelname;
        *meshModelObj = meshAssoc->getMeshModelByName(strName);
        if (NULL != *meshModelObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_addGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (meshAssoc && geomKernel) {
        meshAssoc->addGeometryKernel(geomKernel);
    }
    else {
        return ML_STATUS_ERROR;
    }
    return ML_STATUS_OK;
}


MLSTATUS ML_removeGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (meshAssoc && geomKernel) {
        meshAssoc->removeGeometryKernel(geomKernel);
    }
    else {
        return ML_STATUS_ERROR;
    }
    return ML_STATUS_OK;
}

MLSTATUS ML_setActiveGeometryKernelByName(
    MeshAssociativityObj meshAssocObj, const char *kernelname)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && kernelname) {
        if (!meshAssoc->setActiveGeometryKernelByName(kernelname)) {
            return ML_STATUS_ERROR;
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return ML_STATUS_OK;
}


MLSTATUS ML_getActiveGeometryKernel(
    MeshAssociativityObj meshAssocObj,
    GeometryKernelObj *geomKernelObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && geomKernelObj) {
        *geomKernelObj = meshAssoc->getActiveGeometryKernel();
        if (*geomKernelObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLINT ML_getNumGeometryFiles(MeshAssociativityObj meshAssocObj)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        return meshAssoc->getNumGeometryFiles();
    }
    return 0;  // return zero count
}

MLSTATUS ML_getGeometryFileObj(MeshAssociativityObj meshAssocObj,
    MLINT index,
    MeshLinkFileConstObj *fileObj)
{
    *fileObj = NULL;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        const std::vector<GeometryFile> &geomFiles = meshAssoc->getGeometryFiles();
        if (index >= 0 && index < (MLINT)geomFiles.size()) {
            *fileObj = &(geomFiles[index]);
            return ML_STATUS_OK;
        }
        else {
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getFilename( MeshLinkFileConstObj fileObj, char *filenameBuf, MLINT filenameBufLen)
{
    filenameBuf[0] = '\0';
    MeshLinkFile *mlFile = (MeshLinkFile *)fileObj;
    if (mlFile) {
        copyString(filenameBuf, filenameBufLen, mlFile->getFilename());
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}


/* Define geometry model size */
MLSTATUS ML_setGeomModelSize(
    GeometryKernelObj geomKernelObj,
    MLREAL modelSize)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        geomKernel->setModelSize(modelSize);
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

/* Get geometry model size */
MLSTATUS ML_getGeomModelSize(
    GeometryKernelObj geomKernelObj,
    MLREAL *modelSize)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        *modelSize = geomKernel->getModelSize();
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_readGeomFile(
    GeometryKernelObj geomKernelObj,
    const char *geomFilename)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (!geomKernel->read(geomFilename)) {
            return (ML_STATUS_ERROR);
        }
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_getFileAttIDs(MeshAssociativityObj meshAssocObj,
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
            for (iAtt = ML_STATUS_OK; iAtt < *numAttIDs; ++iAtt) {
                attIDs[iAtt] = attIDvec[iAtt];
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_getAttribute(MeshAssociativityObj meshAssocObj,
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
            return ML_STATUS_OK;
        }
        else {
            /* bad attID */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}

MLINT ML_getNumParamVerts(MeshTopoObj meshTopoObj)
{
    MLINT count = 0;
    MeshTopo *meshTopo = (MeshTopo *)meshTopoObj;
    if (meshTopo) {
        count = (MLINT)meshTopo->getNumParamVerts();
    }
    return count;
}

MLSTATUS ML_getParamVerts(MeshModelObj meshTopoObj,
    ParamVertexConstObj *pvObjs,
    MLINT sizePVObj,
    MLINT *numPVObjs
)
{
    *numPVObjs = 0;
    MeshTopo *meshTopo = (MeshTopo *)meshTopoObj;
    if (meshTopo) {
        MLINT count = ML_getNumParamVerts(meshTopoObj);
        if (sizePVObj >= count) {
            std::vector<const ParamVertex *> pv;
            meshTopo->getParamVerts(pv);
            *numPVObjs = (MLINT)pv.size();
            MLINT i;
            for (i = 0; i < *numPVObjs; ++i) {
                pvObjs[i] = pv.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}



MLINT ML_getNumMeshModels(MeshAssociativityObj meshAssocObj)
{
    MLINT count = 0;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        count = (MLINT)meshAssoc->getMeshModelCount();
    }
    return count;
}

MLSTATUS ML_getMeshModels(MeshAssociativityObj meshAssocObj,
    MeshTopoObj *modelObjs,
    MLINT sizeModelObj,
    MLINT *numModelObjs
)
{
    *numModelObjs = 0;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        MLINT count = ML_getNumMeshModels(meshAssocObj);
        if (sizeModelObj >= count) {
            std::vector<MeshModel *> models;
            meshAssoc->getMeshModels(models);
            *numModelObjs = (MLINT) models.size();
            MLINT i;
            for (i = 0; i < *numModelObjs; ++i) {
                modelObjs[i] = models.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }

    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_getModelMeshSheetByName(MeshModelObj meshModelObj,
    const char *sheetname,
    MeshSheetObj *meshSheetObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && sheetname && meshSheetObj) {
        std::string strName = sheetname;
        *meshSheetObj = meshModel->getMeshSheetByName(strName);
        if (NULL != *meshSheetObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLINT ML_getNumMeshSheets(MeshModelObj meshModelObj
)
{
    MLINT count = 0;
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel) {
        count = (MLINT)meshModel->getMeshSheetCount();
    }
    return count;
}

MLSTATUS ML_getMeshSheets(MeshModelObj meshModelObj,
    MeshSheetObj *sheetObjs,
    MLINT sizeSheetObj,
    MLINT *numSheetObjs
)
{
    *numSheetObjs = 0;
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel) {
        MLINT count = ML_getNumMeshSheets(meshModel);
        if (sizeSheetObj >= count) {
            std::vector<MeshSheet *> sheets;
            meshModel->getMeshSheets(sheets);
            *numSheetObjs = (MLINT)sheets.size();
            MLINT i;
            for (i = 0; i < *numSheetObjs; ++i) {
                sheetObjs[i] = sheets.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }

    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getModelMeshStringByName(MeshModelObj meshModelObj,
    const char *stringname,
    MeshStringObj *meshStringObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && stringname && meshStringObj) {
        std::string strName = stringname;
        *meshStringObj = meshModel->getMeshStringByName(strName);
        if (NULL != *meshStringObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLINT ML_getNumMeshStrings(MeshModelObj meshModelObj
)
{
    MLINT count = 0;
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel) {
        count = (MLINT)meshModel->getMeshStringCount();
    }
    return count;
}

MLSTATUS ML_getMeshStrings(MeshModelObj meshModelObj,
    MeshTopoObj sheetObjs[],
    MLINT sizeStringObj,
    MLINT *numStringObjs
)
{
    *numStringObjs = 0;
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel) {
        MLINT count = ML_getNumMeshStrings(meshModel);
        if (sizeStringObj >= count) {
            std::vector<MeshString *> strings;
            meshModel->getMeshStrings(strings);
            *numStringObjs = (MLINT)strings.size();
            MLINT i;
            for (i = 0; i < *numStringObjs; ++i) {
                sheetObjs[i] = strings.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }

    }
    return ML_STATUS_ERROR;
}


ML_EXTERN ML_STORAGE_CLASS
MLINT ML_getNumSheetMeshFaces(MeshSheetObj meshSheetObj
)
{
    MLINT count = 0;
    MeshSheet *meshSheet = (MeshSheet *)meshSheetObj;
    if (meshSheet) {
        count = meshSheet->getNumFaces();
    }
    return count;
}

ML_EXTERN ML_STORAGE_CLASS
MLSTATUS ML_getSheetMeshFaces(MeshSheetObj meshSheetObj,
    MeshTopoObj faceObjs[],
    MLINT sizeFaceObj,
    MLINT *numFaceObjs
)
{
    MeshSheet *meshSheet = (MeshSheet *)meshSheetObj;
    if (meshSheet) {
        MLINT count = ML_getNumSheetMeshFaces(meshSheet);
        if (sizeFaceObj >= count) {
            std::vector<const MeshFace *> faces;
            meshSheet->getMeshFaces(faces);
            *numFaceObjs = (MLINT)faces.size();
            MLINT i;
            for (i = 0; i < *numFaceObjs; ++i) {
                faceObjs[i] = (MeshTopoObj)faces.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}

ML_EXTERN ML_STORAGE_CLASS
MLINT ML_getNumSheetMeshFaceEdges(MeshSheetObj meshSheetObj
)
{
    MLINT count = 0;
    MeshSheet *meshSheet = (MeshSheet *)meshSheetObj;
    if (meshSheet) {
        count = meshSheet->getNumFaceEdges();
    }
    return count;
}

ML_EXTERN ML_STORAGE_CLASS
MLSTATUS ML_getSheetMeshFaceEdges(MeshSheetObj meshSheetObj,
    MeshTopoObj edgeObjs[],
    MLINT sizeEdgeObj,
    MLINT *numEdgeObjs
)
{
    MeshSheet *meshSheet = (MeshSheet *)meshSheetObj;
    if (meshSheet) {
        MLINT count = ML_getNumSheetMeshFaceEdges(meshSheet);
        if (sizeEdgeObj >= count) {
            std::vector<const MeshEdge *> edges;
            meshSheet->getFaceEdges(edges);
            *numEdgeObjs = (MLINT)edges.size();
            MLINT i;
            for (i = 0; i < *numEdgeObjs; ++i) {
                edgeObjs[i] = (MeshTopoObj)edges.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}


ML_EXTERN ML_STORAGE_CLASS
MLINT ML_getNumStringMeshEdges(MeshSheetObj meshStringObj
)
{
    MLINT count = 0;
    MeshString *meshString = (MeshString *)meshStringObj;
    if (meshString) {
        count = meshString->getNumEdges();
    }
    return count;
}


ML_EXTERN ML_STORAGE_CLASS
MLSTATUS ML_getStringMeshEdges(MeshSheetObj meshStringObj,
    MeshTopoObj edgeObjs[],
    MLINT sizeEdgeObj,
    MLINT *numEdgeObjs
)
{
    MeshString *meshString = (MeshString *)meshStringObj;
    if (meshString) {
        MLINT count = ML_getNumStringMeshEdges(meshString);
        if (sizeEdgeObj >= count) {
            std::vector<const MeshEdge *> edges;
            meshString->getMeshEdges(edges);
            *numEdgeObjs = (MLINT)edges.size();
            MLINT i;
            for (i = 0; i < *numEdgeObjs; ++i) {
                edgeObjs[i] = (MeshTopoObj)edges.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}


/**
 * \brief Get array of MeshElementLinkages in the MeshAssociativity database.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in,out] linkageObjs MeshElementLinkage object array
 * @param[in] sizeLinkageObj MeshElementLinkage object array size
 * @param[out] numLinkageObjs MeshElementLinkage object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshElementLinkages(
    MeshAssociativityObj meshAssocObj,
    MeshElementLinkageObj *linkageObjs,
    MLINT sizeLinkageObj,
    MLINT *numLinkageObjs
)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        MLINT count = ML_getNumMeshElementLinkages(meshAssoc);
        if (sizeLinkageObj >= count) {
            std::vector<MeshElementLinkage *> links;
            meshAssoc->getMeshElementLinkages(links);
            *numLinkageObjs = (MLINT)links.size();
            MLINT i;
            for (i = 0; i < *numLinkageObjs; ++i) {
                linkageObjs[i] = (MeshElementLinkageObj)links.at(i);
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}

/**
 * \brief Get number of MeshElementLinkages in the MeshAssociativity database.
 * @param[in] meshAssocObj MeshAssociativity object
 * \returns MeshElementLinkage count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT  ML_getNumMeshElementLinkages(
    MeshAssociativityObj meshAssocObj)
{
    MLINT count = 0;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        count = meshAssoc->getMeshElementLinkageCount();
    }
    return count;
}


MLSTATUS ML_getMeshElementLinkageInfo(
    MeshAssociativityObj meshAssocObj,
    MeshElementLinkageObj linkageObj,
    char *nameBuf, MLINT nameBufLen,
    char *sourceEntityRefBuf, MLINT sourceEntityRefBufLen,
    char *targetEntityRefBuf, MLINT targetEntityRefBufLen,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs)
{
    int status = ML_STATUS_OK;
    *numAttIDs = 0;

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshElementLinkage *link = (MeshElementLinkage *)(linkageObj);
    if (meshAssoc && link && attIDs && numAttIDs) {
        const std::string &name = link->getName();
        copyString(nameBuf, nameBufLen, name);

        std::string sourceEntityRef;
        std::string targetEntityRef;
        link->getEntityRefs(sourceEntityRef, targetEntityRef);

        copyString(sourceEntityRefBuf, sourceEntityRefBufLen, sourceEntityRef);
        copyString(targetEntityRefBuf, targetEntityRefBufLen, targetEntityRef);


        if (ML_STATUS_OK == status) {
            std::vector<MLINT> attIDvec;
            if (link->hasAref()) {
                MLINT aref = link->getAref();
                const MeshLinkAttribute *att = meshAssoc->getAttributeByID(aref);
                if (NULL != att) {
                    if (att->isGroup()) {
                        attIDvec = att->getAttributeIDs();
                    }
                    else {
                        attIDvec.push_back(aref);
                    }
                }
            }
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
                *numAttIDs = 0;
            }
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return status;
}

MLSTATUS ML_getMeshLinkTransformInfo(
    MeshAssociativityObj meshAssocObj,
    MeshLinkTransformConstObj xformObj,
    char *nameBuf, MLINT nameBufLen,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs)
{
    int status = ML_STATUS_OK;
    *numAttIDs = 0;

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshLinkTransform *xform = (MeshLinkTransform *)(xformObj);
    if (meshAssoc && xform && attIDs && numAttIDs) {
        const std::string &name = xform->getName();
        copyString(nameBuf, nameBufLen, name);

        if (ML_STATUS_OK == status) {
            std::vector<MLINT> attIDvec;
            if (xform->hasAref()) {
                MLINT aref = xform->getAref();
                const MeshLinkAttribute *att = meshAssoc->getAttributeByID(aref);
                if (NULL != att) {
                    if (att->isGroup()) {
                        attIDvec = att->getAttributeIDs();
                    }
                    else {
                        attIDvec.push_back(aref);
                    }
                }
            }
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
                *numAttIDs = 0;
            }
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return status;
}


MLSTATUS ML_getMeshSheetByName(MeshAssociativityObj meshAssocObj,
    const char *sheetname,
    MeshModelObj *meshModel, MeshSheetObj *meshSheet)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        MeshModel *model;
        MeshSheet *sheet;
        if (meshAssoc->getMeshSheetByName(sheetname, &model, &sheet)) {
            *meshModel = model;
            *meshSheet = sheet;
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getMeshStringByName(MeshAssociativityObj meshAssocObj,
    const char *stringname,
    MeshModelObj *meshModel, MeshStringObj *meshString)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        MeshModel *model;
        MeshString *string;
        if (meshAssoc->getMeshStringByName(stringname, &model, &string)) {
            *meshModel = model;
            *meshString = string;
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}



/**
 * \brief Get MeshLinkTransform associated with a MeshElementLinkage
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] linkageObj MeshElementLinkage object
 * @param[out] xformObj MeshLinkTransform object (null if none)
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getTransform(
    MeshAssociativityObj meshAssocObj,
    MeshElementLinkageObj linkageObj,
    MeshLinkTransformConstObj *xformObj
)
{
    *xformObj = NULL;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshElementLinkage *link = (MeshElementLinkage *)linkageObj;
    if (meshAssoc && link) {
        *xformObj = link->getTransform(*meshAssoc);
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

/**
 * \brief Get the quaternion matrix associated with a MeshLinkTransform
 * @param[in] xformObj MeshLinkTransform object
 * @param[out] quat[4][4] quaternion matrix
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getTransformQuaternion(
    MeshLinkTransformConstObj xformObj,
    MLREAL quat[4][4]
)
{
    MeshLinkTransform *xform = (MeshLinkTransform *)xformObj;
    if (xform) {
        xform->getQuaternion(quat);
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_findHighestTopoPointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
        *meshPointObj = meshModel->findHighestTopoPointByInd(pointIndex);
        if (*meshPointObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_findLowestTopoPointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
         *meshPointObj = meshModel->findLowestTopoPointByInd(pointIndex);
        if (*meshPointObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_findMeshEdgePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
        *meshPointObj = meshModel->findEdgePointByInd(pointIndex);
        if (*meshPointObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_findMeshFacePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex,
    MeshPointObj *meshPointObj)
{
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshPointObj) {
        *meshPointObj = meshModel->findFaceEdgePointByInd(pointIndex);
        if (*meshPointObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}



ML_EXTERN ML_STORAGE_CLASS
MLSTATUS ML_getFaceInds(MeshFaceObj meshFaceObj,
    MLINT inds[],       /* array of size 4 */
    MLINT *numInds
)
{
    *numInds = 0;
    MeshFace *meshFace = (MeshFace *)meshFaceObj;
    if (meshFace) {
        meshFace->getInds(inds, numInds);
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

ML_EXTERN ML_STORAGE_CLASS
MLSTATUS ML_getEdgeInds(MeshEdgeObj meshEdgeObj,
    MLINT inds[],       /* array of size 2 */
    MLINT *numInds
)
{
    *numInds = 0;
    MeshEdge *meshEdge = (MeshEdge *)meshEdgeObj;
    if (meshEdge) {
        meshEdge->getInds(inds, numInds);
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_findFaceByInds(MeshModelObj meshModelObj,
    MLINT *indices, MLINT numIndices,
    MeshEdgeObj *meshFaceObj)
{
    if (numIndices != 3 && numIndices != 4) {
        return ML_STATUS_ERROR;
    }
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshFaceObj) {
        if (numIndices == 3) {
            *meshFaceObj = meshModel->findFaceByInds(indices[0], indices[1], indices[2]);
        }
        else {
            *meshFaceObj = meshModel->findFaceByInds(indices[0], indices[1],
                indices[2], indices[3] );
        }
        if (*meshFaceObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_findLowestTopoEdgeByInds(MeshModelObj meshModelObj,
    MLINT *indices, MLINT numIndices,
    MeshEdgeObj *meshEdgeObj)
{
    if (numIndices != 2) {
        return ML_STATUS_ERROR;
    }
    MeshModel *meshModel = (MeshModel *)meshModelObj;
    if (meshModel && meshEdgeObj) {
        *meshEdgeObj = meshModel->findLowestTopoEdgeByInds(indices[0], indices[1]);
        if (*meshEdgeObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getParamVerts(MeshTopoConstObj meshTopoObj,
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
        return ML_STATUS_ERROR;
    }

    if (NULL != meshPoint) {
        *num_pvObjs = meshPoint->getParamVerts(&pvs);
        if (*num_pvObjs > pvObjsArrLen) {
            return ML_STATUS_ERROR;
        }
    }
    else if (NULL != meshEdge) {
        *num_pvObjs = meshEdge->getParamVerts(&pvs);
        if (*num_pvObjs > pvObjsArrLen) {
            return ML_STATUS_ERROR;
        }
    }
    else if (NULL != meshFace) {
        *num_pvObjs = meshFace->getParamVerts(&pvs);
        if (*num_pvObjs > pvObjsArrLen) {
            return ML_STATUS_ERROR;
        }
    }

    if (pvs == NULL && NULL != meshTopo) {
        MLINT count = meshTopo->getNumParamVerts();
        std::vector<const ParamVertex *> pv;
        if (count > pvObjsArrLen) {
            return ML_STATUS_ERROR;
        }
        *num_pvObjs = count;
        meshTopo->getParamVerts(pv);
        MLINT i;
        for (i = 0; i < *num_pvObjs; ++i) {
            pvObjsArr[i] = pv[i];
        }
    }
    else {
        if (pvs == NULL) return ML_STATUS_ERROR;

        for (i = 0; i < *num_pvObjs; ++i) {
            pvObjsArr[i] = pvs[i];
        }
    }

    return ML_STATUS_OK;
}

MLSTATUS ML_getParamVertInfo(ParamVertexConstObj pvObj,
    char *vrefBuf, MLINT vrefBufLen,
    MLINT *gref,
    MLINT *mid,
    MLVector2D UV)
{
    ParamVertex *paramVert = (ParamVertex *)pvObj;

    copyString(vrefBuf, vrefBufLen, "");
    *gref = MESH_TOPO_INVALID_REF;
    *mid = MESH_TOPO_INVALID_REF;

    if (paramVert) {
        const std::string &vrefStr = paramVert->getVref();
        copyString(vrefBuf, vrefBufLen, vrefStr);
        *gref = paramVert->getGref();
        *mid = paramVert->getID();
        paramVert->getUV(&(UV[0]), &(UV[1]));
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getMeshTopoGref(
    MeshTopoObj meshTopoObj,
    MLINT *gref)
{
    int status = ML_STATUS_OK;
    MeshTopo *meshTopo = (MeshTopo *)(meshTopoObj);
    if (meshTopo && gref) {
        *gref = meshTopo->getGref();
    }
    else {
        status = ML_STATUS_ERROR;
    }
    return status;
}

MLSTATUS ML_getMeshTopoInfo(
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
    int status = ML_STATUS_OK;
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

        if (ML_STATUS_OK == status) {
            std::vector<MLINT> attIDvec = meshTopo->getAttributeIDs(*meshAssoc);
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
                *numAttIDs = 0;
            }
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return status;
}


MLSTATUS ML_getMeshPointInfo(
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
    int status = ML_STATUS_OK;
    *numAttIDs = 0;
    *pvObj = NULL;

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshPoint *meshPoint = (MeshPoint *)(meshPointObj);
    if (meshAssoc && meshPoint && attIDs && numAttIDs) {

        status = ML_getMeshTopoInfo(meshAssocObj, meshPointObj,
            refBuf, refBufLen,
            nameBuf, nameBufLen,
            gref, mid, attIDs, sizeAttIDs, numAttIDs);

        if (ML_STATUS_OK == status) {
            *pvObj = meshPoint->getParamVert();
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return status;
}


MLSTATUS ML_getMeshEdgeInfo(
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
    int status = ML_STATUS_OK;
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

        if (ML_STATUS_OK == status) {
            std::vector<MLINT> attIDvec = meshEdge->getAttributeIDs(*meshAssoc);
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
                *numAttIDs = 0;
            }
        }

        if (ML_STATUS_OK == status) {
            std::vector<ParamVertex*> PVvec;
            meshEdge->getParamVerts(PVvec);
            *numpvObjs = (MLINT)PVvec.size();
            if (*numpvObjs <= sizeAttIDs) {
                for (ipv = 0; ipv < *numpvObjs; ++ipv) {
                    pvObjs[ipv] = PVvec[ipv];
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
                *numpvObjs = 0;
            }
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return status;
}


MLSTATUS ML_getMeshFaceInfo(
    MeshAssociativityObj meshAssocObj,
    MeshEdgeObj meshFaceObj,
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
    int status = ML_STATUS_OK;
    *numAttIDs = 0;
    *numpvObjs = 0;
    MLINT ipv;
    for (ipv = 0; ipv < sizepvObjs; ++ipv) {
        pvObjs[ipv] = NULL;
    }

    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    MeshFace *meshFace = (MeshFace *)(meshFaceObj);
    if (meshAssoc && meshFace && attIDs && numAttIDs) {
        const std::string &refStr = meshFace->getRef();
        copyString(refBuf, refBufLen, refStr);

        const std::string &nameStr = meshFace->getName();
        copyString(nameBuf, nameBufLen, nameStr);

        *gref = meshFace->getGref();
        *mid = meshFace->getID();

        if (ML_STATUS_OK == status) {
            std::vector<MLINT> attIDvec = meshFace->getAttributeIDs(*meshAssoc);
            *numAttIDs = (MLINT)attIDvec.size();
            if (*numAttIDs <= sizeAttIDs) {
                MLINT iAtt;
                for (iAtt = 0; iAtt < *numAttIDs; ++iAtt) {
                    attIDs[iAtt] = attIDvec[iAtt];
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
                *numAttIDs = 0;
            }
        }

        if (ML_STATUS_OK == status) {
            std::vector<ParamVertex*> PVvec;
            meshFace->getParamVerts(PVvec);
            MLINT num = (MLINT)PVvec.size();
            if (num <= sizeAttIDs) {
                *numpvObjs = 0;
                for (ipv = 0; ipv < num; ++ipv) {
                    if (NULL != PVvec[ipv]) {
                        pvObjs[(*numpvObjs)++] = PVvec[ipv];
                    }
                }
                status = ML_STATUS_OK;
            }
            else {
                /* insufficient space */
                status = ML_STATUS_ERROR;
            }
        }
    }
    else {
        return ML_STATUS_ERROR;
    }
    return status;
}


MLINT  ML_getNumGeometryGroups(
    MeshAssociativityObj meshAssocObj)
{
    MLINT count = 0;
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc) {
        count = meshAssoc->getGeometryGroupCount();
    }
    return count;
}


MLSTATUS ML_getGeometryGroupIDs(MeshAssociativityObj meshAssocObj,
    MLINT IDs[],
    MLINT sizeIDs,
    MLINT *numIDs
)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && IDs && numIDs) {

        std::vector<MLINT> IDvec;
        meshAssoc->getGeometryGroupIDs(IDvec);
            
        *numIDs = (MLINT)IDvec.size();
        if (*numIDs <= sizeIDs) {
            MLINT i;
            for (i = 0; i < *numIDs; ++i) {
                IDs[i] = IDvec[i];
            }
            return ML_STATUS_OK;
        }
        else {
            /* insufficient space */
            return ML_STATUS_ERROR;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_getGeometryGroupByID(
    MeshAssociativityObj meshAssocObj,
    MLINT gid,
    GeometryGroupObj *geomGroupObj
)
{
    MeshAssociativity *meshAssoc = (MeshAssociativity *)meshAssocObj;
    if (meshAssoc && geomGroupObj) {
        *geomGroupObj = meshAssoc->getGeometryGroupByID(gid);
        if (*geomGroupObj != NULL) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getEntityNames(
    GeometryGroupObj geomGroupObj,
    char *entityNamesBufArr,   /* array of entity name buffers */
    MLINT entityNamesArrLen,   /* length of names array */
    MLINT entityNameBufLen,    /* buffer length of a each name */
    MLINT *num_entityNames
)
{
    int status = ML_STATUS_OK;
    *num_entityNames = 0;
    MLINT i;

    GeometryGroup *geom_group = (GeometryGroup *)geomGroupObj;
    if (geom_group) {
        const std::set<std::string> &entity_names = geom_group->getEntityNameSet();
        *num_entityNames = entity_names.size();
        if (*num_entityNames > entityNamesArrLen) {
            *num_entityNames = 0;
            return ML_STATUS_ERROR;
        }


        std::set<std::string>::const_iterator iter;
        for (i = 0, iter = entity_names.begin(); iter != entity_names.end(); ++i, ++iter) {
            copyString( &(entityNamesBufArr[i*entityNameBufLen]), entityNameBufLen, (*iter) );
        }
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

MLTYPE ML_getEntityType(
    GeometryKernelObj geomKernelObj,
    const char *entityName
)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        return geomKernel->entityType(entityName);
    }
    return ML_TYPE_UNKNOWN;
}

MLSTATUS ML_entityExists(
    GeometryKernelObj geomKernelObj,
    const char *entityName
)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->entityExists(entityName)) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_evalXYZ(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,
    const char *entityName,
    MLVector3D XYZ)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->evalXYZ(UV, entityName, XYZ)) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_evalRadiusOfCurvature(
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
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_evalDerivativesOnCurve(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,                  /* Evaluation parametric location on curve */
    const char *entityName,         /* Evaluation entity name */
    MLVector3D        XYZ,          /* Evaluated location on curve */
    MLVector3D        dXYZdU,       /* First derivative */
    MLVector3D        d2XYZdU2      /* Second derivative */
)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->evalDerivativesOnCurve(UV, entityName,
            XYZ, dXYZdU, d2XYZdU2)) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_evalCurvatureOnCurve(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,                  /* Evaluation parametric location on curve */
    const char *entityName,         /* Evaluation entity name */
    MLVector3D             XYZ,     /* Evaluated location on curve */
    MLVector3D         Tangent,     /* tangent to curve  */
    MLVector3D PrincipalNormal,     /* principal normal (pointing towards the center of curvature) */
    MLVector3D        Binormal,     /* binormal (tangent x principal normal) */

    /* curvature in radians per unit length
     * ALWAYS non-negative and in the direction of the principal normal
     * Radius of curvature = 1 / Curvature
     */
    MLREAL  *Curvature,
    MLINT   *Linear           /* If non-zero, the curve is linear and has no unique normal */

)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        bool isLinear = false;
        *Linear = 0;
        if (geomKernel->evalCurvatureOnCurve(UV, entityName,
            XYZ, Tangent, PrincipalNormal, Binormal, Curvature, &isLinear)) {
            if (isLinear) {
                *Linear = 1;
            }
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

/* Evaluate the curvature on a surface entity at the parametric coordinates */
MLSTATUS ML_evalCurvatureOnSurface(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,                      /* Evaluation parametric location on surface */
    const char *entityName,             /* Evaluation entity name */
    MLVector3D        XYZ,              /* Evaluated location on surface */
    MLVector3D        dXYZdU,           /* First partial derivative */
    MLVector3D        dXYZdV,           /* First partial derivative */
    MLVector3D        d2XYZdU2,         /* Second partial derivative */
    MLVector3D        d2XYZdUdV,        /* Second partial derivative */
    MLVector3D        d2XYZdV2,         /* Second partial derivative */
    MLVector3D        surfaceNormal,    /* Surface normal - unit vector */
    /* Unit vector tangent to surface where curvature = min
     * surfaceNormal cross principalV yields the direction where curvature = max
     * if the surface is locally planar (min and max are 0.0) or if the
     * surface is locally spherical (min and max are equal),
     * this will be an arbitrary vector tangent to the surface
     */
    MLVector3D        principalV,
    /* Minimum and maximum curvature, in radians per unit length
     * Defined so that positive values indicate the surface bends
     * in the direction of surfaceNormal, and negative values indicate
     * the surface bends away from surfaceNormal
     */
    MLREAL          *minCurvature,
    MLREAL          *maxCurvature,
    /* The average or mean curvature is defined as :
     *    avg = (min + max) / 2
     * The Gaussian curvature is defined as :
     *    gauss = min * max
     */
    MLREAL          *avg,               /* Average curvature */
    MLREAL          *gauss,             /*  Gaussian curvature */
    MLORIENT        *orientation        /* Orientation of surface in model */
)
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->evalCurvatureOnSurface(UV, entityName,
            XYZ,
            dXYZdU, dXYZdV,
            d2XYZdU2, d2XYZdUdV, d2XYZdV2,
            surfaceNormal, principalV,
            minCurvature, maxCurvature, avg, gauss,
            orientation)) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}

/**
 * \brief Evaluate model assembly tolerances on geometric surface.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] entityName surface name
 * @param[out] minTolerance minimum assembly tolerance on boundary
 * @param[out] maxCurvature maximum assembly tolerance on boundary
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_evalSurfaceTolerance(
    GeometryKernelObj geomKernelObj,
    const char *entityName,             /* Evaluation surface name */
    MLREAL          *minTolerance,
    MLREAL          *maxTolerance
    )
{
    GeometryKernel *geomKernel = (GeometryKernel *)geomKernelObj;
    if (geomKernel) {
        if (geomKernel->evalSurfaceTolerance(entityName,
            *minTolerance, *maxTolerance)) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
}



MLSTATUS ML_projectPoint(
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
            return ML_STATUS_ERROR;
        }
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}


MLSTATUS ML_getProjectionInfo(
    GeometryKernelObj geomKernelObj,
    ProjectionDataObj projectionDataObj,
    MLVector3D xyz,
    MLVector2D UV,
    char *entityNameBuf, MLINT entityNameBufLen,
    MLREAL *distance,
    MLREAL *tolerance
)
{
    GeometryKernel *geom_kernel = (GeometryKernel *)geomKernelObj;
    ProjectionData *projectionData = (ProjectionData *)projectionDataObj;
    if (geom_kernel && projectionData) {
        std::string name;
        if (!geom_kernel->getProjectionXYZ(*projectionData, xyz) ||
            !geom_kernel->getProjectionUV(*projectionData, UV) ||
            !geom_kernel->getProjectionEntityName(*projectionData, name)) {
            return ML_STATUS_ERROR;
        }
        if (NULL != distance && !geom_kernel->getProjectionDistance(*projectionData, *distance)) {
            return ML_STATUS_ERROR;
        }
        if (NULL != tolerance && !geom_kernel->getProjectionTolerance(*projectionData, *tolerance)) {
            return ML_STATUS_ERROR;
        }
        copyString(entityNameBuf, entityNameBufLen, name);
        return ML_STATUS_OK;
    }
    return ML_STATUS_ERROR;
}

MLSTATUS ML_createProjectionDataObj(
    GeometryKernelObj geomKernelObj,
    ProjectionDataObj *projectionDataObj
)
{
    GeometryKernel *geom_kernel = (GeometryKernel *)geomKernelObj;
    if (geom_kernel && projectionDataObj) {
        *projectionDataObj = (ProjectionDataObj) new ProjectionData(geom_kernel);
        if (*projectionDataObj) {
            return ML_STATUS_OK;
        }
    }
    return ML_STATUS_ERROR;
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
