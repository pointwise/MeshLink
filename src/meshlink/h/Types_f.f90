!****************************************************************************
!
! Variable Type Definitions
!
!***************************************************************************/

#ifndef _ML_FORTRAN_TYPES_
#define _ML_FORTRAN_TYPES_

! C function return value
#define ML_STATUS INTEGER(C_INT)    

! C object pointer
#define ML_OBJECT TYPE(C_PTR)

! C integer
#define MLINT_F INTEGER(C_LONG_LONG)

! C real
#define MLREAL_F REAL(C_DOUBLE)

! C character string
#define ML_STRING CHARACTER(KIND=C_CHAR)

! 3D vector of C reals
#define MLVector3D(x)   MLREAL_F x(3)

! 2D vector of C reals
#define MLVector2D(x)   MLREAL_F x(2)

! 3D vector of C reals (function arg type)
#define MLVector3DType(x,io)   MLREAL_F, INTENT(io) ::  x(3)

! 2D vector of C reals (function arg type)
!#define MLVector2DType(x,io)   REAL(MLREAL_F), INTENT(io) ::  x(2)
#define MLVector2DType(x,io)   MLREAL_F, INTENT(io) ::  x(2)

#endif /* !_ML_FORTRAN_TYPES_ */
