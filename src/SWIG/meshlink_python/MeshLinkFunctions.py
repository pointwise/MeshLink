#############################################################################
#
# (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
#
# This sample source code is not supported by Cadence Design Systems, Inc.
# It is provided freely for demonstration purposes only.
# SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
#
#############################################################################

#
# Functions for interacting with the MeshLink Library
#
# Library calls either return a simple result 
#    For example:
#        numGeomFiles = ML.ML_getNumGeometryFiles( meshAssoc )
#
# OR if more than a single item is to be returned, a list is returned
# with the call status as first element.
#    For example, to get the name and value associated with an attID:
#	     ret = ML.ML_getAttribute(meshAssoc, attID)
#	     status = ret[0]
#	     if (0 != status):
#		     die('ML_getAttribute failed')
#	     attName = ret[1]
#	     attValue = ret[2]
#
# There is one exception, a MeshLinkObject argument is set directly by the Library call
#
#   MeshLinkObject - generic MeshLink object pointer
#                  - used to pass MeshLink objects to/from the library as arguments
#                  - must create before use with:  myObject = ML.MeshLinkObject()
#                  - myObject.value() returns the pointed to object
#
#   When querying an object's data, all we need is the value of the MeshLinkObject,
#   not the pointer itself.
#   For example, the command to return the filename string of an existing MeshLinkFileObj:
#		 ret = ML.ML_getFilename(MeshLinkFileObj.value())
#		 if (0 != ret[0]):
#			 die('ML_getFilename failed')
#		 filename = ret[1]
#
import math
import MeshLink as ML

def die(msg1, msg2 = '', msg3 = '', msg4 = '', msg5 = ''):
    error_message = str(msg1) + ' ' + str(msg2) + ' ' + str(msg3) + ' ' + str(msg4) + ' ' + str(msg5)
    raise Exception(error_message)

def ParseMeshLinkFile( meshlink_filename, meshAssoc ):
    # create pointer to generic MeshLinkObject
    parserObj = ML.MeshLinkObject()
    # load pointer with Xerces parser
    status = ML.ML_createMeshLinkParserXercesObj( parserObj )
    if (0 != status):
        die('ML_createMeshLinkParserXercesObj failed')

    parser = parserObj.value()

    # validate the meshlink file against the schema
    status = ML.ML_parserValidateFile(parser, meshlink_filename, '')
    if (0 != status):
        die(status)
        
    # parse meshlink file into meshAssoc object
    status = ML.ML_parserReadMeshLinkFile(parser, meshlink_filename, meshAssoc)
    if (0 != status):
        die('ML_parserReadMeshLinkFile failed')
        
    ML.ML_freeMeshLinkParserXercesObj(parserObj);    
    
def CreateGeode( meshAssoc ):
    # create pointer to generic MeshLinkObject
    geomKernelObj = ML.MeshLinkObject()
    
    # load pointer with Geode kernel object
    status = ML.ML_createGeometryKernelGeodeObj( geomKernelObj )
    if (0 != status):
        die('ML_createGeometryKernelGeodeObj failed')
        
    geomKernel = geomKernelObj.value()
    
    status = ML.ML_addGeometryKernel(meshAssoc, geomKernel)
    if (0 != status):
        die('ML_addGeometryKernel failed')

    status = ML.ML_setActiveGeometryKernelByName(meshAssoc, 'Geode')
    if (0 != status):
        die('ML_setActiveGeometryKernelByName failed')
    return geomKernelObj

def GetActiveGeomKernel( meshAssoc ):    
    geomKernelObj = ML.MeshLinkObject()
    status = ML.ML_getActiveGeometryKernel(meshAssoc, geomKernelObj)
    if (0 != status):
        die('ML_getActiveGeometryKernel failed')
    geomKernel = geomKernelObj.value()
    return geomKernel
    
def DeleteGeode( geomKernelObj ):
    ML.ML_freeGeometryKernelGeodeObj( geomKernelObj )

  
    
def CreateMeshAssocObject( ):
    # create pointer to generic MeshLinkObject
    meshAssocObj = ML.MeshLinkObject()
    # load pointer with Mesh Associativity object
    status = ML.ML_createMeshAssociativityObj( meshAssocObj )
    if (0 != status):
        die('ML_createMeshAssociativityObj failed')
    return meshAssocObj
    
def DeleteMeshAssocObject( meshAssocObj ):
    ML.ML_freeMeshAssociativityObj( meshAssocObj )

