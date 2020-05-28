/****************************************************************************
 *
 * Copyright (c) 2019-2020 Pointwise, Inc.
 * All rights reserved.
 *
 * This sample Pointwise source code is not supported by Pointwise, Inc.
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
 * DISCLAIMER:
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, POINTWISE DISCLAIMS
 * ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, WITH REGARD TO THIS SCRIPT. TO THE MAXIMUM EXTENT PERMITTED
 * BY APPLICABLE LAW, IN NO EVENT SHALL POINTWISE BE LIABLE TO ANY PARTY
 * FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE
 * USE OF OR INABILITY TO USE THIS SCRIPT EVEN IF POINTWISE HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES AND REGARDLESS OF THE
 * FAULT OR NEGLIGENCE OF POINTWISE.
 *
 ***************************************************************************/
