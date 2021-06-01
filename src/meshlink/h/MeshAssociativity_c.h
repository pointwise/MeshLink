/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#ifndef MESH_ASSOCIATIVITY_C_API
#define MESH_ASSOCIATIVITY_C_API

#include "Types.h"
#include <stdlib.h>


/**
 * \brief Checks MeshLink Caller data sizes against MeshLink library.
 * @param[in] size_of_MLINT sizeof(MLINT)
 * @param[in] size_of_MLINT32 sizeof(MLINT32)
 * @param[in] size_of_MLFLOAT sizeof(MLFLOAT)
 * @param[in] size_of_MLREAL sizeof(MLREAL)
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_checkDataSizes(
    size_t size_of_MLINT,
    size_t size_of_MLINT32,
    size_t size_of_MLFLOAT,
    size_t size_of_MLREAL
);


/**
 * \brief Allocates a new MeshAssociativity Object.
 * @param[in,out] meshAssocObj MeshAssociativity object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_createMeshAssociativityObj( MeshAssociativityObj *meshAssocObj ) ;

/**
 * \brief Frees a MeshAssociativity Object.
 * @param[in,out] meshAssocObj MeshAssociativity object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
void   ML_freeMeshAssociativityObj(MeshAssociativityObj *meshAssocObj) ;


/**
 * \brief Validate a MeshLink XML file against the schema.
 * @param[in] parseObj MeshLinkParser object
 * @param[in] meshlinkFilename meshlink filename
 * @param[in] schemaFilename schema filename
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS   ML_parserValidateFile(
        MeshLinkParserObj parseObj,
        const char *meshlinkFilename,
        const char *schemaFilename);

/**
 * \brief Read a MeshLink XML file into a MeshAssociativity object.
 * @param[in] parseObj MeshLinkParser object
 * @param[in] meshlinkFilename meshlink filename
 * @param[in,out] meshAssocObj MeshAssociativity object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS    ML_parserReadMeshLinkFile(
        MeshLinkParserObj parseObj,
        const char *meshlinkFilename,
        MeshAssociativityObj meshAssocObj);

/**
 * \brief Get MeshLink schema attributes.
 * @param[in] parseObj MeshLinkParser object
 * @param[in,out] xmlns XML name space buffer
 * @param[in] xmlnsBufLen XML name space buffer length
 * @param[in,out] xmlns_xsi XML type space buffer
 * @param[in] xmlns_xsiBufLen XML type space buffer length
 * @param[in,out] schemaLocation schema location buffer
 * @param[in] schemaBufLen schema location buffer length
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS    ML_parserGetMeshLinkAttributes(
        MeshLinkParserObj parseObj,
        char *xmlns, MLINT xmlnsBufLen,
        char *xmlns_xsi, MLINT xmlns_xsiBufLen,
        char *schemaLocation, MLINT schemaBufLen);

/**
 * \brief Write MeshAssociativity data to XML file.
 * @param[in] writeObj MeshLinkWriter object
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] meshlinkFilename meshlink filename
 * @param[in,out] xmlns XML name space 
 * @param[in,out] xmlns_xsi XML type space
 * @param[in,out] schemaLocation schema location
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS    ML_writerWriteMeshLinkFile(
        MeshLinkWriterObj writeObj,
        MeshAssociativityObj meshAssocObj,
        const char *meshlinkFilename,
        const char *xmlns,
        const char *xmlns_xsi,
        const char *schemaLocation);

/**
 * \brief Get MeshModel object by name.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] modelname Mesh Model name
 * @param[out] meshModel MeshModel object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshModelByName(MeshAssociativityObj meshAssocObj, 
    const char *modelname, MeshModelObj *meshModel);


/**
 * \brief Get MeshSheet object by name.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] sheetname Mesh Sheet name
 * @param[out] meshModel MeshModel object containing sheet
 * @param[out] meshSheet MeshSheet object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshSheetByName(MeshAssociativityObj meshAssocObj,
    const char *sheetname, 
    MeshModelObj *meshModel, MeshSheetObj *meshSheet);

/**
 * \brief Get MeshString object by name.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] stringname Mesh String name
 * @param[out] meshModel MeshModel object containing string
 * @param[out] meshString MeshString object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshStringByName(MeshAssociativityObj meshAssocObj,
    const char *stringname, 
    MeshModelObj *meshModel, MeshStringObj *meshString);


/**
 * \brief Add GeometryKernel object to MeshAssociativity.
 * @param[in,out] meshAssocObj MeshAssociativity object
 * @param[in]     geomKernelObj GeometryKernel object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_addGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj);

/**
 * \brief Remove GeometryKernel object from MeshAssociativity.
 * @param[in,out] meshAssocObj MeshAssociativity object
 * @param[in] geomKernelObj GeometryKernel object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_removeGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj);

/**
 * \brief Set MeshAssociativity active Geometry Kernel by name.
 * @param[in,out] meshAssocObj MeshAssociativity object
 * @param[in] kernelname Geometry Kernel name
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_setActiveGeometryKernelByName(MeshAssociativityObj meshAssocObj, const char *kernelname);

/**
 * \brief Get MeshAssociativity active Geometry Kernel object.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[out] geomKernelObj GeometryKernel object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getActiveGeometryKernel(
        MeshAssociativityObj meshAssocObj,
        GeometryKernelObj *geomKernelObj);


/**
 * \brief Get number of geometry files referenced by the MeshAssociativity.
 * @param[in] meshAssocObj MeshAssociativity object
 * \returns File count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumGeometryFiles(MeshAssociativityObj meshAssocObj);

/**
 * \brief Get GeometryFile refereneced in MeshAssociativity by index.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] index geometry file index
 * @param[out] fileObj GeometryFile object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getGeometryFileObj(MeshAssociativityObj meshAssocObj,
    MLINT index,
    MeshLinkFileConstObj *fileObj);

/**
 * \brief Get file name for a generic MeshLinkFile.
 * @param[in] fileObj MeshLinkFile object (could be GeometryFile or MeshFile)
 * @param[in,out] filenameBuf file name buffer
 * @param[in] filenameBufLen file name buffer length
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getFilename( MeshLinkFileConstObj fileObj, char *filenameBuf, MLINT filenameBufLen );


/**
 * \brief Set the model size attribute in the GeometryKernel.
 * Model size (extents) is used to define tolerances related to model closure, e.g. same point tolerance.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] modelSize model size
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_setGeomModelSize(
    GeometryKernelObj geomKernelObj,
    MLREAL modelSize);

/**
 * \brief Get the model size attribute in the GeometryKernel.
 * Model size is used to define tolerances related to model closure, e.g. same point tolerance.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[out] modelSize model size
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getGeomModelSize(
    GeometryKernelObj geomKernelObj,
    MLREAL *modelSize);

/**
 * \brief Read a GeometryFile into the GeometryKernel.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] geomFilename geometry file name
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_readGeomFile(
        GeometryKernelObj geomKernelObj,
        const char *geomFilename);

/**
 * \brief Get array of attribute IDs for a generic MeshLinkFile.
 * @param[in] meshAssocObj MeshAssociativity object 
 * @param[in] fileObj MeshLinkFile object (could be GeometryFile or MeshFile)
 * @param[in,out] attIDs attribute ID array
 * @param[in] sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute ID count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getFileAttIDs(MeshAssociativityObj meshAssocObj,
        MeshLinkFileConstObj fileObj,
        MLINT attIDs[],
        MLINT sizeAttIDs,
        MLINT *numAttIDs  );

/**
 * \brief Get attribute name-value pair by ID.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] attID attribute ID
 * @param[in,out] attNameBuf attribute name buffer
 * @param[in,out] attNameBufLen attribute name buffer length
 * @param[in,out] attValueBuf attribute value buffer
 * @param[in,out] attValueBufLen attribute value buffer length
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getAttribute(MeshAssociativityObj meshAssocObj,
        MLINT attID,
        char *attNameBuf, MLINT attNameBufLen,
        char *attValueBuf, MLINT attValueBufLen
    );


/**
 * \brief Get number of MeshModels in the MeshAssociativity.
 * @param[in] meshAssocObj MeshAssociativity object
 * \returns MeshModel count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumMeshModels(MeshAssociativityObj meshAssocObj );

/**
 * \brief Get array of MeshModels in the MeshAssociativity.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in,out] modelObjs MeshModel object array
 * @param[in] sizeModelObj MeshModel object array size
 * @param[out] numModelObjs MeshModel object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshModels(MeshAssociativityObj meshAssocObj,
        MeshTopoObj *modelObjs,
        MLINT sizeModelObj,
        MLINT *numModelObjs
    );

/**
 * \brief Get number of ParamVertex objects in the MeshTopo.
 * @param[in] meshTopoObj MeshTopo derived object
 * \returns ParamVertex count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumParamVerts(MeshTopoObj meshTopoObj
    );

/**
 * \brief Get number of MeshSheets in the MeshModel.
 * @param[in] meshModelObj MeshModel object
 * \returns MeshSheet count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
    MLINT ML_getNumMeshSheets(MeshModelObj meshModelObj
    );

/**
 * \brief Get array of MeshSheets in the MeshModel.
 * @param[in] meshModelObj MeshModel object
 * @param[in,out] sheetObjs MeshSheet object array
 * @param[in] sizeSheetObj MeshSheet object array size
 * @param[out] numSheetObjs MeshSheet object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshSheets(MeshModelObj meshModelObj,
        MeshSheetObj *sheetObjs,
        MLINT sizeSheetObj,
        MLINT *numSheetObjs
    );

/**
 * \brief Get a MeshSheet in the MeshModel by name.
 * @param[in] meshModelObj MeshModel object
 * @param[in] sheetname MeshSheet name to find
 * @param[in,out] meshSheetObj MeshSheet object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getModelMeshSheetByName(MeshModelObj meshModelObj,
    const char *sheetname,
    MeshSheetObj *meshSheetObj);

/**
 * \brief Get a MeshString in the MeshModel by name.
 * @param[in] meshModelObj MeshModel object
 * @param[in] stringname MeshString name to find
 * @param[in,out] meshSheetObj MeshString object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getModelMeshStringByName(MeshModelObj meshModelObj,
    const char *stringname,
    MeshStringObj *meshStringObj);


/**
 * \brief Get number of MeshFaces in the MeshSheet.
 * @param[in] meshSheetObj MeshSheet object
 * \returns MeshFace count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumSheetMeshFaces(MeshSheetObj meshSheetObj
);

/**
 * \brief Get array of MeshFaces in the MeshSheet.
 * @param[in] meshSheetObj MeshSheet object
 * @param[in,out] faceObjs MeshFace object array
 * @param[in] sizeFaceObj MeshFace object array size
 * @param[out] numFaceObjs MeshFace object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getSheetMeshFaces(MeshSheetObj meshSheetObj,
    MeshTopoObj *faceObjs,
    MLINT sizeFaceObj,
    MLINT *numFaceObjs
);

/**
 * \brief Get number of MeshFaceEdges in the MeshSheet.
 * @param[in] meshSheetObj MeshSheet object
 * \returns MeshFaceEdge count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumSheetMeshFaceEdges(MeshSheetObj meshSheetObj
);

/**
 * \brief Get array of MeshFaceEdges in the MeshSheet.
 * @param[in] meshSheetObj MeshSheet object
 * @param[in,out] faceEdgeObjs MeshFaceEdge object array
 * @param[in] sizeFaceEdgeObj MeshFaceEdge object array size
 * @param[out] numFaceEdgeObjs MeshFaceEdge object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getSheetMeshFaceEdges(MeshSheetObj meshSheetObj,
    MeshTopoObj *faceEdgeObjs,
    MLINT sizeFaceEdgeObj,
    MLINT *numFaceEdgeObjs
);


/**
 * \brief Get number of MeshStrings in the MeshModel.
 * @param[in] meshModelObj MeshModel object
 * \returns MeshString count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumMeshStrings(MeshModelObj meshModelObj
);

/**
 * \brief Get array of MeshStrings in the MeshModel.
 * @param[in] meshModelObj MeshModel object
 * @param[in,out] stringObjs MeshString object array
 * @param[in] sizeStringObj MeshString object array size
 * @param[out] numStringObjs MeshString object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshStrings(MeshModelObj meshModelObj,
    MeshTopoObj stringObjs[],
    MLINT sizeStringObj,
    MLINT *numStringObjs
);


/**
 * \brief Get number of MeshEdges in the MeshString.
 * @param[in] meshStringObj MeshString object
 * \returns MeshEdge count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT ML_getNumStringMeshEdges(MeshSheetObj meshStringObj
);

/**
 * \brief Get array of MeshEdges in the MeshString.
 * @param[in] meshStringObj MeshString object
 * @param[in,out] edgeObjs MeshEdge object array
 * @param[in] sizeEdgeObj MeshEdge object array size
 * @param[out] numEdgeObjs MeshEdge object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getStringMeshEdges(MeshSheetObj meshStringObj,
    MeshTopoObj edgeObjs[],
    MLINT sizeEdgeObj,
    MLINT *numEdgeObjs
);



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
);

/**
 * \brief Get number of MeshElementLinkages in the MeshAssociativity database.
 * @param[in] meshAssocObj MeshAssociativity object
 * \returns MeshElementLinkage count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT  ML_getNumMeshElementLinkages(
    MeshAssociativityObj meshAssocObj);


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
);


/**
 * \brief Get info associated with a MeshElementLinkage
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] linkageObj MeshElementLinkage object
 * @param[in, out] nameBuf name buffer
 * @param[in]  nameBufLen name buffer length
 * @param[in,out] sourceEntityRefBuf Source mesh entity reference buffer
 * @param[in] sourceEntityRefBufLen Source mesh entity reference buffer size
 * @param[in,out] targetEntityRefBuf Target mesh entity reference buffer
 * @param[in] targetEntityRefBufLen Target mesh entity reference buffer size
 * @param[in,out] attIDs attribute ID array
 * @param[in]  sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshElementLinkageInfo(
    MeshAssociativityObj meshAssocObj,
    MeshElementLinkageObj linkageObj,
    char *nameBuf, MLINT nameBufLen,
    char *sourceEntityRefBuf, MLINT sourceEntityRefBufLen,
    char *targetEntityRefBuf, MLINT targetEntityRefBufLen,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs);

/**
 * \brief Get info associated with a MeshLinkTransform
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] xformObj MeshLinkTransform object
 * @param[in, out] nameBuf name buffer
 * @param[in]  nameBufLen name buffer length
 * @param[in,out] attIDs attribute ID array
 * @param[in]  sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshLinkTransformInfo(
    MeshAssociativityObj meshAssocObj,
    MeshLinkTransformConstObj xformObj,
    char *nameBuf, MLINT nameBufLen,
    MLINT attIDs[],
    MLINT sizeAttIDs,
    MLINT *numAttIDs);


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
);


/**
 * \brief Find a point index's lowest topological reference.
 * Returns first usage in the order MeshString, MeshSheet, MeshModel.
 * @param[in] meshModelObj MeshModel object
 * @param[in] pointIndex Point index
 * @param[out] meshPointObj MeshPoint object
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_findLowestTopoPointByInd(MeshModelObj meshModelObj,
        MLINT pointIndex, MeshPointObj* meshPointObj);

/**
 * \brief Find a point index's highest topological reference.
 * Returns first usage in the order MeshModel, MeshSheet, MeshString.
 * @param[in] meshModelObj MeshModel object
 * @param[in] pointIndex Point index
 * @param[out] meshPointObj MeshPoint object
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_findHighestTopoPointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex, MeshPointObj* meshPointObj);

/**
 * \brief Find a point index reference in a MeshString's MeshEdge.
 * Returns first occurance of index in a MeshString's MeshEdge array.
 * @param[in] meshModelObj MeshModel object
 * @param[in] pointIndex Point index
 * @param[out] meshPointObj MeshPoint object
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_findMeshEdgePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex, MeshPointObj* meshPointObj);

/**
 * \brief Find a point index reference in a MeshSheet's MeshFace.
 * Returns first occurance of index in a MeshSheet's MeshFace array.
 * @param[in] meshModelObj MeshModel object
 * @param[in] pointIndex Point index
 * @param[out] meshPointObj MeshPoint object
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_findMeshFacePointByInd(MeshModelObj meshModelObj,
    MLINT pointIndex, MeshPointObj* meshPointObj);

/**
 * \brief Return array of MeshFace point indices.
 * @param[in] meshFaceObj MeshFace object
 * @param[in,out] inds Point index array (size 4)
 * @param[out] numInds Point index count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getFaceInds(MeshFaceObj meshFaceObj,
    MLINT inds[],       /* array of size 4 */
    MLINT *numInds
);

