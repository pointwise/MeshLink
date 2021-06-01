/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

!  MeshAssociativity_f.f90 
!
!
    
#include "Types_f.f90"

!****************************************************************************
!
!  MODULE: mesh_associativity
!
!  PURPOSE:  Interface to MeshAssociativity C functions.
!
!****************************************************************************


    module mesh_associativity
      use iso_c_binding
      interface

        ! Note FORTRAN - C conversion:
        !   TYPE(C_PTR), VALUE       == void *  (pass by value)
        !   TYPE(C_PTR), INTENT(OUT) == void ** (pass by reference)
      
        ! int ML_createMeshAssociativityObj(MeshAssociativityObj *meshAssocObj));
        ML_STATUS FUNCTION ML_createMeshAssociativityObj(meshAssocObj) bind(C, name="ML_createMeshAssociativityObj")
          use, intrinsic :: iso_c_binding
          implicit none
          ML_OBJECT, INTENT(OUT) :: meshAssocObj
        ENDFUNCTION ML_createMeshAssociativityObj
        

        ! void   ML_freeMeshAssociativityObj(MeshAssociativityObj *meshAssocObj) );
        subroutine ML_freeMeshAssociativityObj(meshAssocObj) bind(C, name="ML_freeMeshAssociativityObj")
          use, intrinsic :: iso_c_binding
          implicit none
          ML_OBJECT, INTENT(OUT) :: meshAssocObj
        END subroutine ML_freeMeshAssociativityObj
    

        ! int    ML_parserValidateFile(
        !    MeshLinkParserObj parseObj,
        !    const char *meshlinkFilename,
        !    const char *schemaFilename));
        FUNCTION ML_parserValidateFile(parseObj, meshlinkFilename, schemaFilename) bind(C, name="ML_parserValidateFile")
          use, intrinsic :: iso_c_binding
          implicit none
          ML_STATUS :: ML_parserValidateFile
          ML_OBJECT, VALUE :: parseObj
          ML_STRING, INTENT(IN) :: meshlinkFilename(*)
          ML_STRING, INTENT(IN) :: schemaFilename(*)
        ENDFUNCTION ML_parserValidateFile

        ! int    ML_parserReadMeshLinkFile(
        !    MeshLinkParserObj parseObj,
        !    const char *meshlinkFilename,
        !    MeshAssociativityObj meshAssocObj));
        FUNCTION ML_parserReadMeshLinkFile(parseObj, meshlinkFilename, meshAssocObj) bind(C, name="ML_parserReadMeshLinkFile")
          use, intrinsic :: iso_c_binding
          implicit none
          ML_STATUS :: ML_parserReadMeshLinkFile
          ML_OBJECT, VALUE :: parseObj
          ML_STRING, INTENT(IN) :: meshlinkFilename(*)
          ML_OBJECT, VALUE :: meshAssocObj
        ENDFUNCTION ML_parserReadMeshLinkFile
        
        
        ! int ML_getMeshModelByName(MeshAssociativityObj meshAssocObj, const char *name, MeshModelObj *meshModel));
        FUNCTION ML_getMeshModelByName(meshAssocObj, name, meshModelObj) bind(C, name="ML_getMeshModelByName")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_getMeshModelByName
          ML_OBJECT, VALUE :: meshAssocObj
          ML_STRING, INTENT(IN) :: name(*)
          ML_OBJECT, INTENT(OUT) :: meshModelObj
        ENDFUNCTION ML_getMeshModelByName

        
        !int ML_addGeometryKernel(MeshAssociativityObj meshAssocObj, GeometryKernelObj geomKernelObj));
        FUNCTION ML_addGeometryKernel(meshAssocObj, geomKernelObj) bind(C, name="ML_addGeometryKernel")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_addGeometryKernel
          ML_OBJECT, VALUE :: meshAssocObj
          ML_OBJECT, VALUE :: geomKernelObj
        ENDFUNCTION ML_addGeometryKernel
        
        !int ML_setActiveGeometryKernelByName(MeshAssociativityObj meshAssocObj, const char *name));
        FUNCTION ML_setActiveGeometryKernelByName(meshAssocObj, name) bind(C, name="ML_setActiveGeometryKernelByName")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_setActiveGeometryKernelByName
          ML_OBJECT, VALUE :: meshAssocObj
          ML_STRING, INTENT(IN) :: name(*)
        ENDFUNCTION ML_setActiveGeometryKernelByName
        
        
        !int ML_getActiveGeometryKernel(
        !  MeshAssociativityObj meshAssocObj,   GeometryKernelObj &geomKernelObj));
        FUNCTION ML_getActiveGeometryKernel(meshAssocObj, geomKernelObj) bind(C, name="ML_getActiveGeometryKernel")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_getActiveGeometryKernel
          ML_OBJECT, VALUE :: meshAssocObj
          ML_OBJECT, INTENT(OUT) :: geomKernelObj
        ENDFUNCTION ML_getActiveGeometryKernel
        
        !MLINT ML_getNumGeometryFiles(MeshAssociativityObj meshAssocObj));
        MLINT_F FUNCTION ML_getNumGeometryFiles(meshAssocObj) bind(C, name="ML_getNumGeometryFiles")
          use, intrinsic :: iso_c_binding
          implicit none
          ML_OBJECT, VALUE :: meshAssocObj
        ENDFUNCTION ML_getNumGeometryFiles
        
        !int ML_getGeometryFileObj(MeshAssociativityObj meshAssocObj,
        !MLINT index,
        !MeshLinkFileConstObj *fileObj));
        FUNCTION ML_getGeometryFileObj(meshAssocObj, index, fileObj) bind(C, name="ML_getGeometryFileObj")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_getGeometryFileObj
          ML_OBJECT, VALUE :: meshAssocObj
          MLINT_F, VALUE ::  index
          ML_OBJECT, INTENT(OUT) :: fileObj
        ENDFUNCTION ML_getGeometryFileObj
        
        !int ML_getFilename( MeshLinkFileConstObj fileObj, char *fname, MLINT fnameBufLen ));
        FUNCTION ML_getFilename(fileObj, fname, fnameBufLen) bind(C, name="ML_getFilename")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_getFilename
          ML_OBJECT, VALUE :: fileObj
          ML_STRING, INTENT(OUT) :: fname(*)
          MLINT_F, VALUE ::  fnameBufLen
        ENDFUNCTION ML_getFilename
        
        !
        !/* Read geometry file */
        !int ML_readGeomFile(
        !GeometryKernelObj *geomKernelObj,
        !const char *geomFilename));
        FUNCTION ML_readGeomFile(geomKernelObj, geomFilename) bind(C, name="ML_readGeomFile")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS ML_readGeomFile
          ML_OBJECT, VALUE :: geomKernelObj
          ML_STRING, INTENT(IN) :: geomFilename(*)
    ENDFUNCTION ML_readGeomFile

        
        !
        !/* Set geometry model size */
        !int ML_setGeomModelSize(
        !GeometryKernelObj *geomKernelObj,
        !MLREAL modelSize));
        FUNCTION ML_setGeomModelSize(geomKernelObj, modelSize) bind(C, name="ML_setGeomModelSize")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS ML_setGeomModelSize
          ML_OBJECT, VALUE :: geomKernelObj
          MLREAL_F, VALUE :: modelSize
        ENDFUNCTION ML_setGeomModelSize

        !
        !/* Get geometry model size */
        !int ML_getGeomModelSize(
        !GeometryKernelObj *geomKernelObj,
        !MLREAL *modelSize));
        FUNCTION ML_getGeomModelSize(geomKernelObj, modelSize) bind(C, name="ML_getGeomModelSize")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS ML_getGeomModelSize
          ML_OBJECT, VALUE :: geomKernelObj
          MLREAL_F, INTENT(OUT) :: modelSize
        ENDFUNCTION ML_getGeomModelSize
    
        
        !int ML_getFileAttIDs(MeshAssociativityObj meshAssocObj,
        !MeshLinkFileConstObj fileObj,
        !MLINT attIDs[], MLINT sizeAttIDs, MLINT *numAttIDs  ));
        FUNCTION ML_getFileAttIDs(meshAssocObj, fileObj, attIDs, sizeAttIDs, numAttIDs) bind(C, name="ML_getFileAttIDs")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_getFileAttIDs
          ML_OBJECT, VALUE :: meshAssocObj
          ML_OBJECT, VALUE :: fileObj
          MLINT_F ::  attIDs(*)
          MLINT_F, VALUE ::  sizeAttIDs
          MLINT_F ::  numAttIDs
        ENDFUNCTION ML_getFileAttIDs
        
        
        !
        !int ML_getAttribute(MeshAssociativityObj meshAssocObj,
        !MLINT attID,
        !char *attName, MLINT attNameBufLen,
        !char *attValue, MLINT attValueBufLen
        !));
        FUNCTION ML_getAttribute(meshAssocObj, attID, attName, attNameBufLen, &
		attValue, attValueBufLen) bind(C, name="ML_getAttribute")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_getAttribute
          ML_OBJECT, VALUE :: meshAssocObj
          MLINT_F, VALUE ::  attID
          ML_STRING, INTENT(OUT) :: attName(*)
          MLINT_F, VALUE ::  attNameBufLen
          ML_STRING, INTENT(OUT) :: attValue(*)
          MLINT_F, VALUE ::  attValueBufLen
        ENDFUNCTION ML_getAttribute
        
        
        !
        !int ML_findLowestTopoPointByInd(MeshModelObj meshModelObj,
        !MLINT pointIndex, MeshPointObj* meshPointObj));
        !
        !int ML_getParamVerts(MeshTopoConstObj meshTopoObj,
        !ParamVertexConstObj pvObjsArr[],  /* array of ParamVertexConstObj */
        !MLINT pvObjsArrLen,               /* length of array */
        !MLINT *num_pvObjs ));
        !
        !int ML_getParamVertInfo(ParamVertexConstObj pvObj,
        !char *vref, MLINT vrefBufLen,
        !MLINT *gref,
        !MLINT *mid,
        !MLVector2D UV));
        !
        !int ML_getPointInfo(
        !MeshAssociativityObj meshAssocObj,
        !MeshPointObj meshPointObj,
        !char *ref, MLINT refBufLen,
        !char *name, MLINT nameBufLen,
        !MLINT *gref,
        !MLINT *mid,
        !MLINT attIDs[],
        !MLINT sizeAttIDs,
        !MLINT *numAttIDs,
        !ParamVertexConstObj *pvObj));
        !
        !
        !int ML_getGeometryGroupByID(
        !MeshAssociativityObj meshAssocObj,
        !MLINT gid,
        !GeometryGroupObj *geomGroupObj
        !));
        !
        !int ML_getEntityNames(
        !GeometryGroupObj geomGroupObj,
        !char *entityNamesArr,      /* array of entity name buffers */
        !MLINT entityNamesArrLen,   /* length of names array */
        !MLINT entityNameBufLen,    /* buffer length of a each name */
        !MLINT *num_entityNames
        !));
        !
        !
        
        
        !int ML_evalXYZ(
        !GeometryKernelObj *geomKernelObj,
        !MLVector2D UV,
        !const char *entityName,
        !MLVector3D XYZ));
        FUNCTION ML_evalXYZ(geomKernelObj, UV, entityName, XYZ ) bind(C, name="ML_evalXYZ")
          use, intrinsic :: iso_c_binding
          implicit none
		  ML_STATUS :: ML_evalXYZ
          ML_OBJECT, VALUE :: geomKernelObj
          MLVector2DType(UV, IN)
          ML_STRING :: entityName(*)
          MLVector3DType(XYZ, OUT)
        ENDFUNCTION ML_evalXYZ
        
        
        
        
        
        !
        !int ML_evalRadiusOfCurvature(
        !GeometryKernelObj *geomKernelObj,
        !MLVector2D UV,
        !const char *entityName,
        !MLREAL *radOfCurvature));
        !
        !
        !int ML_createProjectionDataObj(
        !GeometryKernelObj *geomKernelObj,
        !ProjectionDataObj *projectionDataObj
        !));
        !
        !void ML_freeProjectionDataObj(ProjectionDataObj *projectionDataObj));
        !
        !
        !int ML_projectPoint(
        !GeometryKernelObj *geomKernelObj,
        !GeometryGroupObj geomGroupObj,
        !MLVector3D point,
        !ProjectionDataObj projectionDataObj
        !));
        !
        !int ML_getProjectionInfo(
        !GeometryKernelObj *geomKernelObj,
        !ProjectionDataObj projectionDataObj,
        !MLVector3D xyz,
        !MLVector2D UV,
        !char *entityName, MLINT entityNameBufLen
        !));

        
        
        
      end interface
    end module

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
