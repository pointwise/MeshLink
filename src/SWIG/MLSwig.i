/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

// Map MeshLink Library Types and Functions to Python
//
// Character strings are passed back and forth from Python to C 
// by utilizing a temporary buffer with a fixed maximum size defined 
// by ML_BUFFER_SIZE.
#define ML_BUFFER_SIZE 512            // max size of string buffer

// We will handle arrays of data using temporary fixed-size C arrays
// with the following sizes.
#define ML_BUFFER_ARRAY_SIZE 96       // max size of string buffer array
#define MLINT_ARRAY_SIZE 96           // max size of MLINT array
#define ML_PARAMVERTEX_ARRAY_SIZE 12  // max size of ParamVertexConstObj array
	

%begin %{ 
   #ifdef _MSC_VER 
   #define SWIG_PYTHON_INTERPRETER_NO_DEBUG 
   #endif 
%} 
   

%{
// Force the preprocessor to coallesce strtoull to _strtoui64 for MS
#if defined(WIN32) || defined(WIN64)
#define strtoull _strtoui64
#define strtoll _strtoi64
#elif defined(sgi65)
#define strtoull std::strtoull
#define strtoll std::strtoll
#endif
%}


//%include MLTypedefsSwig.i

%{
#include "Types.h"
#include "MeshAssociativity_c.h"

#if defined(HAVE_GEODE)
#include "GeomKernel_Geode_c.h"
#endif

#if defined(HAVE_XERCES)
#include "MeshLinkParser_xerces_c.h"
#endif
%}



%types(unsigned char *, MLUINT *, MLINT, MLINT *, MLREAL *, bool *, MeshAssociativityObj *);



////////////////////////////////////////////////////////////////////////////////////////////////
// Typemaps to handle Point/Edge/Face indices input as a PyList
// to the ML_find<Point/Edge/Face>ByInds function.
//
// Requires match of arg type and arg name
//    Specifically: MLINT *indices, MLINT numIndices
//
%typemap(in) (MLINT *indices) (MLINT indices[MLINT_ARRAY_SIZE], MLINT maxIndices=MLINT_ARRAY_SIZE) {
   // Handle in MLINT *indices
	Py_ssize_t  l = PyList_Size( $input );
	if (maxIndices < l) {
	    // not enough room in static
		SWIG_fail;
	}
	$1 = indices;
	for (int i = 0; i < l; i++) {
		PyObject *oo = PyList_GetItem( $input, i );
		$1[i] = (MLINT) PyLong_AsLongLong( oo );
	}
}

%typemap(in) (MLINT numIndices) ( MLINT numIndices ) {
   // Handle in MLINT numIndices in from Python 
   $1 = numIndices = (MLINT) PyLong_AsLongLong( $input );
}

%typemap(freearg)  (MLINT *indices)  {
   // free MLINT *indices
}

%typemap(argout) (MLINT *indices) {
    // (MLINT *indices) out
	// no-op
}


////////////////////////////////////////////////////////////////////////////////////////////////
// Typemaps to handle MLVector3D, MLVector2D as a PytList
%define HANDLE_MLVECTYPE_AS_LIST(MLType, len, mlItemType, pythonType, pycType, tmptype)

