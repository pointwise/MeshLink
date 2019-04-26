/***********************************************************************
**  MeshLink Library Test Harness (C++)
**
**  Read MeshLink file.
**     + print diagnostic info to console
**
**  If Project Geode geometry kernel present:
**     + read NMB geometry file
**     + test various computational geometry access functions
**/

#include "Types.h"
#if defined(HAVE_GEODE)
#include "GeomKernel_Geode.h"
#endif
#include "MeshAssociativity.h"
#if defined(HAVE_XERCES)
#include "MeshLinkParser_xerces.h"
#endif

#include <stdio.h>
#include <cmath>

// Test the mesh-geometry associativity in sphere_ml.xml
static int sphere_ml_tests(MeshAssociativity &meshAssoc);

// Closest point projection onto geometry of constrained meshTopo entity
static int projectToMeshTopoGeometry(
    MeshAssociativity &meshAssoc,
    MeshTopo *meshTopo,
    MLVector3D point,
    MLVector3D expectedProjectedPoint
);

// Evaluation of parametric coordinates
static int evaluateParamPoint(
    MeshAssociativity &meshAssoc,
    MLVector2D UV,
    const char *entityName,
    MLVector3D expectedEvaluationPoint,
    MLREAL expectedRadiusOfCurvature
);

// Parametric interpolation at mid-point of constrained mesh edge
static int interpolateEdgeMidPoint(
    MeshAssociativity &meshAssoc,
    MeshEdge *edge,
    MLVector3D expectedInterpolatedPoint);


// Parametric interpolation at mid-point of constrained mesh face
static int interpolateFaceMidPoint(
    MeshAssociativity &meshAssoc,
    MeshFace *face,
    MLVector3D expectedInterpolatedPoint);

