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
