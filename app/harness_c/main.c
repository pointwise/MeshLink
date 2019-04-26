/***********************************************************************
**  MeshLink Library Test Harness (C)
**
**  Read MeshLink file.
**     + print diagnostic info to console
**
**  If Project Geode geometry kernel present:
**     + read NMB geometry file
**     + test various computational geometry access functions
**/

#undef BUILD_MESHLINK
#include "Types.h"
#if defined(HAVE_GEODE)
#include "GeomKernel_Geode_c.h"
#endif
#include "MeshAssociativity_c.h"
#if defined(HAVE_XERCES)
#include "MeshLinkParser_xerces_c.h"
#endif

#include <stdio.h>
#include <string.h>
#include <math.h>

#define MAX_STRING_SIZE 256
#define MAX_NAMES_SIZE 24
#define MAX_ATTID_SIZE 24
#define MAX_PV_SIZE 4

static int
sphere_ml_tests(MeshAssociativityObj meshAssoc);

/* Parametric interpolation of constrained mesh point */
static int 
evaluateParamPoint(
    MeshAssociativityObj meshAssoc,
    MeshPointObj meshPoint,
    MLVector3D expectedEvaluationPoint
);

/* Closest point projection onto geometry of constrained mesh topo */
static int projectToMeshTopoGeometry(
        MeshAssociativityObj meshAssoc,
        MeshTopoObj meshTopo,
        MLVector3D point,
        MLVector3D expectedProjectedPoint,
        const char *expectedProjectionEntityName,
        const char *msgLead);

static MLREAL
distBetweenPoints(MLVector3D pt1, MLVector3D pt2)
{
    MLREAL dist = 0.0;
    int n;
    for (n = 0; n < 3; ++n) {
        dist += (pt1[n] - pt2[n]) * (pt1[n] - pt2[n]);
    }
    dist = sqrt(dist);
    return dist;
}

/* Parametric interpolation at mid-point of constrained mesh edge */
static int 
interpolateEdgeMidPoint(
    MeshAssociativityObj meshAssoc,
    MeshEdgeObj meshEdge,
    MLVector3D expectedEvaluationPoint
);


#if defined(SPHERE_TEST)

static int interpolateEdgeMidPoint(
    MeshAssociativity &meshAssoc,
    MeshEdge *edge,
    MLVector3D expectedInterpolatedPoint);

// Closest point projection onto geometry of constrained mesh face
static int projectToFaceGeometry(
    MeshAssociativity &meshAssoc,
    MeshFace *face,
    MLVector3D point,
    MLVector3D expectedProjectedPoint);

// Parametric interpolation at mid-point of constrained mesh face
static int interpolateFaceMidPoint(
    MeshAssociativity &meshAssoc,
    MeshFace *face,
    MLVector3D expectedInterpolatedPoint);
#endif

