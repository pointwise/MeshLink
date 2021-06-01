/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
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
 * This file is licensed under the Cadence Public License Version 1.0 (the
 * "License"), a copy of which is found in the included file named "LICENSE",
 * and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
 * LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
 * ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
 * Please see the License for the full text of applicable terms.
 *
 ****************************************************************************/
