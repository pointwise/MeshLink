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
