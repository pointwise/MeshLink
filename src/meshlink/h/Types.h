/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

/****************************************************************************
*
* Variable Type Definitions
*
***************************************************************************/

#ifndef _MESHLINK_PUBLICDEFS_H_
#define _MESHLINK_PUBLICDEFS_H_

#if defined(DEBUG)
    #include <assert.h>
#endif

#undef EXPORT_FUNCTION
#if defined(DLL_LIBRARY_BUILD)
#if defined(WINDOWS)
#  define EXPORT_FUNCTION(F) F
#  if defined(STATIC_LIBRARY)
#    define ML_DLLIMPORT
#    define ML_DLLEXPORT
#  else
#    define ML_DLLIMPORT __declspec(dllimport)
#    define ML_DLLEXPORT __declspec(dllexport)
#  endif
#    define ML_EXTERN extern "C"
#else
#  define EXPORT_FUNCTION(F) F __attribute__ ((visibility ("default") ))
#  define ML_DLLIMPORT
#  define ML_DLLEXPORT
#  define ML_EXTERN extern "C"
#
#endif /* WINDOWS */

#else
   /* static library */
#  define ML_DLLIMPORT
#  define ML_DLLEXPORT

#ifdef __cplusplus
#    define ML_EXTERN extern "C"
#  else
#    define ML_EXTERN
#  endif

#  if defined(WINDOWS)
#    define EXPORT_FUNCTION(F) F
#  else
#    define EXPORT_FUNCTION(F) F __attribute__ ((visibility ("default") ))
#  endif
#endif

#undef ML_STORAGE_CLASS
#ifdef BUILD_MESHLINK
#   define ML_STORAGE_CLASS ML_DLLEXPORT
#else
#   define ML_STORAGE_CLASS ML_DLLIMPORT
#endif

/* Bit length noted ints */
/** \brief 8-bit integer */
typedef signed char         MLINT8;
/** \brief 8-bit unsigned integer */
typedef unsigned char       MLUINT8;

/** \brief 16-bit integer */
typedef short               MLINT16;
/** \brief 16-bit unsigned integer */
typedef unsigned short      MLUINT16;
#ifndef MLINT16_FORMAT
#   define MLINT16_FORMAT      "hd"
#   define MLUINT16_FORMAT     "hu"
#endif /* !MLINT16_FORMAT */

/** \brief 32-bit integer */
typedef int                 MLINT32;
/** \brief 32-bit unsigned integer */
typedef unsigned int        MLUINT32;
#ifndef MLINT32_FORMAT
#   define MLINT32_FORMAT      "d"
#   define MLUINT32_FORMAT     "u"
#endif /* !MLINT32_FORMAT */

#if defined(darwin) && defined(IS64BIT)
    typedef long                MLINT64;
    typedef unsigned long       MLUINT64;
#   ifndef MLINT64_FORMAT
#      define MLINT64_FORMAT "ld"
#      define MLUINT64_FORMAT "lu"
#   endif /* !MLINT64_FORMAT */
#else
    /** \brief 64-bit integer */
    typedef long long           MLINT64;
    /** \brief 64-bit unsigned integer */
    typedef unsigned long long  MLUINT64;
#   ifndef MLINT64_FORMAT
#       define MLINT64_FORMAT      "lld"
#       define MLUINT64_FORMAT     "llu"
#   endif /* !MLINT64_FORMAT */
#endif /* darwin && IS64BIT */

/** \brief 32-bit real */
typedef float               MLFLOAT;
/** \brief 64-bit real */
typedef double              MLREAL; 

/* MLINT & MLUINT are guaranteed to be sizeof(void*) for the target machine */
#ifdef IS64BIT
    /** \brief Standard integer - value depends on IS64BIT macro */
    typedef MLINT64             MLINT;
    /** \brief Standard unsigned integer - value depends on IS64BIT macro */
    typedef MLUINT64            MLUINT;
#   ifndef MLINT_FORMAT
#       define MLINT_FORMAT        MLINT64_FORMAT
#       define MLUINT_FORMAT       MLUINT64_FORMAT
#   endif
#else
    typedef MLINT32             MLINT;
    typedef MLUINT32            MLUINT;
#   ifndef MLINT_FORMAT
#       define MLINT_FORMAT        MLINT32_FORMAT
#       define MLUINT_FORMAT       MLUINT32_FORMAT
#   endif
#endif /* IS64BIT */

#ifndef MLSIZE_T_FORMAT
#   if defined(_MSC_VER)
#       define MLSIZE_T_FORMAT    "Iu"
#       define MLSSIZE_T_FORMAT   "Id"
#       define MLPTRDIFF_T_FORMAT "Id"
#   elif defined(__GNUC__)
#       define MLSIZE_T_FORMAT    "zu"
#       define MLSSIZE_T_FORMAT   "zd"
#       define MLPTRDIFF_T_FORMAT "zd"
#   else
        /* TODO figure out which format use. */