/**
 * \brief Return array of MeshEdge point indices.
 * @param[in] meshEdgeObj MeshEdge object
 * @param[in,out] inds Point index array (size 2)
 * @param[out] numInds Point index count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getEdgeInds(MeshEdgeObj meshEdgeObj,
    MLINT inds[],       /* array of size 2 */
    MLINT *numInds
);


/**
 * \brief Find a MeshFace in a MeshModel by point indices.
 * Returns first occurance of indices in a MeshSheet's MeshFace.
 * @param[in] meshModelObj MeshModel object
 * @param[in] indices Point index array
 * @param[in] numIndices Number of point indices
 * @param[out] meshFaceObj MeshFace object
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_findFaceByInds(MeshModelObj meshModelObj,
    MLINT *indices, MLINT numIndices,   /* python wrapper depends on these names */
    MeshFaceObj *meshFaceObj);

/**
 * \brief Find a MeshEdge's lowest topological reference by point indices.
 * Returns first usage in the order MeshString, MeshSheet, MeshModel.
 * @param[in] meshModelObj MeshModel object
 * @param[in] indices Point index array
 * @param[in] numIndices Number of point indices
 * @param[out] meshEdgeObj MeshEdge object
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_findLowestTopoEdgeByInds(MeshModelObj meshModelObj,
        MLINT *indices, MLINT numIndices,  /* python wrapper depends on these names */
        MeshEdgeObj *meshEdgeObj);

