/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

// MLINT & MLUINT are guaranteed to be sizeof(void*) for the target machine
#if defined(WIN64)
#define MLINT_POD_TYPE long long
typedef long long           MLINT;
typedef unsigned long long  MLUINT;
#elif defined(IS64BIT)
#if defined(darwin)
#define MLINT_POD_TYPE long
typedef long                MLINT;
typedef unsigned long       MLUINT;
#else
#define MLINT_POD_TYPE long long
typedef long long           MLINT;
typedef unsigned long long  MLUINT;
#endif
#else
#define MLINT_POD_TYPE long
typedef int                 MLINT;      
typedef unsigned int        MLUINT;
#endif

#define MLINT_TCL_TYPE Int

/* Bit length noted ints */
typedef signed char         MLINT8;
typedef unsigned char       MLUINT8;

typedef short               MLINT16;
typedef unsigned short      MLUINT16;

typedef int                 MLINT32;
typedef unsigned int        MLUINT32;

#if defined(darwin) && defined(IS64BIT)
typedef long                MLINT64;
typedef unsigned long       MLUINT64;
#else
typedef long long           MLINT64;
typedef unsigned long long  MLUINT64;
#endif

typedef float               MLFLOAT;    // 32-bit real
typedef double              MLREAL;     // 64-bit real

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
