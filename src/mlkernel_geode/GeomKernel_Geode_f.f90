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