/**
 * \brief Return array of Parametric Vertices for a MeshTopo object.
 * MeshTopo object may be any MeshTopo derived object, e.g. MeshSheet, MeshFace.
 * @param[in] meshTopoObj MeshTopo object
 * @param[in,out] pvObjsArr ParamVertex array 
 * @param[in] pvObjsArrLen ParamVertex array size
 * @param[out] num_pvObjs ParamVertex count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getParamVerts(MeshTopoConstObj meshTopoObj,
        ParamVertexConstObj pvObjsArr[],  /* array of ParamVertexConstObj */
        MLINT pvObjsArrLen,               /* length of array */
        MLINT *num_pvObjs );

/**
 * \brief Get Parametric Vertex information.
 * @param[in] pvObj ParamVertex object
 * @param[in,out] vrefBuf MeshPoint reference buffer
 * @param[in] vrefBufLen MeshPoint reference buffer size
 * @param[out] gref GeometryGroup ID
 * @param[out] mid ID
 * @param[out] UV Parametric coordinates in GeometryGroup entity
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getParamVertInfo(ParamVertexConstObj pvObj,
        char *vrefBuf, MLINT vrefBufLen,
        MLINT *gref,
        MLINT *mid,
        MLVector2D UV);


/**
 * \brief Get GeometryGroup ID for a MeshTopo object.
 * MeshTopo object may be a MeshPoint, MeshEdge, MeshFace object.
 * @param[in] meshTopoObj MeshTopo object
 * @param[out] gref GeometryGroup ID
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshTopoGref(
        MeshTopoObj meshTopoObj,
        MLINT *gref);

/**
 * \brief Get MeshTopo information.
 * MeshTopo object may be a MeshPoint, MeshEdge, MeshFace object.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] meshTopoObj MeshTopo object
 * @param[in,out] refBuf reference buffer
 * @param[in]  refBufLen reference buffer size
 * @param[in,out] nameBuf name buffer
 * @param[in]  nameBufLen name buffer length
 * @param[out] gref GeometryGroup ID
 * @param[out] mid ID
 * @param[in,out] attIDs attribute ID array
 * @param[in]  sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getMeshTopoInfo(
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


/**
 * \brief Get MeshPoint information.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] meshPointObj MeshPoint object
 * @param[in,out] refBuf reference buffer
 * @param[in]  refBufLen reference buffer length
 * @param[in,out] nameBuf name buffer
 * @param[in]  nameBufLen  name buffer length
 * @param[out] gref GeometryGroup ID
 * @param[out] mid ID
 * @param[in,out] attIDs attribute ID array
 * @param[in]  sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute count
 * @param[out] pvObj ParamVertex object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
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
    ParamVertexConstObj *pvObj);


/**
 * \brief Get MeshEdge information.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] meshEdgeObj MeshEdge object
 * @param[in,out] refBuf reference buffer
 * @param[in]  refBufLen  reference buffer length
 * @param[in,out] nameBuf name buffer
 * @param[in]  nameBufLen name buffer length
 * @param[out] gref GeometryGroup ID
 * @param[out] mid ID
 * @param[in,out] attIDs attribute ID array
 * @param[in]  sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute count
 * @param[in,out] pvObjs ParamVertex object array
 * @param[in] sizepvObjs ParamVertex object array size
 * @param[out] numpvObjs ParamVertex object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
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
    MLINT *numpvObjs);


/**
 * \brief Get MeshFace information.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] meshFaceObj MeshFace object
 * @param[in,out] refBuf reference buffer
 * @param[in]  refBufLen  reference buffer length
 * @param[in,out] nameBuf name buffer
 * @param[in]  nameBufLen name buffer length
 * @param[out] gref GeometryGroup ID
 * @param[out] mid ID
 * @param[in,out] attIDs attribute ID array
 * @param[in]  sizeAttIDs attribute ID array size
 * @param[out] numAttIDs attribute count
 * @param[in,out] pvObjs ParamVertex object array
 * @param[in] sizepvObjs ParamVertex object array size
 * @param[out] numpvObjs ParamVertex object count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
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
    MLINT *numpvObjs);


/**
 * \brief Get number of GeometryGroups in the MeshAssociativity database.
 * @param[in] meshAssocObj MeshAssociativity object
 * \returns GeometryGroup count
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLINT  ML_getNumGeometryGroups(
    MeshAssociativityObj meshAssocObj);

/**
 * \brief Get array of GeometryGroup IDs in the MeshAssociativity database.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in,out] IDs GeometryGroup ID array
 * @param[in] sizeIDs GeometryGroup ID array size
 * @param[out] numIDs GeometryGroup ID count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS  ML_getGeometryGroupIDs(
    MeshAssociativityObj meshAssocObj,
    MLINT IDs[],
    MLINT sizeIDs,
    MLINT *numIDs);


/**
 * \brief Get GeometryGroup by ID.
 * @param[in] meshAssocObj MeshAssociativity object
 * @param[in] gid Geometry Group ID
 * @param[out] geomGroupObj GeometryGroup object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getGeometryGroupByID(
        MeshAssociativityObj meshAssocObj,
        MLINT gid,
        GeometryGroupObj *geomGroupObj
    );

/**
 * \brief Get array of entity names referenced by GeometryGroup.
 * @param[in] geomGroupObj GeometryGroup object
 * @param[in,out] entityNamesBufArr entity name buffer array
 * @param[in]  entityNamesArrLen entity name buffer array size
 * @param[in]  entityNameBufLen entity name buffer length
 * @param[out] num_entityNames entity name count
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getEntityNames(
        GeometryGroupObj geomGroupObj,
        char *entityNamesBufArr,      /* array of entity name buffers */
        MLINT entityNamesArrLen,   /* length of names array */
        MLINT entityNameBufLen,    /* buffer length of a each name */
        MLINT *num_entityNames
    );


