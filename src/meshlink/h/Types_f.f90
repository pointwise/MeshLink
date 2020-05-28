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
