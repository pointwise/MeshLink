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
# define ML_STORAGE_CLASS ML_DLLEXPORT
#else
# define ML_STORAGE_CLASS ML_DLLIMPORT
#endif

/* Bit length noted ints */
typedef signed char         MLINT8;
typedef unsigned char       MLUINT8;

typedef short               MLINT16;
typedef unsigned short      MLUINT16;
#ifndef MLINT16_FORMAT
#define MLINT16_FORMAT      "hd"
#define MLUINT16_FORMAT     "hu"
#endif /* !MLINT16_FORMAT */

typedef int                 MLINT32;
typedef unsigned int        MLUINT32;
#ifndef MLINT32_FORMAT
#define MLINT32_FORMAT      "d"
#define MLUINT32_FORMAT     "u"
#endif /* !MLINT32_FORMAT */

#if defined(darwin) && defined(IS64BIT)
typedef long                MLINT64;
typedef unsigned long       MLUINT64;
#ifndef MLINT64_FORMAT
#define MLINT64_FORMAT "ld"
#define MLUINT64_FORMAT "lu"
#endif /* !MLINT64_FORMAT */
#else
typedef long long           MLINT64;
typedef unsigned long long  MLUINT64;
#ifndef MLINT64_FORMAT
#define MLINT64_FORMAT      "lld"
#define MLUINT64_FORMAT     "llu"
#endif /* !MLINT64_FORMAT */
#endif /* darwin && IS64BIT */

typedef float               MLFLOAT;    /* 32-bit real */
typedef double              MLREAL;     /* 64-bit real */

/* MLINT & MLUINT are guaranteed to be sizeof(void*) for the target machine */
#ifdef IS64BIT
typedef MLINT64             MLINT;
typedef MLUINT64            MLUINT;
#ifndef MLINT_FORMAT
#define MLINT_FORMAT        MLINT64_FORMAT
#define MLUINT_FORMAT       MLUINT64_FORMAT
#endif
#else
typedef MLINT32             MLINT;
typedef MLUINT32            MLUINT;
#ifndef MLINT_FORMAT
#define MLINT_FORMAT        MLINT32_FORMAT
#define MLUINT_FORMAT       MLUINT32_FORMAT
#endif
#endif /* IS64BIT */

#ifndef MLSIZE_T_FORMAT
#if defined(_MSC_VER)
#define MLSIZE_T_FORMAT    "Iu"
#define MLSSIZE_T_FORMAT   "Id"
#define MLPTRDIFF_T_FORMAT "Id"
#elif defined(__GNUC__)
#define MLSIZE_T_FORMAT    "zu"
#define MLSSIZE_T_FORMAT   "zd"
#define MLPTRDIFF_T_FORMAT "zd"
#else
/* TODO figure out which to use. */
#define MLSIZE_T_FORMAT    "zu"
#define MLSSIZE_T_FORMAT   "zd"
#define MLPTRDIFF_T_FORMAT "zd"
#endif
#endif


typedef MLREAL MLVector3D[3];
typedef MLREAL MLVector2D[2];

typedef void * MeshLinkObject;
typedef void const * MeshLinkConstObject;

typedef MeshLinkObject GeometryKernelObj;
typedef MeshLinkObject MeshLinkParserObj;

typedef MeshLinkObject MeshAssociativityObj;
typedef MeshLinkObject MeshLinkFileObj;
typedef MeshLinkConstObject MeshLinkFileConstObj;

typedef MeshLinkObject MeshTopoObj;
typedef MeshLinkConstObject MeshTopoConstObj;

typedef MeshLinkObject MeshModelObj;
typedef MeshModelObj MeshModelObject;
typedef MeshLinkObject MeshSheetObj;
typedef MeshLinkObject MeshStringObj;
typedef MeshLinkObject MeshPointObj;
typedef MeshLinkObject MeshEdgeObj;
typedef MeshLinkObject MeshFaceObj;
typedef MeshLinkObject ParamVertexObj;
typedef MeshLinkConstObject ParamVertexConstObj;

typedef MeshLinkConstObject GeometryGroupConstObj;
typedef MeshLinkObject GeometryGroupObj;

typedef MeshLinkObject KernelDataObj;
typedef MeshLinkObject ProjectionDataObj;


/* Helper function for trapping assertions */
ML_EXTERN ML_STORAGE_CLASS
void ml_assert(int val);

/* Python wrapper test functions */
void printVec3(MLVector3D vec);
void printVec2(MLVector2D vec);

void addVec3(MLVector3D vec1, MLVector3D vec2);

#endif /* !_MESHLINK_PUBLICDEFS_H_ */