/**
 * \brief Evaluate XYZ at parametric coordinates on geometric entity.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] UV            2D parametric coordinates (2nd coord unused for 1D entities)
 * @param[in] entityName    entity name
 * @param[out] XYZ          location
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_evalXYZ(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,
    const char *entityName,
    MLVector3D XYZ);

/**
 * \brief Evaluate radius of curvature at parametric coordinates on geometric entity.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] UV 2D parametric coordinates (2nd coord unused for 1D entities)
 * @param[in] entityName entity name
 * @param[out] minRradOfCurvature minimum radius of curvature
 * @param[out] maxRradOfCurvature maximum radius of curvature
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_evalRadiusOfCurvature(
        GeometryKernelObj geomKernelObj,
        MLVector2D UV,
        const char *entityName,
        MLREAL *minRradOfCurvature,
        MLREAL *maxRradOfCurvature);


/**
 * \brief Evaluate curvature at parametric coordinate on geometric curve.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] UV 2D parametric coordinates (2nd coord unused)
 * @param[in] entityName entity name
 * @param[out] XYZ location on curve
 * @param[out] Tangent unit vector of tangent to curve
 * @param[out] PrincipalNormal unit vector of principal normal to curve (pointing towards the center of curvature)
 * @param[out] Binormal unit vector of binormal to curve (tangent x principal normal)
 * @param[out] Curvature curvature in radians per unit length
 * @param[out] Linear whether the curve is linear and has no unique normal
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
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
);

/**
 * \brief Evaluate parametric derivatives on curve.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] UV 2D parametric coordinates
 * @param[in] entityName entity name
 * @param[out] XYZ XYZ location on curve
 * @param[out] dXYZdU first derivative
 * @param[out] d2XYZdU2 second derivative
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_evalDerivativesOnCurve(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,                  /* Evaluation parametric location on curve */
    const char *entityName,         /* Evaluation entity name */
    MLVector3D        XYZ,          /* Evaluated location on curve */
    MLVector3D        dXYZdU,       /* First derivative */
    MLVector3D        d2XYZdU2      /* Second derivative */
);



