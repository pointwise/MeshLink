#############################################################################
#
# (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
#
# This sample source code is not supported by Cadence Design Systems, Inc.
# It is provided freely for demonstration purposes only.
# SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
#
#############################################################################

import sys
sys.path.append('../src/SWIG/meshlink_python')

from MeshLinkClasses import *
from MeshLinkFunctions import *

for func in dir(ML):
    print(func)        

print('\n')
for func in dir():
    print(func)        
    
print('\nTesting')

            
target_block_name = '/Base/sphere'
meshlink_filename = 'sphere_ml.xml'

# Create MeshAssociativity Object
meshAssocObj = CreateMeshAssocObject() # pointer to object
meshAssoc = meshAssocObj.value()       # the object

# Parse the MeshLink file - load MeshAssocation data
ParseMeshLinkFile(meshlink_filename, meshAssoc) 


# Does a meshModel for the target block exist?
meshModel = GetMeshModelByName( meshAssoc, target_block_name )
if meshModel is None:
    die('Missing MeshModel named', target_block_name)


    
# Create Geode GeometryKernel Object
geomKernelObj = CreateGeode( meshAssoc )    
geomKernel = GetActiveGeomKernel( meshAssoc )

# Get the attributes of the meshModel
meshModelInfo = MeshTopoInfo( meshAssoc, meshModel )
meshModelInfo.printInfo()

# Import geometry referenced in the MeshLink file    
ReadGeometryFiles( meshAssoc, geomKernel )



##############################################################################################
print('\n\n######  Mesh Point Test  ############\n')
# Test MeshPoint data (lowest topological match for point index)
bottom_con_pt = [ -0.49742707, 0.00074147824, 0.49999283 ]
bottom_con_pt_ind = 17;
bottom_con_pt_gref = 15;
bottom_con_pt_uv = [0.625156631213186, 0.0]
bottom_con_pt_entity_name = 'bottom_con_1';
bottom_con_pt_radius = 0.501719


meshPoint = FindLowestTopoPointByInd( meshModel, bottom_con_pt_ind )
if meshPoint is None:
    die('Missing MeshPoint for index ', bottom_con_pt_ind)


# Get the attributes of the meshPoint
meshPointInfo = MeshPointInfo( meshAssoc, meshPoint )
if (meshPointInfo.gref != bottom_con_pt_gref):
	die('Bad MeshPoint gref:', meshPointInfo.gref, '!=', bottom_con_pt_gref)

geomGroup = GetGeometryGroupByID( meshAssoc, meshPointInfo.gref )
if geomGroup is None:
	die('Missing GeometryGroup for gid:', meshPointInfo.gref)

entityNames = GetGeomGroupEntityNames( geomGroup )
if (len(entityNames) != 1):
	die('Bad GeometryGroup entity count:', len(entityNames), '!=', 1)

if (entityNames[0] != bottom_con_pt_entity_name):
	die('Bad GeometryGroup entity name:', entityNames[0], '!=', bottom_con_pt_entity_name)

if meshPointInfo.paramVertex is None:
	die('Missing ParamVertex for MeshPoint')

paramVertInfo = ParamVertexInfo(meshPointInfo.paramVertex)
paramVertInfo.printInfo()
if (paramVertInfo.UV[0] != bottom_con_pt_uv[0]):
	die('Bad ParamVertex U:', paramVertInfo.UV[0], '!=', bottom_con_pt_uv[0])
if (paramVertInfo.UV[1] != bottom_con_pt_uv[1]):
	die('Bad ParamVertex V:', paramVertInfo.UV[1], '!=', bottom_con_pt_uv[1])

# Access the MeshPoint's ParamVertices directly
print("\nAccess the MeshPoint's ParamVertices directly")
pvs = GetParamVertices( meshPoint )
for pv in pvs:
	paramVertInfo = ParamVertexInfo(pv)
	paramVertInfo.printInfo()


# Test point projection
tol = 1e-6
projectInfo = ProjectPointToMeshTopoGeometry( meshAssoc, geomKernel, meshPoint, bottom_con_pt )
dist = PointToPointDistance( bottom_con_pt, projectInfo['XYZ'] )
if dist > tol:
	die('Bad point projection', dist,' > ', tol)

# Test parametric evalation
evalXYZ = GeometryEvalXYZ( geomKernel, projectInfo['UV'], projectInfo['hitEntityName'] )
dist = PointToPointDistance( bottom_con_pt, evalXYZ )
if dist > tol:
	die('Bad parametric evaluation', dist,' > ', tol)

# Test parametric radius of curvature evalation
evalRadius = GeometryEvalRadiusOfCurvature( geomKernel, projectInfo['UV'], projectInfo['hitEntityName'] )
diff = abs(bottom_con_pt_radius - evalRadius)
if diff > tol:
	die('Bad radius of curvature evaluation', evalRadius,' != ', bottom_con_pt_radius)