#       define MLSIZE_T_FORMAT    "zu"
#       define MLSSIZE_T_FORMAT   "zd"
#       define MLPTRDIFF_T_FORMAT "zd"
#   endif
#endif

/** \brief 3D vector of MLREAL */
typedef MLREAL MLVector3D[3];
/** \brief 2D vector of MLREAL */
typedef MLREAL MLVector2D[2];

/** @cond */
/** \brief Opaque pointer to a MeshLink object */
typedef void * MeshLinkObject;

/** \brief Opaque pointer to a constant MeshLink object */
typedef void const * MeshLinkConstObject;
/** @endcond */

/** \brief Opaque pointer to a GeometryKernel object */
typedef MeshLinkObject GeometryKernelObj;
/** \brief Opaque pointer to a MeshLinkParser object */
typedef MeshLinkObject MeshLinkParserObj;
/** \brief Opaque pointer to a MeshLinkWriter object */
typedef MeshLinkObject MeshLinkWriterObj;

/** \brief Opaque pointer to a MeshAssociativity object */
typedef MeshLinkObject MeshAssociativityObj;
/** \brief Opaque pointer to a generic MeshLinkFile object */
typedef MeshLinkObject MeshLinkFileObj;
/** \brief Opaque pointer to a constant generic MeshLinkFile object */
typedef MeshLinkConstObject MeshLinkFileConstObj;

/** \brief Opaque pointer to a generic MeshTopo object */
typedef MeshLinkObject MeshTopoObj;
/** \brief Opaque pointer to a constant generic MeshTopo object */
typedef MeshLinkConstObject MeshTopoConstObj;

/** \brief Opaque pointer to a MeshModel object */
typedef MeshLinkObject MeshModelObj;
/** @cond */
typedef MeshModelObj MeshModelObject;
/** @endcond */
/** \brief Opaque pointer to a MeshSheet object */
typedef MeshLinkObject MeshSheetObj;
/** \brief Opaque pointer to a MeshString object */
typedef MeshLinkObject MeshStringObj;
/** \brief Opaque pointer to a MeshPoint object */
typedef MeshLinkObject MeshPointObj;
/** \brief Opaque pointer to a MeshEdge object */
typedef MeshLinkObject MeshEdgeObj;
/** \brief Opaque pointer to a MeshFace object */
typedef MeshLinkObject MeshFaceObj;
/** \brief Opaque pointer to a MeshLinkTransform object */
typedef MeshLinkObject MeshLinkTransformObj;
typedef MeshLinkConstObject MeshLinkTransformConstObj;
/** \brief Opaque pointer to a MeshElementLinkage object */
typedef MeshLinkObject MeshElementLinkageObj;
/** \brief Opaque pointer to a ParamVertex object */
typedef MeshLinkObject ParamVertexObj;
/** \brief Opaque pointer to a constant ParamVertex object */
typedef MeshLinkConstObject ParamVertexConstObj;

/** \brief Opaque pointer to a GeometryGroup object */
typedef MeshLinkObject GeometryGroupObj;
/** \brief Opaque pointer to a constant GeometryGroup object */
typedef MeshLinkConstObject GeometryGroupConstObj;

/** @cond */
typedef MeshLinkObject KernelDataObj;
/** @endcond */
/** \brief Opaque pointer to a ProjectionData object */
typedef MeshLinkObject ProjectionDataObj;

/**
 * \brief Enumeration of values returned for MLORIENT.
 */
enum MeshLinkOrientation {ML_ORIENT_SAME, ML_ORIENT_OPPOSITE};

/**
 * \brief Enumeration of values returned for MLSTATUS.
 */
enum MeshLinkStatus { ML_STATUS_OK, ML_STATUS_ERROR };

/**
 * \brief Enumeration of values returned for MLTYPE.
 */
enum MeshLinkGeomType { ML_TYPE_UNKNOWN /* unknown */, ML_TYPE_CURVE, ML_TYPE_SURFACE };

/**
 * \brief Geometry orientation with respect to container entity.
 * \sa MeshLinkOrientation
 */
typedef int     MLORIENT;

/**
 * \brief Return status of most public functions.
 * \sa MeshLinkStatus
*/
typedef int               MLSTATUS;

/**
 * \brief Basic geometry type.
 * \sa MeshLinkGeomType
 */
typedef int     MLTYPE;

/**
 * \brief Helper function for trapping assertions 
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
void ML_assert(int val);

/* Python wrapper test functions */
/** @cond */
void printVec3(MLVector3D vec);
void printVec2(MLVector2D vec);
void addVec3(MLVector3D vec1, MLVector3D vec2);
/** @endcond */

#endif /* !_MESHLINK_PUBLICDEFS_H_ */

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
