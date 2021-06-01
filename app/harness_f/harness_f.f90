/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

!***********************************************************************
!  MeshLink Library Test Harness (FORTRAN)
!
!  Read MeshLink file.
!     + print diagnostic info to console
!
!  If Project Geode geometry kernel present:
!     + read NMB geometry file
!     + test various computational geometry access functions
!
    

#include "Types_f.f90"

    
!****************************************************************************
!
!  PROGRAM: harness_f
!
!  PURPOSE:  Entry point for the console application.
!
!****************************************************************************

    program harness_f
    USE, INTRINSIC :: ISO_C_BINDING
    use mesh_associativity
#if defined(HAVE_XERCES)
    use mlparser_xerces
#endif
#if defined(HAVE_GEODE)
    use geomkernel_geode
#endif
    implicit none

    ! Variables
    ML_OBJECT meshAssoc
    ML_OBJECT parser
    ML_STATUS status
    INTEGER sphere_test
    ! Name of geometry-mesh associativity file 
    character(len=128) meshlink_fname
    character(len=128) schema_fname

    
    ! Body of harness_f
    
    IF(COMMAND_ARGUMENT_COUNT().NE.1)THEN
      print *, 'usage: <program name> <xml file name>'
      STOP
    ENDIF    
    
    
    ! Name of geometry-mesh associativity file 
    CALL GET_COMMAND_ARGUMENT(1,meshlink_fname)
    meshlink_fname = trim(meshlink_fname)//C_NULL_CHAR
    
    
    status = ML_createMeshAssociativityObj(meshAssoc)
    if (status .eq. 0) then
        print *, 'Success creating Mesh Associativity Object'
    else
        print *, 'FAILED creating Mesh Associativity Object'
        stop
    end if
    
#if defined(HAVE_XERCES)
    status = ML_createMeshLinkParserXercesObj(parser)
    if (status .eq. 0) then
        print *, 'Success loading Xerces parser'
    else
        print *, 'FAILED loading Xerces parser'
        stop
    end if
    
    ! empty schema filename causes schemaLocation in meshlink file to be used */
    schema_fname = trim(schema_fname)//C_NULL_CHAR
    status = ML_parserValidateFile(parser, meshlink_fname, schema_fname)
    if (status .ne. 0) then
        ! validation error 
    end if
    
    status = ML_parserReadMeshLinkFile(parser, meshlink_fname, meshAssoc)
    if (status .ne. 0) then
        print *, 'FAILED reading MeshLink file'
        stop
    end if
    
    call ML_freeMeshLinkParserXercesObj(parser)
#else
    !print *, 'Error freeing parser object'
    !stop
#endif
    
    
    status = sphere_test(meshAssoc)
    if (status .eq. 0) then
        print *, 'Success sphere_test'
    else
        print *, 'FAILED sphere_test'
        stop
    end if
    
    ! clean up MeshAssociativity data
    call ML_freeMeshAssociativityObj(meshAssoc)

    end program harness_f


!****************************************************************************
! STRING_F_TO_C
! Replace space padding with C null characters
!****************************************************************************
    SUBROUTINE STRING_F_TO_C(STRING) 
    USE, INTRINSIC :: ISO_C_BINDING
    CHARACTER*(*) STRING 
    DO 15, I = LEN(STRING), 1, -1 
      IF(STRING(I:I) .EQ. ' ') THEN
          STRING(I:I) = C_NULL_CHAR
      END IF
15  CONTINUE 
    END SUBROUTINE    
    
!****************************************************************************
! STRING_C_TO_F
! Replace C null character padding with spaces
!****************************************************************************
    SUBROUTINE STRING_C_TO_F(STRING) 
    USE, INTRINSIC :: ISO_C_BINDING
    CHARACTER*(*) STRING 
    DO 15, I = LEN(STRING), 1, -1 
      IF(STRING(I:I) .EQ. C_NULL_CHAR) THEN
          STRING(I:I) = ' '
      END IF
15  CONTINUE 
    END SUBROUTINE    
    
    
!****************************************************************************
! Function Vec3Distance - distance between two points
!****************************************************************************
    REAL function Vec3Distance(pt1,pt2)
    USE, INTRINSIC :: ISO_C_BINDING
        MLVector3D(pt1)
        MLVector3D(pt2)
        integer n
        Vec3Distance = 0.0
        do n = 1, 3, 1
            Vec3Distance = Vec3Distance + (pt1(n)-pt2(n))**2.0
        end do
        Vec3Distance = sqrt(Vec3Distance)
        return
    endfunction
    
    
!****************************************************************************
! Function SPHERE_TEST
!****************************************************************************
    INTEGER function sphere_test(meshAssoc)
    USE, INTRINSIC :: ISO_C_BINDING
    use mesh_associativity
#if defined(HAVE_GEODE)
    use geomkernel_geode
