#############################################################################
#
# (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
#
# This sample source code is not supported by Cadence Design Systems, Inc.
# It is provided freely for demonstration purposes only.
# SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
#
#############################################################################

import MeshLink as ML
from MeshLinkFunctions import *

class ParamVertexInfo:
    # initialize from the ParamVertex object
    def __init__(self, paramVertex):
        self.paramVertex = paramVertex
        # reference string to point in mesh data file
        self.vref = ''
        # optional integer matching geometry reference GID
        self.gref = -1
        # optional integer ID
        self.mid = -1
        self.UV = []

        if self.paramVertex is None:
            die('error instantiating ParamVertexInfo')
        else:
            ret = GetParamVertexInfo( self.paramVertex )
            # vref, gref, mid, u, v
            self.vref   = ret.pop(0)
            self.gref   = ret.pop(0)
            self.mid    = ret.pop(0)
            self.UV     = ret.pop(0)

    def printInfo(self, indent=''):
        if indent != '':
            print(indent,'ParamVertexInfo')
            print(indent,'  vref = ',self.vref)
            print(indent,'  gref = ',self.gref)
            print(indent,'  mid = ',self.mid)
            print(indent,'  UV = ',self.UV)
        else:
            print('ParamVertexInfo')
            print('  vref = ',self.vref)
            print('  gref = ',self.gref)
            print('  mid = ',self.mid)
            print('  UV = ',self.UV)


class MeshTopoInfo:
    # initialize from the MeshLink object
    def __init__(self, meshAssoc, ML_object):
        self.ML_object = ML_object
        # optional integer identifier
        self.mid = -1
        # optional name string
        self.name = ''
        # optional reference string into mesh data file
        self.ref = ''
        # optional integer matching geometry reference GID
        self.gref = -1
        # optional integer matching attribute ATTID
        self.aref = -1
        self.attDict = {} # attribute name=value dictionary corresponding to aref

        if self.ML_object is None:
            die('error instantiating MeshTopoInfo')
        else:
            ret = GetMeshTopoInfo(meshAssoc, self.ML_object)
            self.ref    = ret[0]
            self.name   = ret[1]
            self.gref   = ret[2]
            self.mid    = ret[3]
            self.attDict = ret[4]

    def set_name(self, name):
        self.name = name

    def printInfo(self, indent=''):
        if indent != '':
            print(indent,'MeshTopoInfo')
            print(indent,'  ref = ',self.ref)
            print(indent,'  name = ',self.name)
            print(indent,'  gref = ',self.gref)
            print(indent,'  mid = ',self.mid)
            print(indent,'  numAtts = ',len(self.attDict))
            for attName,attValue in self.attDict.items():
                print(indent,'   ',attName,' = ',attValue)
        else:
            print('MeshTopoInfo')
            print('  ref = ',self.ref)
            print('  name = ',self.name)
            print('  gref = ',self.gref)
            print('  mid = ',self.mid)
            print('  numAtts = ',len(self.attDict))
            for attName,attValue in self.attDict.items():
                print('   ',attName,' = ',attValue)

    

class MeshPointInfo(MeshTopoInfo):
    # initialize from the MeshLink object
    def __init__(self, meshAssoc, ML_object):
        # call parent initializer
        super().__init__(meshAssoc, ML_object)

        self.paramVertex = None
        if self.ML_object is None:
            die('error instantiating MeshPointInfo')
        else:
            # Access the MeshPoint's ParamVertices directly
            self.paramVertex = GetParamVertices( ML_object )

        if self.paramVertex is not None:
            # there should be only one
            if len(self.paramVertex) != 1:
                die('error instantiating MeshPointInfo (bad ParamVertex)')
            self.paramVertex = self.paramVertex.pop(0)

class MeshEdgeInfo(MeshTopoInfo):
    # initialize from the MeshLink object
    def __init__(self, meshAssoc, ML_object):
        # call parent initializer
        super().__init__(meshAssoc, ML_object)

        self.paramVertices = []
        if self.ML_object is None:
            die('error instantiating MeshEdgeInfo')
        else:
            # Access the MeshEdge's ParamVertices directly
            self.paramVertices = GetParamVertices( ML_object )

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