def CreateProjectionDataObject( geomKernel ):
    # create pointer to generic MeshLinkObject
    dataObj = ML.MeshLinkObject()
    # load pointer with Projection Data object
    status = ML.ML_createProjectionDataObj( geomKernel, dataObj )
    if (0 != status):
        die('ML_createProjectionDataObj failed')
    return dataObj
    
def DeleteProjectionDataObject( dataObj ):
    ML.ML_freeProjectionDataObj( dataObj )

    
def ReadGeometryFiles( meshAssoc, geomKernel ):
    geomFileObj = ML.MeshLinkConstObject()
    numGeomFiles = ML.ML_getNumGeometryFiles( meshAssoc )

    for iFile in range(0, numGeomFiles):
        status = ML.ML_getGeometryFileObj(meshAssoc, iFile, geomFileObj)
        if (0 != status):
            die('ML_getGeometryFileObj failed')
        ret = ML.ML_getFilename(geomFileObj.value())
        if (0 != ret[0]):
            die('ML_getFilename failed')
        geom_fname = ret[1]
        status = ML.ML_readGeomFile(geomKernel, geom_fname)
        if (0 != status):
            die('ML_readGeomFile failed')

def GetMeshTopoInfo(meshAssoc, meshTopo):
    if meshTopo is None:
        die('ML_getMeshTopoInfo failed')
    ret = ML.ML_getMeshTopoInfo(meshAssoc, meshTopo)
    status = ret.pop(0)
    if (0 != status):
        die('ML_getMeshTopoInfo failed')
    ref = ret.pop(0)
    name = ret.pop(0)
    gref = ret.pop(0)
    mid = ret.pop(0)
    attIDs = ret.pop(0)
    attDict = {}
    for attID in attIDs:
        attName, attValue = GetAttributeInfo(meshAssoc, attID)
        attDict[attName] = attValue
    ret = [ref, name, gref, mid, attDict]
    return ret

def GetMeshTopoGref(meshTopo):
    ret = ML.ML_getMeshTopoGref(meshTopo)
    status = ret.pop(0)
    if (0 != status):
        die('ML_getMeshTopoGref failed')
    gref = ret.pop(0)
    return gref



def GetMeshPointInfo(meshAssoc, meshPoint):
    if meshPoint is None:
        die('ML_getMeshPointInfo failed')
    paramVertObj = ML.MeshLinkConstObject()
    ret = ML.ML_getMeshPointInfo(meshAssoc, meshPoint, paramVertObj)
    status = ret.pop(0)
    if (0 != status):
        die('ML_getMeshPointInfo failed')
    ref = ret.pop(0)
    name = ret.pop(0)
    gref = ret.pop(0)
    mid = ret.pop(0)
    attIDs = ret.pop(0)
    attDict = {}
    for attID in attIDs:
        attName, attValue = GetAttributeInfo(meshAssoc, attID)
        attDict[attName] = attValue
    # ref, name, gref, mid, attDict, paramVertex
    ret = [ref, name, gref, mid, attDict, paramVertObj.value()]
    return ret

def GetMeshEdgeInfo(meshAssoc, meshEdge):
    if meshEdge is None:
        die('ML_getMeshEdgeInfo failed')
    paramVertObj = ML.MeshLinkConstObject()
    ret = ML.ML_getMeshEdgeInfo(meshAssoc, meshEdge)
    status = ret.pop(0)
    if (0 != status):
        die('ML_getMeshEdgeInfo failed')
    print(ret)

    ref = ret.pop(0)
    name = ret.pop(0)
    gref = ret.pop(0)
    mid = ret.pop(0)
    numAttIDs = ret.pop(0)
    attDict = {}
    for i in range(0, numAttIDs):
        attName, attValue = GetAttributeInfo(meshAssoc, ret.pop(0))
        attDict[attName] = attValue
    # ref, name, gref, mid, attDict, paramVertex
    ret = [ref, name, gref, mid, attDict, paramVertObj.value()]
    return ret

def GetAttributeInfo(meshAssoc, attID):
    ret = ML.ML_getAttribute(meshAssoc, attID)
    status = ret[0]
    if (0 != status):
        die('ML_getAttribute failed')
    return [ret[1], ret[2]]

def GetMeshModelByName( meshAssoc, name ):
    meshModelObj = ML.MeshLinkObject()
    status = ML.ML_getMeshModelByName( meshAssoc, name, meshModelObj )
    if (0 != status):
        return None
    return meshModelObj.value()