/*===============================================================================*/
/* MAIN routine */
int main(int argc, char** argv)
{
    int ret = 0;
    if (argc != 2) {
        printf("usage: <program name> <xml file name>\n");
        return(1);
    }

    /* Name of geometry-mesh associativity file */
    const char * meshlink_fname = argv[1];
    const char * schema_fname = NULL;  /* empty schema filename causes schemaLocation in meshlink file to be used */

    MeshAssociativityObj meshAssoc;
    if (0 != ML_createMeshAssociativityObj(&meshAssoc)) {
        printf("Error creating Mesh Associativity Object\n");
        return(1);
    }


#if defined(HAVE_XERCES)
    /* Read Geometry-Mesh associativity */
    {
        /* Xerces MeshLink XML parser */
        MeshLinkParserObj parser;
        if (0 != ML_createMeshLinkParserXercesObj(&parser)) {
        }

        if (0 != ML_parserValidateFile(parser, meshlink_fname, schema_fname)) {
            /* validation error */
        }

        if (0 != ML_parserReadMeshLinkFile(parser, meshlink_fname, meshAssoc)) {
            /* read error */

        }

        ML_freeMeshLinkParserXercesObj(&parser);
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif

    if (strcmp(meshlink_fname,"sphere_ml.xml") == 0) {
        /* Test the mesh-geometry associativity in sphere_ml.xml */
        if (0 != sphere_ml_tests(meshAssoc)) {
            printf("Error testing sphere_ml.xml geometry-mesh associativity\n");
            return (-1);
        }
    }

    ML_freeMeshAssociativityObj(&meshAssoc);

    return ret;
}


/* Closest point projection onto geometry of constrained mesh topo */
int projectToMeshTopoGeometry(
    MeshAssociativityObj meshAssoc,
    MeshTopoObj meshTopo,
    MLVector3D point,
    MLVector3D expectedProjectedPoint,
    const char *expectedProjectionEntityName,
    const char *msgLead
)
{
    int status = 0;
    if (!meshTopo) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernelObj geom_kernel = NULL;
    if (0 != ML_getActiveGeometryKernel(meshAssoc, &geom_kernel)) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        MLINT gref;
        GeometryGroupObj geom_group = NULL;
        if (0 != ML_getMeshTopoGref(meshTopo,
            &gref)) {
            printf("%s: bad mesh topo info\n", msgLead);
            ml_assert(0 == 1);
            return 1;
        }

        if (0 == ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
            /* project point onto associated geometry group */

            ProjectionDataObj projectionData = NULL;
            MLVector3D projectedPt;
            MLVector2D UV;
            char entity_name[MAX_STRING_SIZE];
            MLREAL dist = 0.0;

            ML_createProjectionDataObj(geom_kernel, &projectionData);

            if (0 != ML_projectPoint(
                geom_kernel, geom_group, point, projectionData)) {
                printf("%s: Point projection failed\n", msgLead);
                return 1;
            }

            if (0 != ML_getProjectionInfo(geom_kernel, projectionData,
                projectedPt, UV, entity_name, MAX_STRING_SIZE)) {
                printf("%s: Point projection failed\n", msgLead);
                return 1;
            }

            if (0 != strcmp(expectedProjectionEntityName, entity_name)) {
                printf("%s: bad projection entity\n", msgLead);
                return 1;
            }

            dist = distBetweenPoints(expectedProjectedPoint, projectedPt);
            if (dist > tol) {
                printf("%s: bad point projection\n", msgLead);
                ml_assert(dist < tol);
                return 1;
            }

            ML_freeProjectionDataObj(&projectionData);
        }
        else {
            printf("%s: missing Geometry Group\n", msgLead);
            ml_assert(NULL != geom_group);
            return 1;
        }
    }

    return status;
}