##############################################################################################
print('\n\n######  My Special Mesh Point Test  ############\n')
# Test MeshPoint data (special point reference at MeshModel level)
my_special_pt = [ -0.47677290802217431, 0.29913675338094192, 0.39997213024780004 ]
my_special_pt_ind = 17;
my_special_pt_gref = 15;
my_special_pt_uv = [0.1234, 0.0]
my_special_pt_entity_name = 'bottom_con_1';

meshPoint = FindHighestTopoPointByInd( meshModel, my_special_pt_ind )
if meshPoint is None:
    die('Missing MeshPoint for index ', my_special_pt_ind)


# Get the attributes of the meshPoint
meshPointInfo = MeshPointInfo( meshAssoc, meshPoint )
if (meshPointInfo.gref != my_special_pt_gref):
	die('Bad MeshPoint gref:', meshPointInfo.gref, '!=', my_special_pt_gref)

geomGroup = GetGeometryGroupByID( meshAssoc, meshPointInfo.gref )
if geomGroup is None:
	die('Missing GeometryGroup for gid:', meshPointInfo.gref)

entityNames = GetGeomGroupEntityNames( geomGroup )
if (len(entityNames) != 1):
	die('Bad GeometryGroup entity count:', len(entityNames), '!=', 1)

if (entityNames[0] != my_special_pt_entity_name):
	die('Bad GeometryGroup entity name:', entityNames[0], '!=', my_special_pt_entity_name)

if meshPointInfo.paramVertex is None:
	die('Missing ParamVertex for MeshPoint')

paramVertInfo = ParamVertexInfo(meshPointInfo.paramVertex)
paramVertInfo.printInfo()
if (paramVertInfo.UV[0] != my_special_pt_uv[0]):
	die('Bad ParamVertex U:', paramVertInfo.UV[0], '!=', my_special_pt_uv[0])
if (paramVertInfo.UV[1] != my_special_pt_uv[1]):
	die('Bad ParamVertex V:', paramVertInfo.UV[1], '!=', my_special_pt_uv[1])




##############################################################################################
print('\n\n######  Mesh Edge Test  ############\n')
# Test MeshEdge data (lowest topological match for edge indices)

edgePoints = [ [ -0.5, 0.33348231, 0.37254469] , [-0.49742707, 0.00074147824, 0.49999283 ] ]
edgeInds = [ 18, 17 ];
# known-good projected and parametric interpolated mid-edge XYZs
projectedMidEdgePt = [ -0.4530720856, 0.1770840201, 0.4652300728 ]
interpolatedMidEdgePt = [ -0.45617166, 0.23260459, 0.440425 ]
edge_gref = [ 15, 15 ]
edge_u = [ 0.0, 0.625156631213186 ]
edge_entity_name = "bottom_con_1";

meshEdge = FindLowestTopoEdgeByInds( meshModel, edgeInds )
if meshEdge is None:
    die('Missing MeshEdge for indices ', edgeInds)


# Get the attributes of the meshEdge
meshEdgeInfo = MeshEdgeInfo( meshAssoc, meshEdge )
meshEdgeInfo.printInfo()
if (meshEdgeInfo.gref != edge_gref[0]):
	die('Bad MeshPoint gref:', meshEdgeInfo.gref, '!=', edge_gref[0])

geomGroup = GetGeometryGroupByID( meshAssoc, meshEdgeInfo.gref )
if geomGroup is None:
	die('Missing GeometryGroup for gid:', meshEdgeInfo.gref)

entityNames = GetGeomGroupEntityNames( geomGroup )
if (len(entityNames) != 1):
	die('Bad GeometryGroup entity count:', len(entityNames), '!=', 1)

if (entityNames[0] != edge_entity_name):
	die('Bad GeometryGroup entity name:', entityNames[0], '!=', edge_entity_name)

if meshEdgeInfo.paramVertices is None:
	die('Missing ParamVertex for MeshEdge')

pvind = 0
for pv in meshEdgeInfo.paramVertices:
	print('Edge PV ',pvind+1)
	paramVertInfo = ParamVertexInfo(pv)
	paramVertInfo.printInfo()
	if (paramVertInfo.UV[0] != edge_u[pvind]):
		die('Bad ParamVertex U:', paramVertInfo.UV[0], '!=', edge_u[pvind])
	if (paramVertInfo.UV[1] != 0.0):
		die('Bad ParamVertex V:', paramVertInfo.UV[1], '!=', 0.0)
	pvind += 1


# Project edge midpoint onto associated edge geometry
midPoint = CalcMidPoint(edgePoints[0], edgePoints[1])
projectInfo = ProjectPointToMeshTopoGeometry( meshAssoc, geomKernel, meshEdge, midPoint )
dist = PointToPointDistance( projectedMidEdgePt, projectInfo['XYZ'] )
if dist > tol:
	die('Bad point projection', dist,' > ', tol)


# clean up
DeleteGeode( geomKernelObj )
DeleteMeshAssocObject(meshAssocObj)    

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