/**
 * \brief Evaluate curvature at parametric coordinate on geometric surface.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] UV 2D parametric coordinates 
 * @param[in] entityName entity name
 * @param[out] XYZ location on curve
 * @param[out] dXYZdU first partial derivative w.r.t. U
 * @param[out] dXYZdV first partial derivative w.r.t. V
 * @param[out] d2XYZdU2 second partial derivative w.r.t. U
 * @param[out] d2XYZdUdV second partial derivative w.r.t. UV
 * @param[out] d2XYZdV2 second partial derivative w.r.t. V
 * @param[out] surfaceNormal unit vector of surface normal
 * @param[out] principalV unit vector tangent to surface where curvature = min
 * @param[out] minCurvature minimum curvature in radians per unit length
 * @param[out] maxCurvature maximum curvature in radians per unit length
 * @param[out] avg average curvature in radians per unit length
 * @param[out] gauss Gaussian curvature 
 * @param[out] orientation orientation of the surface in the model
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
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
    MLREAL          *gauss,             /* Gaussian curvature */
    MLORIENT        *orientation        /* Orientation of surface in model */
);


/**
 * \brief Evaluate model assembly tolerances on geometric surface.
 * If the surface name given matches a model face, then the face's 
 * boundary edges and vertices will be querried for the assembly 
 * tolerance used to close the model.
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
);


/**
 * \brief Allocates a new GeometryKernel-specific ProjectionData Object.
 * A ProjectionData object is used to store inverse evaluation data 
 * specific to the GeometryKernel.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in,out] projectionDataObj ProjectionData object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_createProjectionDataObj(
        GeometryKernelObj geomKernelObj,
        ProjectionDataObj *projectionDataObj
    );

/**
 * \brief Frees a  GeometryKernel-specific ProjectionData Object.
 * @param[in,out] projectionDataObj ProjectionData object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
    void ML_freeProjectionDataObj(ProjectionDataObj *projectionDataObj);


/**
 * \brief Closest point projection onto a GeometryGroup.
 * A ProjectionData object is used to store inverse evaluation data
 * specific to the GeometryKernel.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] geomGroupObj GeometryGroup object
 * @param[in] point XYZ location
 * @param[in,out] projectionDataObj ProjectionData object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_projectPoint(
    GeometryKernelObj geomKernelObj,
    GeometryGroupObj geomGroupObj,
    MLVector3D point,
    ProjectionDataObj projectionDataObj
);

/**
 * \brief Get info from closest point projection onto a GeometryGroup
 * stored in a ProjectionData object specific to the GeometryKernel.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] projectionDataObj ProjectionData object
 * @param[out] xyz location
 * @param[out] UV parametric coordinates
 * @param[in,out] entityNameBuf geoemtry entity name buffer
 * @param[in] entityNameBufLen  geoemtry entity name buffer length
 * @param[out] distance the distance between the original point and projected point (can be NULL)
 * @param[out] tolerance the error tolerance of the projection result (can be NULL)
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_getProjectionInfo(
    GeometryKernelObj geomKernelObj,
    ProjectionDataObj projectionDataObj,
    MLVector3D xyz,
    MLVector2D UV,
    char *entityNameBuf, MLINT entityNameBufLen,
    MLREAL *distance,
    MLREAL *tolerance
);


/**
 * \brief Get the geomtric type of an entity by name.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] entityName geoemtry entity name 
 * \returns entity type
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLTYPE ML_getEntityType(
    GeometryKernelObj geomKernelObj,
    const char *entityName
);

/**
 * \brief Determine if a geometric entity exists by name.
 * @param[in] geomKernelObj GeometryKernel object
 * @param[in] entityName geometry entity name
 * \returns ML_STATUS_OK if found
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
MLSTATUS ML_entityExists(
    GeometryKernelObj geomKernelObj,
    const char *entityName
);

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
