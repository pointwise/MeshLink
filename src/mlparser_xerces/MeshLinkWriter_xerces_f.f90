/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

!  MeshLinkWriter_xerces_f.f90
!
!

!****************************************************************************
!
!  MODULE: mlwriter_xerces
!
!  PURPOSE:  Interface to MeshLinkWriterXerces C functions.
!
!****************************************************************************


    module mlwriter_xerces
      use iso_c_binding
      interface

! Allocate a new MeshLinkWriterXercesObj
        INTEGER(C_INT) FUNCTION ML_createMeshLinkWriterXercesObj(writerObj) bind(C, name="ML_createMeshLinkWriterXercesObj")
          use, intrinsic :: iso_c_binding
          implicit none
          TYPE(C_PTR), INTENT(OUT) :: writerObj
        ENDFUNCTION ML_createMeshLinkWriterXercesObj


! Free MeshLinkWriterXercesObj memory
        subroutine ML_freeMeshLinkWriterXercesObj(writerObj) bind(C, name="ML_freeMeshLinkWriterXercesObj")
          use, intrinsic :: iso_c_binding
          implicit none
          TYPE(C_PTR), INTENT(OUT) :: writerObj
        END subroutine ML_freeMeshLinkWriterXercesObj


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