#endif

    ML_STATUS status
    MLINT_F numGeomFiles
    MLINT_F iFile
    ML_OBJECT meshAssoc
    ML_OBJECT meshModel
    character(len=64) target_block_name
    ML_OBJECT geomKernel
    character(len=64) geom_fname
    character(len=64) attName
    character(len=64) attValue
    MLINT_F MAX_STRING_SIZE 
    ML_OBJECT geomFile
    
    MLINT_F iAtt
    MLINT_F sizeAttIDs
    MLINT_F numAttIDs
    MLINT_F attIDs(24)

    character(len=64) entityNames(10)
    MLVector2D(UV)
    MLVector3D(evaluationPoint)
    MLVector3D(bottom_con_pt)
    MLREAL_F dist
	MLREAL_F modelSize
   
    modelSize = 1000.0
    sizeAttIDs = 24
    MAX_STRING_SIZE = 64 
    
    target_block_name = "/Base/sphere"
    call STRING_F_TO_C(target_block_name)

    status = ML_getMeshModelByName(meshAssoc, target_block_name, meshModel)
    if (status .ne. 0) then
        print *, 'FAILED loading mesh model'
        sphere_test = 1
        return
    end if
    
#if defined(HAVE_GEODE)
    ! Load Project Geode Kernel and set as active kernel
    status = ML_createGeometryKernelGeodeObj(geomKernel)
    if (status .eq. 0) then
        print *, 'Success loading Geode kernel'
    else
        print *, 'FAILED loading Geode kernel'
        sphere_test = 1
        return
    end if
    
    
#else
    print *, 'No geometry kernel'
    sphere_test = 0
    return
#endif

        status = ML_addGeometryKernel(meshAssoc, geomKernel)
        status = ML_setActiveGeometryKernelByName(meshAssoc, "Geode")   
        if (status .ne. 0) then
            ! error setting active geom kernel 
			write(*,*) 'Error setting active geometry kernel'
            sphere_test = 1;
            return
        end if
            
        ! Read geometry files
        numGeomFiles = ML_getNumGeometryFiles(meshAssoc);
        
        do iFile = 0, numGeomFiles-1, 1
            !
            status =  ML_getGeometryFileObj(meshAssoc, iFile, geomFile)
            if (status .ne. 0) then
			    write(*,*) 'Error getting geometry file object'
                sphere_test = 1;
                return
            end if
            
            status = ML_getFilename(geomFile, geom_fname, MAX_STRING_SIZE)
            if (status .ne. 0) then
			    write(*,*) 'Error getting geometry file name'
                sphere_test = 1;
                return
            end if
            
            print *, 'GeometryFile Attributes'
            status = ML_getFileAttIDs(meshAssoc, geomFile, attIDs, sizeAttIDs, numAttIDs)
            if (0 .ne. status) then
            !     error 
                continue
            end if
            do iAtt = 1, numAttIDs, 1
                status = ML_getAttribute(meshAssoc, attIDs(iAtt), attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)
                if (status .ne. 0) then
                    ! error
                    continue;
                else 
                    call STRING_C_TO_F(attName)
                    call STRING_C_TO_F(attValue)
                    write(*,'(I2,2X,A,A,A)')  iAtt, trim(attName), ' = ', trim(attValue)

                    ! Get ModelSize attribute
                    if (trim(attName) .eq. 'model size') then
					   read( attValue , * ) modelSize  
					end if

                end if
            end do

			status = ML_setGeomModelSize(geomKernel, modelSize)
            if (status .ne. 0) then
                !     error
                write(*,'(A,F10.2)') 'ML_setGeomModelSize:error ', modelSize
                sphere_test = 1;
                return
            end if
			!
			status = ML_getGeomModelSize(geomKernel, modelSize)
            if (status .ne. 0) then
                !     error
                write(*,'(A)') 'ML_getGeomModelSize:error '
                sphere_test = 1;
                return
            end if
            !
            !
			write(*,*)
            write(*,'(A,F10.2)') 'Geometry Kernel ModelSize = ', modelSize
            status = ML_readGeomFile(geomKernel, geom_fname)
            if (status .ne. 0) then
                !     error
                write(*,'(A,A)') 'ML_readGeomFile:error reading', geom_fname
                sphere_test = 1;
                return
            end if
        end do


        ! input data for geometry eval
        entityNames(1) = 'bottom_con_1'
        call STRING_F_TO_C(entityNames(1))
        UV(1) = 0.1234
        UV(2) = 0.0
        status = ML_evalXYZ( geomKernel, UV, entityNames(1), evaluationPoint)
        if (status .ne. 0) then
            print *, 'ML_evalXYZ:invalid param evaluation'
            sphere_test = 1;
            return
        else
            ! target result for geometry eval
            bottom_con_pt(1) = -0.47677290802217431
            bottom_con_pt(2) =  0.29913675338094192
            bottom_con_pt(3) =  0.39997213024780004
            dist = Vec3Distance(evaluationPoint,bottom_con_pt)
            write(*,'(A,D11.3)') 'ML_evalXYZ: distance =', dist
            if (dist > 1e-6) then
                print *, 'ML_evalXYZ:invalid param evaluation'
                sphere_test = 1;
                return
            end if
        end if

#if defined(HAVE_GEODE)
        ! clean up GeometryKernel data
        call ML_freeGeometryKernelGeodeObj(geomKernel)
#endif


    sphere_test = 0
    return
    endfunction sphere_test

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
