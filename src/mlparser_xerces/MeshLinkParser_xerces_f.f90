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