%typemap(in)  MLType  (bool del = false) {
    // MLType in
    if (!SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, SWIGTYPE_p_a_##len##__##tmptype##, 0))) {
		Py_ssize_t  l = PyList_Size( $input );
        if (len != l) {
            SWIG_fail;
        }

        del = true;
        $1 = new mlItemType[len];

        int i;
        for (i = 0; i < len; i++) {
		    PyObject *oo = PyList_GetItem( $input, i );
            $1[i] = (##mlItemType##) ##pythonType##_As##pycType##( oo );
        }
    }
}
%typemap(freearg)  MLType  {
    // MLType free
    if (del$argnum) {
        delete [] $1;
        $1 = 0;
    }
}
%typemap(freearg) (MLType OutValue)  {
    // MLType OutValue free
}
%typemap(out) MLType {
    // MLType out
    int i;
	$result = PyList_New(len);
    for (i = 0; i < len; i++) {
		PyList_SetItem($result,i,##pythonType##_From##pycType##($1[i]));
    }
}

%typemap(typecheck) MLType {
	int l = PyList_Size( $input );
    MLType *ptr = (MLType *)0;
	if (len != l && !SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &ptr, SWIGTYPE_p_a_##len##__##tmptype##, 0))) {
        $1 = 0;
    } else {
        $1 = 1;
    }
}

// This tells SWIG to treat an MLVector3D argument with name 'OutValue' as
// an output value.  We'll append the value to the current result which 
// is guaranteed to be a List object by SWIG.

%typemap(in,numinputs=0) (MLType OutValue) (MLType outVar) {
    // Handle in MLType OutValue
	$1 = outVar;
}

%typemap(argout) (MLType OutValue)  {
    // MLType argout
    PyObject *o;
    int i;
 	o = PyList_New(len);
    for (i = 0; i < len; i++) {
		PyList_SetItem(o,i,##pythonType##_From##pycType##($1[i]));
    }
    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {	
		// return object might not be a PyList
		if (!PyList_Check($result)) {
		   // remake result as a PyList
		   PyObject *newResult = PyList_New( 1 );
		   PyList_SetItem(newResult, 0, $result);
		   $result = newResult;
		}

        // we want to append the MLType as a nested list, so a tempList is required
		PyObject *tempList = PyList_New(1);
		PyList_SetItem(tempList, 0, PyInt_FromLong(0));      // have to set a dummy object
		resultobj = PySequence_Concat(resultobj, tempList);  // now we have a spot for our real return
		
		// now set the last item in the result list to our list 
		PyList_SetItem($result, PySequence_Size($result)-1, o);
	}
}
%enddef



///////////////////////////////////////////////////////////////////////////////////////////////////
%define HANDLE_MLTYPE(MLType, pythonType, pycType, tmptype)


%typemap(in)  MLType  (bool del = false) {
    // Handle in MLType
    if (!SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &$1, SWIGTYPE_p_##tmptype##, 0))) {
	
        $1 = (##MLType##) ##pythonType##_As##pycType##( $input );
    }
}



%typemap(in,numinputs=0)  (MLType *)  (MLType outVar) {
    // Handle in MLType *
	$1 = &outVar;
}

%typemap(freearg)  MLType  {
}

%typemap(out) MLType {
    $result = ##pythonType##_From##pycType##($1);
}

%typemap(argout) (MLType *) {
    // return MLType * value as pythonType appended to PyList if nec'y
	PyObject *argObj = pythonType##_From##pycType##(*$1);
	
    if ((!$result) || ($result == Py_None)) {
	    // result empty, return just this object
        $result = argObj;
	} else {
	   // result exists, have to add argObj to new PyList and
	   // append new PyList to return PyList 
       PyObject *argList = PyList_New(1);
       PyList_SetItem(argList,0,argObj);
	   
	   // return object might not be a PyList
	   if (!PyList_Check($result)) {
	       // remake result as a PyList
		   PyObject *newResult = PyList_New( 1 );
		   PyList_SetItem(newResult, 0, $result);
		   $result = newResult;
	   }
       // add this argList to the result PyList   
	   $result = PySequence_Concat($result,argList);
	}
}

%typemap(typecheck) MLType {
    MLType *ptr = (MLType *)0;
	if (!SWIG_IsOK(SWIG_ConvertPtr($input, (void **) &ptr, SWIGTYPE_p_##tmptype##, 0))) {
        $1 = 0;
    } else {
        $1 = 1;
    }
}

// This tells SWIG to treat an MLType argument with name 'OutValue' as
// an output value.  We'll append the value to the current result which 
// is guaranteed to be a List object by SWIG.

%typemap(argout) xxxMLType OutValue {
    PyObject *o;
 	o = PyList_New(1);
	PyList_SetItem(o,0,##pythonType##_From##pycType##($1));
    if ((!$result) || ($result == Py_None)) {
        $result = o;
    } else {	
	    $result = PySequence_Concat($result,o);
	}
}
%enddef

///////////////////////////////////////////////////////////////////////////////////////////////////
%define HANDLE_MLTYPE_ARRAY(MLType, array_size, pythonType, pycType, tmptype)

// This tells SWIG to treat an MLType [] argument with name 'OutArray' as
// an output value.  We'll append the value to the current result which 
// is guaranteed to be a List object by SWIG.


// For this to work, the C arg pattern is MLType arr[], MLINT arrSize, MLINT *numItems
%typemap(arginit) (MLType OutArray[], MLINT OutArraySize, MLINT *OutArrayCount) (MLType OutArray[array_size], MLINT OutArrayCount) {
    // Handle MLType array in from Python (arginit)
	// Arg pattern is MLType C_array, MLINT arraySize, MLINT *arrayItemCount
	// MLSwig.i declares the fixed C_array size (currently array_size)
	$1 = &(OutArray[0]);    // hard-coded array
	$2 = array_size;        // hard-coded array size arg
	$3 = &OutArrayCount;    // item count
}

%typemap(in,numinputs=0) (MLType OutArray[], MLINT OutArraySize, MLINT *OutArrayCount) {
   // Handle MLType array in from Python 
   //   a no-op since we're using fixed-size array
}

%typemap(argout) (MLType OutArray[], MLINT OutArraySize, MLINT *OutArrayCount) {
    // Handle MLType array out to Python List
    PyObject *argObj;
    int i;
	MLINT size = array_size;
	MLINT len = OutArrayCount$argnum;  // return count var
	
	if (len > size) len = size;
 	argObj = PyList_New(len);
    for (i = 0; i < len; i++) {
		PyList_SetItem(argObj,i,##pythonType##_From##pycType##($1[i]));
    }
	
    if ((!$result) || ($result == Py_None)) {
	    // result empty, return just this object
        $result = argObj;
	} else {
	   // result exists, append argObj to return PyList
	   
	   // return object might not be a PyList
	   if (!PyList_Check($result)) {
	       // remake result as a PyList
		   PyObject *newResult = PyList_New( 1 );
		   PyList_SetItem(newResult, 0, $result);
		   $result = newResult;
	   }

		// we want to append the MLType array as a nested list, so a tempList is required
		PyObject *tempList = PyList_New(1);
		PyList_SetItem(tempList, 0, PyInt_FromLong(0));      // have to set a dummy object
		resultobj = PySequence_Concat(resultobj, tempList);  // now we have a spot for our real return

		// now set the last item in the result list to our list 
		PyList_SetItem($result, PySequence_Size($result)-1, argObj);
	}
}


%enddef

%typemap(out) xint {
	$result = PyLong_FromLong((long)$1);
}



///////////////////////////////////////////////////////////////////////////////////////////////////
%define HANDLE_MLPARAMVERTEX_ARRAY_OUTPUT(MLType, array_size)

// This tells SWIG to treat an ParamVertexObj [] argument with name 'OutArray' as
// an output value.  We'll create a list of the return objects and append to the 
// result PyList.

// For this to work, the C arg pattern is MLType arr[], MLINT arrSize, MLINT *numItems
%typemap(arginit) (MLType OutArray[], MLINT OutArraySize, MLINT *OutArrayCount) 
   (MLType OutArray[array_size], MLINT OutArrayCount) 
{
    // Handle MLType array in from Python (arginit)
	// Arg pattern is MLType C_array, MLINT arraySize, MLINT *arrayItemCount
	// MLSwig.i declares the fixed C_array size (currently array_size)
	$1 = OutArray;          // hard-coded array
	$2 = array_size;        // hard-coded array size arg
	$3 = &OutArrayCount;    // item count
}

%typemap(in,numinputs=0) (MLType OutArray[], MLINT OutArraySize, MLINT *OutArrayCount) {
   // Handle MLType array in from Python 
   //   a no-op since we're using fixed-size array
}

%typemap(argout) (MLType OutArray[], MLINT OutArraySize, MLINT *OutArrayCount) {
    // Handle MLType array out to Python
    PyObject *argObj;
    int i;
	MLINT size = array_size;
	MLINT len = OutArrayCount$argnum;  // return count var
	
	if (len > size) len = size;
	// add ParamVertex object to list
	argObj = PyList_New(len);
	for (i = 0; i < len; i++) {
        // Create a Python Object for MLType
        // The creation of a suitable PyObject is lifted verbatim 
        // from the SWIG wrapper code for the MeshLinkConstObject new operator	
        PyObject *obj = 0;
        MeshLinkConstObject *result = 0;

        // create new MeshLinkConstObject
        result = (MeshLinkConstObject *)new_MeshLinkConstObject();
        // wrap it in python object
        obj = SWIG_NewPointerObj(SWIG_as_voidptr(result), SWIGTYPE_p_MeshLinkConstObject, SWIG_POINTER_NEW | 0);

        // get pointer to python object's payload pointer
        void **pvPtr = 0;
        int res = SWIG_ConvertPtr(obj, (void**)&(pvPtr), SWIGTYPE_p_MeshLinkConstObject, 0 | 0);
        if (!SWIG_IsOK(res)) {
            SWIG_fail;
        }
        // set the payload pointer value to MLType item (loses qualifiers)
        *pvPtr = (void *)OutArray$argnum[i];
	
		PyList_SetItem(argObj,i,obj);  // set the object in the list
	}
	
    if ((!$result) || ($result == Py_None)) {
        $result = argObj;
    } else {	
	   // return object might not be a PyList
	   if (!PyList_Check($result)) {
	       // remake result as a PyList
		   PyObject *newResult = PyList_New( 1 );
		   PyList_SetItem(newResult, 0, $result);
		   $result = newResult;
	   }
		// we want to append the MLType array as a nested list, so a tempList is required
		PyObject *tempList = PyList_New(1);
		PyList_SetItem(tempList, 0, PyInt_FromLong(0));      // have to set a dummy object
		resultobj = PySequence_Concat(resultobj, tempList);  // now we have a spot for our real return

		// now set the last item in the result list to our list 
		PyList_SetItem($result, PySequence_Size($result)-1, argObj);
	}
}
%enddef


%define HANDLE_MLSTRING_BUFFER(bufName, buffer_size, bufLenName)
////////////////////////////////////////////////////////////////////////////////////
// This tells SWIG to treat an char*,MLINT argument pattern as
// an output value.  We'll append the value to the current result which 
// is guaranteed to be a List object by SWIG.


// For this to work, the C arg pattern is MLType arr[], MLINT arrSize, MLINT *numItems
%typemap(arginit) (char *bufName, MLINT bufLenName) (char OutArray[buffer_size], MLINT OutArraySize=buffer_size) {
    // Handle string buffer in from Python (arginit)
	// Arg pattern is char *bufName, MLINT bufLenName
	// MLSwig.i declares the fixed C_array size (currently array_size)
	$1 = &(OutArray[0]);    // hard-coded array
	$2 = OutArraySize;      // hard-coded array size arg
}

%typemap(in,numinputs=0) (char *bufName, MLINT bufLenName) {
    // Handle string buffer in from Python 
    //   a no-op since we're using fixed-size array
}

%typemap(argout) (char *bufName, MLINT bufLenName) {
    // Handle string buffer bufName out to Python
	PyObject *argObj = PyString_FromString($1);
	
    if ((!$result) || ($result == Py_None)) {
	    // result empty, return just this object
        $result = argObj;
	} else {
	   // result exists, have to add argObj to new PyList and
	   // append new PyList to return PyList 
       PyObject *argList = PyList_New(1);
       PyList_SetItem(argList,0,argObj);
	   
	   // return object might not be a PyList
	   if (!PyList_Check($result)) {
	       // remake result as a PyList
		   PyObject *newResult = PyList_New( 1 );
		   PyList_SetItem(newResult, 0, $result);
		   $result = newResult;
	   }
       // add this argList to the result PyList   
	   $result = PySequence_Concat($result,argList);
	}
}
%enddef



	
	
%define HANDLE_MLSTRING_BUFFER_ARRAY(bufArrayName, array_size, buffer_size, arrayLenName, bufLenName, arrayCountName)
////////////////////////////////////////////////////////////////////////////////////
// This tells SWIG to treat an char*,MLINT, MLINT, MLINT* argument pattern as
// an output string buffer array.  We'll append the array values to the current result which 
// is guaranteed to be a List object by SWIG.

// For this to work, the C arg pattern is char *bufArrayName, MLINT arrayLenName, MLINT bufLenName, MLINT *arrayCountName

%typemap(arginit) (char *bufArrayName, MLINT arrayLenName, MLINT bufLenName, MLINT *arrayCountName) (char OutArray[array_size][buffer_size], MLINT OutArraySize = array_size, MLINT OutArrayBufSize=buffer_size, MLINT OutArrayCount) {
    // Handle string buffer array in from Python (arginit)
	// Arg pattern is char *bufArrayName, MLINT arrayLenName, MLINT bufLenName, MLINT *arrayCountName
	// MLSwig.i declares the fixed C_array size (currently array_size)
	$1 = &(OutArray[0][0]);    // hard-coded array of buffers
	$2 = OutArraySize;      // hard-coded array size arg
	$3 = OutArrayBufSize;   // hard-coded buffer size arg
	$4 = &OutArrayCount;    // item count
}

%typemap(in,numinputs=0) (char *bufArrayName, MLINT arrayLenName, MLINT bufLenName, MLINT *arrayCountName) {
    // Handle string buffer array in from Python 
    //   a no-op since we're using fixed-size array of fixed-size buffers
}

%typemap(argout) (char *bufArrayName, MLINT arrayLenName, MLINT bufLenName, MLINT *arrayCountName) {
    // Handle string buffer array bufArrayName out to Python
    PyObject *argObj;
    int i;
	MLINT size = array_size;
	MLINT len = OutArrayCount$argnum;  // return count var
	
	if (len > size) len = size;
 	argObj = PyList_New(len);
    for (i = 0; i < len; i++) {
		PyList_SetItem(argObj,i,PyString_FromString( &($1[i*OutArrayBufSize$argnum]) ));
    }
	
    if ((!$result) || ($result == Py_None)) {
	    // result empty, return just this object
        $result = argObj;
	} else {
	   // result exists, append argObj to return PyList
	   
	   // return object might not be a PyList
	   if (!PyList_Check($result)) {
	       // remake result as a PyList
		   PyObject *newResult = PyList_New( 1 );
		   PyList_SetItem(newResult, 0, $result);
		   $result = newResult;
	   }
		// we want to append the buffer array as a nested list, so a tempList is required
		PyObject *tempList = PyList_New(1);
		PyList_SetItem(tempList, 0, PyInt_FromLong(0));      // have to set a dummy object
		resultobj = PySequence_Concat(resultobj, tempList);  // now we have a spot for our real return

		// now set the last item in the result list to our list 
		PyList_SetItem($result, PySequence_Size($result)-1, argObj);
	}
}
%enddef

// Input and output of Vec2 and Vec3 will be as python lists	
%types(MLVector2D *, MLVector3D *);
HANDLE_MLVECTYPE_AS_LIST(MLVector3D, 3, MLREAL, PyFloat, Double, double);
HANDLE_MLVECTYPE_AS_LIST(MLVector2D, 2, MLREAL, PyFloat, Double, double);


HANDLE_MLTYPE( MLINT , PyLong, LongLong, long_long)

HANDLE_MLTYPE( MLREAL , PyFloat, Double, double)

HANDLE_MLTYPE_ARRAY( MLINT , MLINT_ARRAY_SIZE, PyLong, LongLong, long_long)

HANDLE_MLPARAMVERTEX_ARRAY_OUTPUT( ParamVertexConstObj , ML_PARAMVERTEX_ARRAY_SIZE )


HANDLE_MLSTRING_BUFFER( refBuf, ML_BUFFER_SIZE, refBufLen )
HANDLE_MLSTRING_BUFFER( nameBuf, ML_BUFFER_SIZE, nameBufLen )

HANDLE_MLSTRING_BUFFER( entityNameBuf, ML_BUFFER_SIZE, entityNameBufLen )
HANDLE_MLSTRING_BUFFER( filenameBuf, ML_BUFFER_SIZE, filenameBufLen )
HANDLE_MLSTRING_BUFFER( attNameBuf, ML_BUFFER_SIZE, attNameBufLen )
HANDLE_MLSTRING_BUFFER( attValueBuf, ML_BUFFER_SIZE, attValueBufLen )
HANDLE_MLSTRING_BUFFER( vrefBuf, ML_BUFFER_SIZE, vrefBufLen )

HANDLE_MLSTRING_BUFFER_ARRAY(entityNamesBufArr, ML_BUFFER_ARRAY_SIZE, ML_BUFFER_SIZE, entityNamesArrLen, entityNameBufLen, num_entityNames)	


%include "cpointer.i"
%include "cstring.i"
%include "carrays.i"


// Define new pointer types for Python interpreter.
// These allow input/return of MeshLink objects from the Library.
// They are really just void*'s so we can get by with 2 simple types
// for all MeshLink objects.
%pointer_class(MeshLinkObject, MeshLinkObject)
%pointer_class(MeshLinkConstObject, MeshLinkConstObject)

%pointer_cast(MeshLinkObject, MeshLinkConstObject, MeshLinkObject_to_const);


//%typecheck(SWIG_TYPECHECK_VOIDPTR) MeshLinkConstObject * {
//  void *ptr;
//  if (SWIG_ConvertPtr($input, (void **) &ptr, SWIGTYPE_p_MeshLinkObject, 0) == -1) {
//    // NJW MeshLinkConstObject
//    $1 = 0;
//    PyErr_Clear();
//  } elseif (SWIG_ConvertPtr($input, (void **) &ptr, SWIGTYPE_p_MeshLinkConstObject, 0) == -1) {
//    $1 = 0;
//    PyErr_Clear();
//  } else {
//    $1 = 1;
//  }
//}
//

// Variable(s) named OutValue OutArray will be appended to function result
void addVec3(MLVector3D OutValue, MLVector3D OutValue);

int ML_getMeshTopoInfo(
    MeshAssociativityObj meshAssocObj,
    MeshTopoObj meshTopoObj,
    char *refBuf, MLINT refBufLen,
    char *nameBuf, MLINT nameBufLen,
    MLINT *gref,
    MLINT *mid,
    MLINT OutArray[], MLINT OutArraySize, MLINT *OutArrayCount);

int ML_getMeshPointInfo(
    MeshAssociativityObj meshAssocObj,
    MeshPointObj meshPointObj,
    char *refBuf, MLINT refBufLen,
    char *nameBuf, MLINT nameBufLen,
    MLINT *gref,
    MLINT *mid,
    MLINT OutArray[], MLINT OutArraySize, MLINT *OutArrayCount,
    ParamVertexConstObj *pvObj);
	
int ML_getMeshEdgeInfo(
    MeshAssociativityObj meshAssocObj,
    MeshEdgeObj meshEdgeObj,
    char *refBuf, MLINT refBufLen,
    char *nameBuf, MLINT nameBufLen,
    MLINT *gref,
    MLINT *mid,
    MLINT OutArray[], MLINT OutArraySize, MLINT *OutArrayCount,
	ParamVertexConstObj OutArray[], MLINT OutArraySize, MLINT *OutArrayCount);
	
int ML_getEntityNames(
	GeometryGroupObj geomGroupObj,
	char *entityNamesBufArr,   /* array of entity name buffers */
	MLINT entityNamesArrLen,   /* length of names array */
	MLINT entityNameBufLen,    /* buffer length of a each name */
	MLINT *num_entityNames
);

int ML_getParamVertInfo(ParamVertexConstObj pvObj,
	char *vrefBuf, MLINT vrefBufLen,
	MLINT *gref,
	MLINT *mid,
	MLVector2D OutValue);

//int ML_findLowestTopoEdgeByInds(MeshModelObj meshModelObj,
//	MLINT *indices, MLINT numIndices, MeshEdgeObj *meshEdgeObj);

int ML_getParamVerts(MeshTopoConstObj meshTopoObj,
	ParamVertexConstObj OutArray[],  /* array of ParamVertexConstObj */
	MLINT OutArraySize,               /* length of array */
	MLINT *OutArrayCount );
	
int ML_getProjectionInfo(
	GeometryKernelObj geomKernelObj,
	ProjectionDataObj projectionDataObj,
	MLVector3D OutValue,  /* xyz */
	MLVector2D OutValue,  /* uv */
	char *entityNameBuf, MLINT entityNameBufLen
);
	
int ML_evalXYZ(
    GeometryKernelObj geomKernelObj,
    MLVector2D UV,
    const char *entityName,
    MLVector3D OutValue /* XYZ */
);
	
%include ..\meshlink\h\Types.h

%include ..\meshlink\h\MeshAssociativity_c.h
%include ..\mlparser_xerces\MeshLinkParser_xerces_c.h
%include ..\mlkernel_geode\GeomKernel_Geode_c.h

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