/*===============================================================================================
* MESH POINT Test (lowest topological match for point index )
*/
int test_lowest_mesh_point(
    MeshAssociativityObj meshAssoc,
    MeshModelObj meshModel
)
{
    int ret = 0;

    /* Test data for MeshPoint defined at MeshString level */
    MLVector3D bottom_con_pt = { -0.49742707, 0.00074147824, 0.49999283 };
    MLINT  bottom_con_pt_ind = 17;
    MLINT  bottom_con_pt_gref = 15;
    MLREAL bottom_con_pt_u = 0.625156631213186;
    MLREAL bottom_con_pt_radius = 0.501719;
    const char *bottom_con_pt_entity_name = "bottom_con_1";
    MeshPointObj meshPoint = NULL;

    printf("\nMESH POINT Test (lowest topological match for point index )\n");

    /* Find the point at the lowest topological level (MeshString, MeshSheet, or MeshModel) */
    if (0 == ML_findLowestTopoPointByInd(meshModel, bottom_con_pt_ind, &meshPoint)) {

        /* test param vertex info */
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (0 == ML_getParamVerts(meshPoint, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs == 1) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref;
            MLINT mid;
            MLVector2D UV;

            if (0 != ML_getParamVertInfo(pvObjsArr[0], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                gref != bottom_con_pt_gref ||
                UV[0] != bottom_con_pt_u) {
                /* didn't get the parametric data we were expecting */
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                MLINT num_entityNames;
                GeometryGroupObj geom_group = NULL;

                if (0 != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                    printf("Error: missing Geometry Group\n");
                    ml_assert(NULL != geom_group);
                    return 1;
                }

                if (0 != ML_getEntityNames(
                    geom_group,
                    &(entityNamesArr[0][0]),
                    entityNamesArrLen,
                    MAX_STRING_SIZE,
                    &num_entityNames) ||
                    num_entityNames != 1 ||
                    0 != strcmp(entityNamesArr[0], bottom_con_pt_entity_name)) {
                    printf("Error: evaluateParamPoint - bad Geometry Group\n");
                    ml_assert(0 == 1);
                    return 1;
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ml_assert(0 == 1);
            ret = 1;
        }

        /* test projection onto associated geometry */
        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt,
            bottom_con_pt, bottom_con_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }


        /* test parametric evaluation on associated geometry */
        if (0 != evaluateParamPoint(meshAssoc, meshPoint, bottom_con_pt)) {
            printf("Error: bad point parametric evaluation\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric evaluation OK\n");
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
* MESH POINT Test (lowest topological match for point index )
*/
int test_highest_mesh_point(
    MeshAssociativityObj meshAssoc,
    MeshModelObj meshModel
)
{
    int ret = 0;
    /* Test data for MeshPoint at MeshModel level */
    MLVector3D bottom_con_pt = { -0.47677290802217431, 0.29913675338094192, 0.39997213024780004 };
    MLINT bottom_con_pt_ind = 17;
    MLINT bottom_con_pt_gref = 15;
    MLREAL bottom_con_pt_u = 0.1234;
    const char *bottom_con_pt_entity_name = "bottom_con_1";
    MeshPointObj meshPoint = NULL;

    printf("\nMESH POINT Test (highest topological match for point index )\n");

    /* Find the point at the highest topological level (MeshModel) */
    if (0 == ML_findHighestTopoPointByInd(meshModel, bottom_con_pt_ind, &meshPoint)) {

        /* test param vertex info */
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (0 == ML_getParamVerts(meshPoint, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs == 1) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref;
            MLINT mid;
            MLVector2D UV;

            if (0 != ML_getParamVertInfo(pvObjsArr[0], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                gref != bottom_con_pt_gref ||
                UV[0] != bottom_con_pt_u) {
                /* didn't get the parametric data we were expecting */
                printf("Error: incorrect point parametric data\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                MLINT num_entityNames;
                GeometryGroupObj geom_group = NULL;

                if (0 != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                    printf("Error:missing Geometry Group\n");
                    ml_assert(NULL != geom_group);
                    return 1;
                }

                if (0 != ML_getEntityNames(
                    geom_group,
                    &(entityNamesArr[0][0]),
                    entityNamesArrLen,
                    MAX_STRING_SIZE,
                    &num_entityNames) ||
                    num_entityNames != 1 ||
                    0 != strcmp(entityNamesArr[0], bottom_con_pt_entity_name)) {
                    printf("Error: evaluateParamPoint -bad Geometry Group\n");
                    ml_assert(0 == 1);
                    return 1;
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ml_assert(0 == 1);
            ret = 1;
        }

        /* test projection onto associated geometry */
        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt,
            bottom_con_pt, bottom_con_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }


        /* test parametric evaluation on associated geometry */
        if (0 != evaluateParamPoint(meshAssoc, meshPoint, bottom_con_pt)) {
            printf("bad point parametric evaluation\n");
            ml_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric evaluation OK\n");
        }


    }
    else {
        printf("Error: missing constrained point\n");
        ml_assert(NULL != meshPoint);
        ret = 1;
    }
    return ret;
}


/*===============================================================================
 * Test the mesh-geometry associativity in sphere_ml.xml
 */
int
sphere_ml_tests(MeshAssociativityObj meshAssoc)
{
    int ret = 0;
#if defined(HAVE_GEODE)
    int n;
    MLINT iFile;
    MLINT numGeomFiles;
    MeshLinkFileConstObj geomFileObj;
    char geom_fname[MAX_STRING_SIZE];

    const MLINT sizeAttIDs = MAX_ATTID_SIZE;
    MLINT attIDs[MAX_ATTID_SIZE];
    MLINT numAttIDs;
    char attName[MAX_STRING_SIZE];
    char attValue[MAX_STRING_SIZE];
    MLINT iAtt;
    /* Load Project Geode Kernel and set as active kernel */
    GeometryKernelObj geomKernel = NULL;
    MeshModelObj meshModel;

    /* Name of mesh model */
    const char *target_block_name = "/Base/sphere";


    printf("\nSphere_ml.xml Tests\n");
    if (NULL == meshAssoc) {
        return 1;
    }

    /* Find the MeshModel by name */
    if (0 == ML_getMeshModelByName(meshAssoc, target_block_name, &meshModel)) {

        /* Load Project Geode Kernel and set as active kernel */
        if (0 != ML_createGeometryKernelGeodeObj(&geomKernel)) {
            /* error creating geom kernel */
            return 1;
        }

        if ((0 != ML_addGeometryKernel(meshAssoc, geomKernel)) ||
            (0 != ML_setActiveGeometryKernelByName(meshAssoc, "Geode")) ) {
            /* error setting active geom kernel */
            return 1;
        }
            
        /* Read geometry files */
        numGeomFiles = ML_getNumGeometryFiles(meshAssoc);


        for (iFile = 0; iFile < numGeomFiles; ++iFile) {

            if (0 != ML_getGeometryFileObj(meshAssoc, iFile, &geomFileObj)) {
                /* error */
                continue;
            }

            if (0 != ML_getFilename(geomFileObj, geom_fname, MAX_STRING_SIZE)) {
                /* error */
                continue;
            }

            printf("\nGeometryFile Attributes\n");
            if (0 != ML_getFileAttIDs(meshAssoc, geomFileObj,
                attIDs, sizeAttIDs, &numAttIDs)) {
                /* error */
                continue;
            }
            for (iAtt = 0; iAtt < numAttIDs; ++iAtt) {
                if (0 != ML_getAttribute(meshAssoc,
                    attIDs[iAtt], attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)) {
                    /* error */
                    continue;
                }
                else {
                    printf("  %" MLINT_FORMAT " %s = %s\n", iAtt+1, attName, attValue);
                }
            }

            if (0 != ML_readGeomFile(geomKernel, geom_fname)) {
                /* error */
                continue;
            }
        }

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




        if (10) {
            /*===============================================================================================
             * MESH EDGE Test
             * sphere bottom con edge points and indices
             */
            MLVector3D edgePoints[2] = { -0.5, 0.33348231, 0.37254469, -0.49742707, 0.00074147824, 0.49999283 };
            MLINT edgeInds[2] = { 18, 17 };
            /* known-good projected and parametric interpolated mid-edge XYZs */
            MLVector3D projectedMidEdgePt = { -0.45307208568368834, 0.17708402010909388, 0.46523007284560064 };
            MLVector3D interpolatedMidEdgePt = { -0.45617166, 0.23260459, 0.440425 };
            MLINT edge_gref[2] = { 15, 15 };
            MLREAL edge_u[2] = { 0.0, 0.625156631213186 };
            const char *edge_entity_name = "bottom_con_1";
            MLVector3D point;

            MeshEdgeObj meshEdge = NULL;
            if (0 == ML_findLowestTopoEdgeByInds(meshModel, edgeInds, 2, &meshEdge)) {

                const MLINT pvObjsArrLen = MAX_PV_SIZE;
                ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
                MLINT num_pvObjs;
                if (0 == ML_getParamVerts(meshEdge, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
                    num_pvObjs == 2) {
                    /* have parametric data */
                    char vref[MAX_STRING_SIZE];
                    MLINT gref;
                    MLINT mid;
                    MLVector2D UV;
                    MLINT ipv;
                    for (ipv = 0; ipv < num_pvObjs; ++ipv) {

                        if (0 != ML_getParamVertInfo(pvObjsArr[ipv], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                            gref != edge_gref[ipv] ||
                            UV[0] != edge_u[ipv]) {
                            /* didn't get the parametric data we were expecting */
                            printf("MESH EDGE Test: incorrect point parametric data\n");
                            ml_assert(0 == 1);
                            ret = 1;
                        }
                        else {
                            const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                            char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                            MLINT num_entityNames;
                            GeometryGroupObj geom_group = NULL;

                            if (0 != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                                printf("MESH EDGE Test:missing Geometry Group\n");
                                ml_assert(NULL != geom_group);
                                return 1;
                            }

                            if (0 != ML_getEntityNames(
                                geom_group,
                                &(entityNamesArr[0][0]),
                                entityNamesArrLen,
                                MAX_STRING_SIZE,
                                &num_entityNames) ||
                                num_entityNames != 1 ||
                                0 != strcmp(entityNamesArr[0], edge_entity_name)) {
                                printf("evaluateParamPoint:bad Geometry Group\n");
                                ml_assert(0 == 1);
                                return 1;
                            }
                        }
                    }
                }
                else {
                    printf("MESH Edge Test: incorrect edge parametric data\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }


                if (0 != interpolateEdgeMidPoint(meshAssoc, meshEdge, interpolatedMidEdgePt)) {
                    printf("bad edge parametric evaluation\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Edge parametric evaluation OK\n");
                }

                for (n = 0; n < 3; ++n) {
                    point[n] = 0.5*(edgePoints[0][n] + edgePoints[1][n]);
                }
                if (0 != projectToMeshTopoGeometry(meshAssoc, meshEdge, point,
                    projectedMidEdgePt, edge_entity_name, "Mesh Edge")) {
                    printf("bad edge projection\n");
                    ml_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Edge closest point projection OK\n");
                }

            }
            else {
                printf("missing constrained point\n");
                ml_assert(NULL != meshEdge);
                ret = 1;
            }
        }
#if defined(FACE_TEST)
        //===============================================================================================
        // MESH FACE Test
        // sphere bottom dom2 face points and indices
        MLVector3D facePoints[4] = { -0.5, 0.33348231, 0.37254469, -0.49742707, 0.00074147824, 0.49999283,
                            -0.75349552, 0.25348322, 0.34854497 , -0.83348231, 5.5113687e-10, 0.37254469 };
        MLINT faceInds[4] = { 18, 17, 27, 26 };
        // known-good projected and parametric interpolated mid-face XYZs
        MLVector3D projectedMidFacePt = { -0.66267164, 0.16359104, 0.44359431 };
        MLVector3D interpolatedMidFacePt = { -0.64859257709701390, 0.15925496600015798, 0.45006455296541237 };

        MeshFace *face =
            meshModel->findFaceByInds(faceInds[0], faceInds[1], faceInds[2], faceInds[3]);
        if (face) {
            // original face was associated with geometry group
            // project interpolated point onto geometry group
            for (n = 0; n < 3; ++n) {
                point[n] = 0.25*(facePoints[0][n] +
                    facePoints[1][n] +
                    facePoints[2][n] +
                    facePoints[3][n]);
            }

            if (0 != projectToFaceGeometry(meshAssoc, face, point, projectedMidFacePt)) {
                printf("bad face point projection\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("\nMesh Face closest point projection OK\n");
            }

            if (0 != interpolateFaceMidPoint(meshAssoc, face, interpolatedMidFacePt)) {
                printf("bad face parametric interpolation\n");
                ml_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("\nMesh Face parametric interpolation OK\n");
            }
        }
        else {
            printf("missing constrained face\n");
            ml_assert(face);
            ret = 1;
        }
#endif


        ML_freeMeshLinkParserXercesObj(&geomKernel);
    }
#endif
    return ret;
}



/* Parametric evaluation of point */
int evaluateParamPoint(
    MeshAssociativityObj meshAssoc,
    MeshPointObj meshPoint,
    MLVector3D expectedEvaluationPoint
)
{
    int status = 0;
    if (!meshPoint) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernelObj geom_kernel = NULL;
    if (0 != ML_getActiveGeometryKernel(meshAssoc, &geom_kernel)) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        char ref[MAX_STRING_SIZE];
        char name[MAX_STRING_SIZE];
        MLINT gref;
        MLINT mid;
        const MLINT sizeAttIDs = MAX_ATTID_SIZE;
        MLINT attIDs[MAX_ATTID_SIZE];
        MLINT numAttIDs;
        ParamVertexConstObj paramVert;
        const MLINT size_entityNames = MAX_NAMES_SIZE;
        char entityNames[MAX_NAMES_SIZE][MAX_STRING_SIZE];
        MLINT num_entityNames;
        MLVector3D evaluationPoint;

        if (0 != ML_getMeshPointInfo(meshAssoc, meshPoint,
            ref, MAX_STRING_SIZE,
            name, MAX_STRING_SIZE,
            &gref,
            &mid,
            attIDs,
            sizeAttIDs,
            &numAttIDs,
            &paramVert)) {
            printf("evaluateParamPoint: bad point info\n");
            ml_assert(0 == 1);
            return 1;
        }

        /* parametric interpolation of edge mid point */
        if (paramVert) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref;
            MLINT mid;
            MLVector2D UV;
            MLREAL dist = 0.0;
            GeometryGroupObj geom_group = NULL;

            if (0 != ML_getParamVertInfo(paramVert, vref, MAX_STRING_SIZE, &gref, &mid, UV)) {
                /* didn't get the parametric data we were expecting */
                printf("evaluateParamPoint: bad point parametric info\n");
                ml_assert(0 == 1);
                return 1;
            }

            if (0 != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                printf("evaluateParamPoint:missing Geometry Group\n");
                ml_assert(NULL != geom_group);
                return 1;
            }

            if (0 != ML_getEntityNames(
                geom_group,
                (char *)entityNames, MAX_STRING_SIZE,
                size_entityNames,
                &num_entityNames) || num_entityNames != 1) {
                printf("evaluateParamPoint:bad Geometry Group\n");
                ml_assert(0 == 1);
                return 1;
            }

            if (0 != ML_evalXYZ(
                geom_kernel,
                UV,
                entityNames[0],
                evaluationPoint)) {
                printf("evaluateParamPoint:invalid param evaluation\n");
                ml_assert(0 == 1);
                return 1;
            }

            dist = distBetweenPoints(expectedEvaluationPoint, evaluationPoint);
            if (dist > tol) {
                printf("evaluateParamPoint:bad point evaluation\n");
                ml_assert(dist < tol);
                return 1;
            }
        }
        else {
            printf("evaluateParamPoint:missing param vert\n");
            ml_assert(NULL != paramVert);
            return 1;
        }
    }

    return status;
}



/* Parametric interpolation at mid-point of constrained mesh edge */
int interpolateEdgeMidPoint(
    MeshAssociativityObj meshAssoc,
    MeshEdgeObj meshEdge,
    MLVector3D expectedEvaluationPoint
)
{
    int status = 0;
    if (!meshEdge) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernelObj geom_kernel = NULL;
    if (0 != ML_getActiveGeometryKernel(meshAssoc, &geom_kernel)) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (0 == ML_getParamVerts(meshEdge, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs == 2) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref[2];
            MLINT mid[2];
            MLVector2D UV[2];
            MLINT ipv;
            for (ipv = 0; ipv < num_pvObjs; ++ipv) {

                if (0 != ML_getParamVertInfo(pvObjsArr[ipv], vref, MAX_STRING_SIZE,
                    &gref[ipv], &mid[ipv], UV[ipv])) {
                    /* didn't get the parametric data we were expecting */
                    printf("interpolateEdgeMidPoint: incorrect point parametric data\n");
                    ml_assert(0 == 1);
                    return 1;
                }
            }

            if (gref[0] == gref[1]) {
                /* all vertices constrained to the same geometry entity */
                MLVector2D interpUV;
                const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                MLINT num_entityNames;
                GeometryGroupObj geom_group = NULL;
                MLVector3D evaluationPoint;
                MLREAL dist;

                interpUV[0] = 0.5*(UV[0][0] + UV[1][0]);
                interpUV[1] = 0.5*(UV[0][1] + UV[1][1]);

                if (0 != ML_getGeometryGroupByID(meshAssoc, gref[0], &geom_group)) {
                    printf("interpolateEdgeMidPoint: missing Geometry Group\n");
                    ml_assert(NULL != geom_group);
                    return 1;
                }

                if (0 != ML_getEntityNames(
                    geom_group,
                    &(entityNamesArr[0][0]),
                    entityNamesArrLen,
                    MAX_STRING_SIZE,
                    &num_entityNames) ||
                    num_entityNames != 1 ) {
                    printf("interpolateEdgeMidPoint: bad Geometry Group\n");
                    ml_assert(0 == 1);
                    return 1;
                }

                if (0 != ML_evalXYZ(
                    geom_kernel,
                    interpUV,
                    entityNamesArr[0],
                    evaluationPoint)) {
                    printf("interpolateEdgeMidPoint:invalid param evaluation\n");
                    ml_assert(0 == 1);
                    return 1;
                }

                dist = distBetweenPoints(expectedEvaluationPoint, evaluationPoint);
                if (dist > tol) {
                    printf("interpolateEdgeMidPoint:bad point evaluation\n");
                    ml_assert(dist < tol);
                    return 1;
                }
            }
            else {
                printf("interpolateEdgeMidPoint: invalid param verts\n");
                ml_assert(gref[0] == gref[1]);
                return 1;
            }
        }
        else {
            printf("interpolateEdgeMidPoint: incorrect edge parametric data\n");
            ml_assert(0 == 1);
            return 1;
        }
    }

    return status;
}




#if defined(SPHERE_TEST_FUNCS)


// Closest point projection onto geometry of constrained mesh face
int projectToFaceGeometry(
    MeshAssociativity &meshAssoc,
    MeshFace *face,
    MLVector3D point,
    MLVector3D expectedProjectedPoint

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
        ProjectionData projectionData(geom_kernel);

        GeometryGroup *geom_group =
            meshAssoc.getGeometryGroupByID(face->getGref());
        if (geom_group) {
            // face associated with geometry group
            // project point onto geometry group
            if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                printf("Point projection failed\n");
            }
            else {
                MLVector3D projectedPt = { 0.0, 0.0,0.0 };
                geom_kernel->getProjectionXYZ(projectionData, projectedPt);
                MLREAL dist = 0.0;
                for (n = 0; n < 3; ++n) {
                    dist += std::pow((expectedProjectedPoint[n] - projectedPt[n]), 2.0);
                }
                dist = std::sqrt(dist);
                if (dist > tol) {
                    printf("bad point projection\n");
                    ml_assert(dist < tol);
                }

            }
        }
        else {
            printf("missing Geometry Group\n");
            ml_assert(geom_group);
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
                for (n = 0; n < 4; ++n) {
                    gref[n] = paramVerts[n]->getGref();
                }
                if (gref[0] == gref[1] && gref[2] == gref[3]) {
                    // all vertices constrained to the same geometry entity
                    MLREAL u[4], v[4];
                    MLVector2D UV = { 0.0, 0.0 };
                    for (n = 0; n < 4; ++n) {
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
                    if (geom_kernel->getXYZ(UV, entityNames[0], interpolatedPoint)) {

                        MLREAL dist = 0.0;
                        for (n = 0; n < 3; ++n) {
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
            ml_assert(geom_group);
            return 1;
        }

    }

    return status;
}
#endif
