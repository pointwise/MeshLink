/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

!  MeshLinkParser_xerces_f.f90 
!
!

!****************************************************************************
!
!  MODULE: mlparser_xerces
!
!  PURPOSE:  Interface to MeshLinkParserXerces C functions.
!
!****************************************************************************


    module mlparser_xerces
      use iso_c_binding
      interface
      
! Allocate a new MeshLinkParserXercesObj 
        INTEGER(C_INT) FUNCTION ML_createMeshLinkParserXercesObj(parseObj) bind(C, name="ML_createMeshLinkParserXercesObj")
          use, intrinsic :: iso_c_binding
          implicit none
          TYPE(C_PTR), INTENT(OUT) :: parseObj
        ENDFUNCTION ML_createMeshLinkParserXercesObj
        

! Free MeshLinkParserXercesObj memory 
        subroutine ML_freeMeshLinkParserXercesObj(parseObj) bind(C, name="ML_freeMeshLinkParserXercesObj")
          use, intrinsic :: iso_c_binding
          implicit none
          TYPE(C_PTR), INTENT(OUT) :: parseObj
        END subroutine ML_freeMeshLinkParserXercesObj
    
        
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