def FindLowestTopoPointByInd( meshModel, index ):
    '''Return point matching index on lowest topological level'''
    meshPointObj = ML.MeshLinkObject()
    status = ML.ML_findLowestTopoPointByInd(meshModel, index, meshPointObj)
    if (0 != status):
        return None
    return meshPointObj.value()

def FindHighestTopoPointByInd( meshModel, index ):
    '''Return point matching index on MeshModel topological level'''
    meshPointObj = ML.MeshLinkObject()
    status = ML.ML_findHighestTopoPointByInd(meshModel, index, meshPointObj)
    if (0 != status):
        return None
    return meshPointObj.value()

def FindLowestTopoEdgeByInds( meshModel, indices ):
    meshEdgeObj = ML.MeshLinkObject()
    status = ML.ML_findLowestTopoEdgeByInds(meshModel, indices, len(indices), meshEdgeObj)
    if (0 != status):
        return None
    return meshEdgeObj.value()

def GetGeometryGroupByID( meshAssoc, gid ):
    geomGroupObj = ML.MeshLinkObject()
    status = ML.ML_getGeometryGroupByID(meshAssoc, gid, geomGroupObj)
    if (0 != status):
        return None
    return geomGroupObj.value()


def GetGeomGroupEntityNames( geomGroup ):
    ret = ML.ML_getEntityNames( geomGroup )
    status = ret.pop(0)
    if (0 != status):
        die('ML_getEntityNames failed')
    return ret.pop(0)

def GetParamVertices( meshTopo ):
    meshTopoConst = ML.MeshLinkObject_to_const( meshTopo )
    ret = ML.ML_getParamVerts( meshTopoConst )
    status = ret.pop(0)
    if (0 != status):
        die('ML_getParamVerts failed')
    return ret.pop(0)

def GetParamVertexInfo( paramVert ):
    ret = ML.ML_getParamVertInfo( paramVert )
    status = ret.pop(0)
    if (0 != status):
        die('ML_getParamVertInfo failed')
    # vref, gref, mid, uv
    return ret

def ProjectPointToMeshTopoGeometry( meshAssoc, geomKernel, meshTopo, point ):
    projectInfo = {}
    projectInfo['originalXYZ'] = point
    projectInfo['success'] = False

    geomGroup = GetGeometryGroupByID( meshAssoc, GetMeshTopoGref( meshTopo ) )
    if geomGroup is None:
        print('ProjectPointToMeshTopoGeometry: missing geometry group')
        return projectInfo

    projectionDataObj = CreateProjectionDataObject( geomKernel )
    projectionData = projectionDataObj.value()

    status = ML.ML_projectPoint( geomKernel, geomGroup, point, projectionData)
    if (0 != status):
        die('ML_projectPoint failed')

    ret = ML.ML_getProjectionInfo(geomKernel, projectionData)
    status = ret.pop(0)
    if (0 != status):
        die('ML_projectPoint failed')
    # projectedPt, UV, entity_name 
    projectInfo['success'] = True
    projectInfo['XYZ'] = ret.pop(0)
    projectInfo['UV'] = ret.pop(0)
    projectInfo['hitEntityName'] = ret.pop(0)

    DeleteProjectionDataObject( projectionDataObj )
    return projectInfo

def GeometryEvalXYZ( geomKernel, UV, entityName ):
    ret = ML.ML_evalXYZ( geomKernel, UV, entityName)
    status = ret.pop(0)
    if (0 != status):
        die('ML_evalXYZ failed')
    return ret.pop(0)


def GeometryEvalRadiusOfCurvature( geomKernel, UV, entityName ):
    ret = ML.ML_evalRadiusOfCurvature( geomKernel, UV, entityName)
    status = ret.pop(0)
    if (0 != status):
        die('ML_evalRadiusOfCurvature failed')
    return ret.pop(0)


def PointToPointDistance( p0, p1 ):
    print('Dist from \n',p0,' to \n', p1)
    return math.sqrt((p0[0] - p1[0])**2 + (p0[1] - p1[1])**2 + (p0[2] - p1[2])**2)


def CalcMidPoint( p0, p1 ):
    return [ (p0[0] + p1[0])*0.5 , (p0[1] + p1[1])*0.5 , (p0[2] + p1[2])*0.5 ]


#############################################################################
#
# This file is licensed under the Cadence Public License Version 1.0 (the
# "License"), a copy of which is found in the included file named "LICENSE",
# and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
# LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
# ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
# Please see the License for the full text of applicable terms.
#
#############################################################################
