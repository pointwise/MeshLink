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

!  GeomKernel_Geode_f.f90 
!
!

!****************************************************************************
!
!  MODULE: geomkernel_geode
!
!  PURPOSE:  Interface to GeometryKernelGeode C functions.
!
!****************************************************************************


    module geomkernel_geode
      use iso_c_binding
      interface
      
      
! Allocate a new GeometryKernelGeodeObj 
        INTEGER(C_INT) FUNCTION ML_createGeometryKernelGeodeObj(geomKernelObj) bind(C, name="ML_createGeometryKernelGeodeObj")
          use, intrinsic :: iso_c_binding
          implicit none
          TYPE(C_PTR), INTENT(OUT) :: geomKernelObj
        ENDFUNCTION ML_createGeometryKernelGeodeObj
        

! Free GeometryKernelGeodeObj memory 
        subroutine ML_freeGeometryKernelGeodeObj(geomKernelObj) bind(C, name="ML_freeGeometryKernelGeodeObj")
          use, intrinsic :: iso_c_binding
          implicit none
          TYPE(C_PTR), INTENT(OUT) :: geomKernelObj
        END subroutine ML_freeGeometryKernelGeodeObj
    
        
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