//===============================================================================
// MAIN routine
int main(int argc, char** argv)
{
    int ret = 0;
    if (argc != 2) {
        printf("usage: <program name> <xml file name>\n");
        exit(1);
    }

    // Name of geometry-mesh associativity file
    std::string meshlink_fname(argv[1]);
    std::string schema_fname;  // empty schema filename causes schemaLocation in meshlink file to be used

    MeshAssociativity meshAssoc;

#if defined(HAVE_XERCES)
    // Read Geometry-Mesh associativity
    {
        // Xerces MeshLink XML parser
        MeshLinkParserXerces parser;

        // Validate first
        parser.validate(meshlink_fname, schema_fname);

        printf("\nParsing %s...\n\n", meshlink_fname.c_str());
        if (!parser.parseMeshLinkFile(meshlink_fname, &meshAssoc)) {
            printf("Error parsing geometry-mesh associativity\n");
            return (-1);
        }
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif

#if defined(HAVE_GEODE)
    if (meshlink_fname.compare("sphere_ml.xml") == 0) {
        // Test the mesh-geometry associativity in sphere_ml.xml
        if (0 != sphere_ml_tests(meshAssoc)) {
            printf("Error testing sphere_ml.xml geometry-mesh associativity\n");
            return (-1);
        }
    }
#endif

    return ret;
}


/*===============================================================================================
* MESH POINT Test (lowest topological match for point index )
*/
int test_lowest_mesh_point(
    MeshAssociativity &meshAssoc,
    MeshModel* meshModel
)
{
    printf("\nMESH POINT Test (lowest topological match for point index )\n");
    int ret = 0;

    printf(" MeshPoint defined in MeshString name = \"root/bottom_con\"\n");
    /* Test data for MeshPoint defined in MeshString name = "root/bottom_con" */
    MLVector3D bottom_con_pt = { -0.49742707, 0.00074147824, 0.49999283 };
    MLINT  bottom_con_pt_ind = 17;
    MLINT  bottom_con_pt_gref = 15;
    MLREAL bottom_con_pt_u = 0.625156631213186;
    MLREAL bottom_con_pt_radius = 0.501719;
    const char *bottom_con_pt_entity_name = "bottom_con_1";

    /* Find the point at the lowest topological level (MeshString, MeshSheet, or MeshModel) */
    MeshPoint *meshPoint = meshModel->findLowestTopoPointByInd(bottom_con_pt_ind);
    if (NULL != meshPoint) {
        ParamVertex *const paramVert = meshPoint->getParamVert();
        if (paramVert) {
            // have parametric data
            MLINT gref;
            gref = paramVert->getGref();
            MLVector2D UV;
            paramVert->getUV(&(UV[0]), &(UV[1]));
            if (gref != bottom_con_pt_gref || UV[0] != bottom_con_pt_u) {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }

            GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
            if (NULL == geomGroup) {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                if (entityNames.size() != 1 ||
                    entityNames[0].compare(bottom_con_pt_entity_name) != 0) {
                    printf("Error: incorrect point parametric data\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }

                if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(), 
                    bottom_con_pt, bottom_con_pt_radius)) {
                    printf("Error: bad point parametric evaluation\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  parametric evaluation OK\n");
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ml_assert(0 == 1);
            ret = 1;
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt, bottom_con_pt)) {
            printf("Error: bad point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }

    }
    else {
        printf("Error: missing constrained point\n");
        ml_assert(NULL != meshPoint);
        ret = 1;
    }


    /* Test a point which we know to be defined in the "dom-4" MeshSheet level,
     * but not at the MeshString level 
     */
    printf(" MeshPoint defined in MeshSheet name = \"root/dom-4\"\n");
    /* Test data for MeshPoint defined in MeshSheet name="root/dom-4" */
    MLVector3D dom4_pt = { -0.5, 0.3431516, 0.58247119 };
    MLINT dom4_pt_ind = 15;
    MLINT dom4_pt_gref = 2;
    MLREAL dom4_pt_UV[2] = { 89.9999999933445, 99.4938870159108 };
    const char *dom4_pt_entity_name = "surface-3";
    MLREAL dom4_pt_radius = 0.5;

    /* Find the point at the lowest topological level (MeshString, MeshSheet, or MeshModel) */
    meshPoint = meshModel->findLowestTopoPointByInd(dom4_pt_ind);
    if (meshPoint) {
        ParamVertex *const paramVert = meshPoint->getParamVert();
        if (paramVert) {
            // have parametric data
            MLINT gref;
            gref = paramVert->getGref();
            MLVector2D UV;
            paramVert->getUV(&(UV[0]), &(UV[1]));
            if (gref != dom4_pt_gref ||
                UV[0] != dom4_pt_UV[0] ||
                UV[1] != dom4_pt_UV[1]
                ) {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }

            GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
            if (NULL == geomGroup) {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                if (entityNames.size() != 1 ||
                    entityNames[0].compare(dom4_pt_entity_name) != 0) {
                    printf("Error: incorrect point parametric data\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }

                if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                    dom4_pt, dom4_pt_radius)) {
                    printf("Error: bad point parametric evaluation\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  parametric evaluation OK\n");
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ml_assert(0 == 1);
            ret = 1;
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, dom4_pt, dom4_pt)) {
            printf("Error: bad point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }
    }
    else {
        printf("Error: missing constrained point\n");
        ml_assert(NULL != meshPoint);
        ret = 1;
    }
    return ret;
}



/*===============================================================================================
* MESH POINT Test (highest topological match for point index )
*/
int test_highest_mesh_point(
    MeshAssociativity &meshAssoc,
    MeshModel* meshModel
)
{
    printf("\nMESH POINT Test (highest topological match for point index )\n");
    int ret = 0;
    MLVector3D bottom_con_pt = { -0.47677290802217431, 0.29913675338094192, 0.39997213024780004 };
    MLINT bottom_con_pt_ind = 17;
    MLINT bottom_con_pt_gref = 15;
    MLREAL bottom_con_pt_u = 0.1234;
    const char *bottom_con_pt_entity_name = "bottom_con_1";
    MLREAL bottom_con_pt_radius = 0.24873;

    /* Find the point at the highest topological level (MeshModel) */
    MeshPoint *meshPoint = meshModel->findHighestTopoPointByInd(bottom_con_pt_ind);
    if (NULL != meshPoint) {
        ParamVertex *const paramVert = meshPoint->getParamVert();
        if (paramVert) {
            // have parametric data
            MLINT gref;
            gref = paramVert->getGref();
            MLVector2D UV;
            paramVert->getUV(&(UV[0]), &(UV[1]));
            if (gref != bottom_con_pt_gref || UV[0] != bottom_con_pt_u) {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }

            GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
            if (NULL == geomGroup) {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                if (entityNames.size() != 1 ||
                    entityNames[0].compare(bottom_con_pt_entity_name) != 0) {
                    printf("Error: incorrect point parametric data\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }

                if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                    bottom_con_pt, bottom_con_pt_radius)) {
                    printf("Error: bad point parametric evaluation\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  parametric evaluation OK\n");
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ml_assert(0 == 1);
            ret = 1;
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt, bottom_con_pt)) {
            printf("Error: bad point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }
    }
    else {
        printf("Error: missing constrained point\n");
        ml_assert(NULL != meshPoint);
        ret = 1;
    }

    return ret;
}



/*===============================================================================================
* MESH EDGE Test (lowest topological match for edge point indices )
*/
int test_mesh_edge(
    MeshAssociativity &meshAssoc,
    MeshModel* meshModel
)
{
    printf("\nMESH EDGE Test (lowest topological match for edge point indices )\n");
    int ret = 0;

    printf(" MeshEdge defined in MeshString name = \"root/bottom_con\"\n");
    /* Test data for MeshEdge defined in MeshString name = "root/bottom_con" */

    MLVector3D edgePoints[2] = { -0.5, 0.33348231, 0.37254469, -0.49742707, 0.00074147824, 0.49999283 };
    MLINT edgeInds[2] = { 18, 17 };

    const char *edge_entity_name = "bottom_con_1";
    MLINT  edge_gref[2] = { 15, 15 };

    MLREAL edge_u[2] = { 0, 0.625156631213186, };
    MLREAL edge_radius[2] = { 0.239211, 0.501719 };


    // known-good projected and parametric interpolated mid-edge XYZs
    MLVector3D projectedMidEdgePt = { -0.45307208568368834, 0.17708402010909388, 0.46523007284560064 };
    MLVector3D interpolatedMidEdgePt = { -0.45617166, 0.23260459, 0.440425 };


    MLVector3D point;

    MeshEdge *edge =
        meshModel->findLowestTopoEdgeByInds(edgeInds[0], edgeInds[1]);
    if (edge) {
        ParamVertex *const* paramVerts;
        MLINT numVerts = edge->getParamVerts( &paramVerts );
        for (MLINT i = 0; i < numVerts; ++i) {
            ParamVertex const* paramVert = paramVerts[i];
            if (paramVert) {
                // have parametric data
                MLINT gref;
                gref = paramVert->getGref();
                MLVector2D UV;
                paramVert->getUV(&(UV[0]), &(UV[1]));
                if (gref != edge_gref[i] || 
                    UV[0] != edge_u[i]) {
                    printf("Error: incorrect point parametric data\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }

                GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
                if (NULL == geomGroup) {
                    printf("Error: incorrect point parametric data\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }
                else {
                    const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                    if (entityNames.size() != 1 ||
                        entityNames[0].compare(edge_entity_name) != 0) {
                        printf("Error: incorrect point parametric data\n");
                        ml_assert(0 == 1);
                        ret = 1;
                    }

                    if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                        edgePoints[i], edge_radius[i])) {
                        printf("Error: bad point parametric evaluation\n");
                        ml_assert(0 == 1);
                        ret = 1;
                    }
                    else {
                        printf("  parametric evaluation OK\n");
                    }
                }
            }
            else {
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }
        }


        // original edge was associated with geometry group
        // project interpolated point onto geometry group
        for (int n = 0; n < 3; ++n) {
            point[n] = 0.5*(edgePoints[0][n] + edgePoints[1][n]);
        }
        if (0 != projectToMeshTopoGeometry(meshAssoc, edge, point, projectedMidEdgePt)) {
            printf("bad edge point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }

        if (0 != interpolateEdgeMidPoint(meshAssoc, edge, interpolatedMidEdgePt)) {
            printf("Error: bad edge parametric interpolation\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric interpolation OK\n");
        }
    }
    else {
        printf("Error: missing constrained edge\n");
        ml_assert(NULL != edge);
        ret = 1;
    }
    return ret;
}


/*===============================================================================================
* MESH FACE Test 
*/
int test_mesh_face(
    MeshAssociativity &meshAssoc,
    MeshModel* meshModel
)
{
    printf("\nMESH FACE Test (find match for face point indices )\n");
    int ret = 0;

    //===============================================================================================
    // MESH FACE Test
    // sphere bottom dom2 face points and indices
    MLVector3D facePoints[4] = { -0.5, 0.33348231, 0.37254469, -0.49742707, 0.00074147824, 0.49999283,
                        -0.75349552, 0.25348322, 0.34854497 , -0.83348231, 5.5113687e-10, 0.37254469 };
    MLINT faceInds[4] = { 18, 17, 27, 26 };
    // known-good projected and parametric interpolated mid-face XYZs
    MLVector3D projectedMidFacePt = { -0.66267164, 0.16359104, 0.44359431 };
    MLVector3D interpolatedMidFacePt = { -0.64859257709701390, 0.15925496600015798, 0.45006455296541237 };

    MLVector3D point;
    MeshFace *face =
        meshModel->findFaceByInds(faceInds[0], faceInds[1], faceInds[2], faceInds[3]);
    if (face) {
        // original face was associated with geometry group
        // project interpolated point onto geometry group
        for (int n = 0; n < 3; ++n) {
            point[n] = 0.25*(facePoints[0][n] +
                facePoints[1][n] +
                facePoints[2][n] +
                facePoints[3][n]);
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, face, point, projectedMidFacePt)) {
            printf("Error: bad face point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }

        if (0 != interpolateFaceMidPoint(meshAssoc, face, interpolatedMidFacePt)) {
            printf("Error: bad face parametric interpolation\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric interpolation OK\n");
        }
    }
    else {
        printf("Error: missing constrained face\n");
        ml_assert(NULL != face);
        ret = 1;
    }
    return ret;
}



//===============================================================================
// Test the mesh-geometry associativity in sphere_ml.xml
int
sphere_ml_tests(MeshAssociativity &meshAssoc)
{
    int ret = 0;
#if defined(HAVE_GEODE)

    printf("\nSphere_ml.xml Tests\n");

    // Name of mesh model
    const char *target_block_name = "/Base/sphere";
    MeshModel* meshModel = meshAssoc.getMeshModelByName(target_block_name);
    if (meshModel) {

        // Load Project Geode Kernel and set as active kernel
        GeometryKernelGeode geomKernel;
        meshAssoc.addGeometryKernel(&geomKernel);
        meshAssoc.setActiveGeometryKernelByName(geomKernel.getName());

        // Read geometry files
        MLINT iFile;
        MLINT numGeomFiles = meshAssoc.getNumGeometryFiles();
        const std::vector<GeometryFile> &geomFiles = meshAssoc.getGeometryFiles();

        for (iFile = 0; iFile < numGeomFiles; ++iFile) {
            const GeometryFile &geomFile = geomFiles[iFile];
            const char * geom_fname = geomFile.getFilename();

            printf("\nGeometryFile Attributes\n");
            std::vector<MLINT> attIDs = geomFile.getAttributeIDs(meshAssoc);
            MLINT numAtts = attIDs.size();
            MLINT iAtt;
            for (iAtt = 0; iAtt < numAtts; ++iAtt) {
                const char *attName, *attValue;
                if (meshAssoc.getAttribute(attIDs[iAtt], &attName, &attValue)) {
                    printf("  %" MLINT_FORMAT " %s = %s\n", iAtt, attName, attValue);
                }
            }

            if (!geomKernel.read(geom_fname)) {
                printf("Error reading geometry file\n  %s\n", geom_fname);
                return (-1);
            }
        }

        MLREAL tol = 1e-5;
        GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
        if (!geom_kernel) {
            printf("ERROR: no active geometry kernel\n");
            ret = 1;
        }
        else {

            if (0 != test_lowest_mesh_point(meshAssoc, meshModel)) {
                printf("MESH Point Test: test_lowest_mesh_point failed\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("MESH Point Test: test_lowest_mesh_point OK\n");
            }


            if (0 != test_highest_mesh_point(meshAssoc, meshModel)) {
                printf("MESH Point Test: test_highest_mesh_point failed\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("MESH Point Test: test_highest_mesh_point OK\n");
            }


            if (0 != test_mesh_edge(meshAssoc, meshModel)) {
                printf("MESH Edge Test: test_mesh_edge failed\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("MESH Edge Test: test_mesh_edge OK\n");
            }


            if (0 != test_mesh_face(meshAssoc, meshModel)) {
                printf("MESH Face Test: test_mesh_face failed\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("MESH Face Test: test_mesh_face OK\n");
            }

        }
    }
    else {
        printf("missing Mesh Model\n");
        ml_assert(NULL != meshModel);
        ret = 1;
    }
#endif
    return ret;
}

// Closest point projection onto geometry of constrained meshTopo entity
int projectToMeshTopoGeometry(
    MeshAssociativity &meshAssoc,
    MeshTopo *meshTopo,
    MLVector3D point,
    MLVector3D expectedProjectedPoint
)
{
    int status = 0;
    if (!meshTopo) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
    if (!geom_kernel) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        ProjectionData projectionData(geom_kernel);
        GeometryGroup *geom_group =
            meshAssoc.getGeometryGroupByID(meshTopo->getGref());
        if (geom_group) {
            // point associated with geometry group
            // project point onto geometry group
            if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                printf("Point projection failed\n");
                return 1;
            }
            else {
                MLVector3D projectedPt = { 0.0, 0.0,0.0 };
                geom_kernel->getProjectionXYZ(projectionData, projectedPt);
                MLREAL dist = 0.0;
                for (int n = 0; n < 3; ++n) {
                    dist += std::pow((expectedProjectedPoint[n] - projectedPt[n]), 2.0);
                }
                dist = std::sqrt(dist);
                if (dist > tol) {
                    printf("bad point projection\n");
                    ml_assert(dist < tol);
                    return 1;
                }

            }
        }
        else {
            printf("missing Geometry Group\n");
            ml_assert(NULL != geom_group);
            return 1;
        }
    }

    return status;
}

// Evaluation of parametric coordinates
int evaluateParamPoint(
    MeshAssociativity &meshAssoc,
    MLVector2D UV,
    const char *entityName,
    MLVector3D expectedEvaluationPoint,
    MLREAL expectedRadiusOfCurvature
)
{
    int status = 0;
    MLREAL tol = 1e-5;
    GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
    if (!geom_kernel) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        // Test geometry XYZ at ParamVert location
        MLVector3D evaluationPoint;
        if (geom_kernel->evalXYZ(UV, entityName, evaluationPoint)) {
            MLREAL dist = 0.0;
            for (int n = 0; n < 3; ++n) {
                dist += std::pow((expectedEvaluationPoint[n] - evaluationPoint[n]), 2.0);
            }
            dist = std::sqrt(dist);
            if (dist > tol) {
                printf("bad point evaluation\n");
                ml_assert(dist < tol);
                return 1;
            }
        }
        else {
            printf("invalid param evaluation\n");
            ml_assert(0 == 1);
            return 1;
        }

        // Test geometry radius of curvature at ParamVert location
        MLREAL minRadOfCurvature, maxRadOfCurvature;
        if (!geom_kernel->evalRadiusOfCurvature(UV, entityName, 
            &minRadOfCurvature, &maxRadOfCurvature) ||
            abs(minRadOfCurvature - expectedRadiusOfCurvature) > tol) {
            printf("Error: bad radius of curvature evaluation\n");
            ml_assert(0 == 1);
            return 1;
        }

    }

    return status;
}



// Parametric interpolation at mid-point of constrained mesh edge
int interpolateEdgeMidPoint(
    MeshAssociativity &meshAssoc,
    MeshEdge *edge,
    MLVector3D expectedInterpolatedPoint
)
{
    int status = 0;
    if (!edge) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
    if (!geom_kernel) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        GeometryGroup *geom_group =
            meshAssoc.getGeometryGroupByID(edge->getGref());
        if (geom_group) {
            // original edge was associated with geometry group

            // parametric interpolation of edge mid point
            ParamVertex *const* paramVerts;
            edge->getParamVerts(&paramVerts);
            if (paramVerts[0] && paramVerts[1]) {
                // have parametric data
                MLINT gref[2];
                for (int n = 0; n < 2; ++n) {
                    gref[n] = paramVerts[n]->getGref();
                }
                if (gref[0] == gref[1]) {
                    // all vertices constrained to the same geometry entity
                    MLREAL u[2], v[2];
                    for (int n = 0; n < 2; ++n) {
                        paramVerts[n]->getUV(&(u[n]), &(v[n]));
                    }
                    MLVector2D UV = { 0.0, 0.0 };
                    UV[0] = 0.5*(u[0] + u[1]);

                    GeometryGroup *geom_group =
                        meshAssoc.getGeometryGroupByID(gref[0]);
                    const std::vector<std::string> entityNames = geom_group->getEntityNames();
                    ml_assert(1 == entityNames.size());

                    MLVector3D interpolatedPoint;
                    if (geom_kernel->evalXYZ(UV, entityNames[0], interpolatedPoint)) {
                        MLREAL dist = 0.0;
                        for (int n = 0; n < 3; ++n) {
                            dist += std::pow((expectedInterpolatedPoint[n] - interpolatedPoint[n]), 2.0);
                        }
                        dist = std::sqrt(dist);
                        if (dist > tol) {
                            printf("bad point interpolation\n");
                            ml_assert(dist < tol);
                            return 1;
                        }
                    }
                    else {
                        printf("invalid param interpolation\n");
                        ml_assert(0 == 1);
                        return 1;
                    }
                }
                else {
                    printf("invalid param verts\n");
                    ml_assert(gref[0] == gref[1]);
                    return 1;
                }
            }
            else {
                printf("missing param verts\n");
                ml_assert(paramVerts[0] && paramVerts[1]);
                return 1;
            }
        }
        else {
            printf("missing Geometry Group\n");
            ml_assert(NULL != geom_group);
            return 1;
        }
    }

    return status;
}


// Parametric interpolation at mid-point of constrained mesh face
int interpolateFaceMidPoint(
    MeshAssociativity &meshAssoc,
    MeshFace *face,
    MLVector3D expectedInterpolatedPoint
)
{
    int status = 0;
    if (!face) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
    if (!geom_kernel) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        GeometryGroup *geom_group =
            meshAssoc.getGeometryGroupByID(face->getGref());
        if (geom_group) {
            // original face was associated with geometry group
            // parametric interpolation of face mid point
            ParamVertex *const* paramVerts;
            face->getParamVerts(&paramVerts);
            if (paramVerts[0] && paramVerts[1] && paramVerts[2] && paramVerts[3]) {
                // have parametric data
                MLINT gref[4];
                for (int n = 0; n < 4; ++n) {
                    gref[n] = paramVerts[n]->getGref();
                }
                if (gref[0] == gref[1] && gref[2] == gref[3]) {
                    // all vertices constrained to the same geometry entity
                    MLREAL u[4], v[4];
                    MLVector2D UV = { 0.0, 0.0 };
                    for (int n = 0; n < 4; ++n) {
                        paramVerts[n]->getUV(&(u[n]), &(v[n]));
                        UV[0] += u[n];
                        UV[1] += v[n];
                    }
                    UV[0] *= 0.25;
                    UV[1] *= 0.25;

                    GeometryGroup *geom_group =
                        meshAssoc.getGeometryGroupByID(gref[0]);
                    const std::vector<std::string> entityNames = geom_group->getEntityNames();
                    ml_assert(1 == entityNames.size());


                    MLVector3D interpolatedPoint;
                    if (geom_kernel->evalXYZ(UV, entityNames[0], interpolatedPoint)) {

                        MLREAL dist = 0.0;
                        for (int n = 0; n < 3; ++n) {
                            dist += std::pow((expectedInterpolatedPoint[n] - interpolatedPoint[n]), 2.0);
                        }
                        dist = std::sqrt(dist);
                        if (dist > tol) {
                            printf("bad point interpolation\n");
                            ml_assert(dist < tol);
                        }
                    }
                    else {
                        printf("invalid param interpolation\n");
                        ml_assert(0 == 1);
                    }
                }
                else {
                    printf("invalid param verts\n");
                    ml_assert(gref[0] == gref[1] && gref[2] == gref[3]);
                }
            }
            else {
                printf("missing param verts\n");
                ml_assert(paramVerts[0] && paramVerts[1] && paramVerts[2] && paramVerts[3]);
            }
        }
        else {
            printf("missing Geometry Group\n");
            ml_assert(NULL != geom_group);
            return 1;
        }

    }

    return status;
}
