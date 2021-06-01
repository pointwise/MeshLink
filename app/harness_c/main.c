/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

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
#include "MeshLinkWriter_xerces_c.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define MAX_STRING_SIZE 256
#define MAX_NAMES_SIZE 24
#define MAX_ATTID_SIZE 24
#define MAX_PV_SIZE 4

/* Unit tests on the sphere_ml mesh and model in the test directory */
static int
sphere_ml_tests(MeshAssociativityObj meshAssoc);

/* Unit tests on the onera_m6 mesh and model in the test directory */
static int
oneraM6_tests(MeshAssociativityObj meshAssoc);

/* Unit tests on the drivaer mesh and model in the test directory */
static int
drivAer_tests(MeshAssociativityObj meshAssoc);

void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    memmove(s + len, s, strlen(s) + 1);
    memcpy(s, t, len);
}

#define VEC_SET(dest,v)                       \
               dest[0]=v[0]; \
               dest[1]=v[1]; \
               dest[2]=v[2]; 

#define VEC_CROSS(dest,v1,v2)                       \
               dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
               dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
               dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

static void printMeshElementLinkages(MeshAssociativityObj meshAssoc);

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

static void
getNormalizedVec(MLVector3D pt1, MLVector3D pt2, MLVector3D vec)
{
    MLREAL dist = distBetweenPoints(pt1, pt2);
    if (dist > 0.0) {
        MLREAL invDist = 1.0 / dist;
        for (int n = 0; n < 3; ++n) {
            vec[n] = (pt2[n] - pt1[n])*invDist;
        }
    }
}

/* Parametric interpolation at mid-point of constrained mesh edge */
static int
interpolateEdgeMidPoint(
    MeshAssociativityObj meshAssoc,
    MeshEdgeObj meshEdge,
    MLVector3D expectedEvaluationPoint
);

/* Parametric interpolation at mid-point of constrained mesh face */
static int
interpolateFaceMidPoint(
    MeshAssociativityObj meshAssoc,
    MeshFaceObj meshFace,
    MLVector3D expectedEvaluationPoint
);

/*===============================================================================*/
/* MAIN routine */
int main(int argc, char** argv)
{
    int ret = 0;
    if (argc < 2) {
        printf("usage: <program name> <xml file name>\n");
        return(1);
    }

    /* Name of geometry-mesh associativity file */
    const char * meshlink_fname = argv[1];
    const char * schema_fname = NULL;  /* empty schema filename causes
                                        * schemaLocation in meshlink
                                        * file to be used */

    /* Needed for writing out Xml file */
    int MeshAssocDataWritable = 1; /* true */
    char xmlns[MAX_STRING_SIZE];
    char xmlns_xsi[MAX_STRING_SIZE];
    char schemaLocation[MAX_STRING_SIZE];
    char exported_fname[MAX_STRING_SIZE];
    time_t startTime;
    MLREAL elapsedTimeF;
    MeshAssociativityObj meshAssoc;

    if (ML_STATUS_OK != ML_checkDataSizes(
        sizeof(MLINT), sizeof(MLINT32),
        sizeof(MLFLOAT), sizeof(MLREAL)) ) {
        printf("Error ML data size mismatch\n");
        return(1);
    }

    if (ML_STATUS_OK != ML_createMeshAssociativityObj(&meshAssoc)) {
        printf("Error creating Mesh Associativity Object\n");
        return(1);
    }


#if defined(HAVE_XERCES)
    /* Read Geometry-Mesh associativity */
    {
        /* Xerces MeshLink XML parser */
        MeshLinkParserObj parser;
        if (ML_STATUS_OK != ML_createMeshLinkParserXercesObj(&parser)) {
            printf("Error creating geometry-mesh associativity object\n");
            return (-1);
        }
        ML_setMeshLinkParserVerboseLevel(parser, 0);

        startTime = clock();

        //if (ML_STATUS_OK != ML_parserValidateFile(parser, meshlink_fname, schema_fname)) {
        //    /* validation error */
        //    printf("Error validating input Xml file\n");
        //    return (-1);
        //}

        elapsedTimeF = ((MLREAL)(clock() - startTime) / CLOCKS_PER_SEC);
        printf( "\nValidate elapsed CPU time: %8.2f seconds\n",
            elapsedTimeF);


        startTime = clock();
        if (ML_STATUS_OK != ML_parserReadMeshLinkFile(parser, meshlink_fname, meshAssoc)) {
            /* read error */
            printf("Error parsing input Xml file\n");
            return (-1);
        }
        elapsedTimeF = ((MLREAL)(clock() - startTime) / CLOCKS_PER_SEC);
        printf("\nReadMeshLinkFile elapsed CPU time: %8.2f seconds\n",
            elapsedTimeF);

        if (ML_STATUS_OK != ML_parserGetMeshLinkAttributes(parser, xmlns, MAX_STRING_SIZE,
                xmlns_xsi, MAX_STRING_SIZE, schemaLocation, MAX_STRING_SIZE)) {
            /* read error */
            printf("Error retrieving Xml attributes for exporting to Xml\n");
            MeshAssocDataWritable = 0; /* not writable */
        }

        ML_freeMeshLinkParserXercesObj(&parser);
    }
#else
    printf("Error parsing geometry-mesh associativity - XERCES missing\n");
    return (-1);
#endif

    printMeshElementLinkages(meshAssoc);

    if (strcmp(meshlink_fname, "sphere_ml.xml") == 0) {
        /* Test the mesh-geometry associativity in sphere_ml.xml */
        if (0 != sphere_ml_tests(meshAssoc)) {
            printf("Error testing sphere_ml.xml geometry-mesh associativity\n");
            return (-1);
        }
    }
    else if (strcmp(meshlink_fname, "om6.xml") == 0 ||
        strcmp(meshlink_fname, "om6_fun3d.xml") == 0) {
        /* Test the mesh-geometry associativity in om6.xml */
        if (0 != oneraM6_tests(meshAssoc)) {
            printf("Error testing om6.xml geometry-mesh associativity\n");
            return (-1);
        }
    }
    else if (strcmp(meshlink_fname, "grill.xml") == 0 ||
        strcmp(meshlink_fname, "drivAer.xml") == 0  ||
        strcmp(meshlink_fname, "drivAer_grill2.xml") == 0 ||
        strcmp(meshlink_fname, "drivAer_wheel.xml") == 0) {
        /* Test the mesh-geometry associativity in grill.xml */
        if (0 != drivAer_tests(meshAssoc)) {
            printf("Error testing grill.xml geometry-mesh associativity\n");
            return (-1);
        }
    }

    /* Test of MeshLink XML Writer */
#if defined(HAVE_XERCES) 
    /* Write Geometry-Mesh associativity */
    if (0 && 1 == MeshAssocDataWritable) {
        printf("\nMeshLink XML Export Test\n");

        /* Xerces MeshLink XML writer */
        MeshLinkWriterObj writer;
        if (ML_STATUS_OK != ML_createMeshLinkWriterXercesObj(&writer)) {
            printf("Error creating Mesh Writer object\n");
            return (-1);
        }
        strcpy(exported_fname, meshlink_fname);
        prepend(exported_fname, "exported_");
        if (ML_STATUS_OK != ML_writerWriteMeshLinkFile(writer, meshAssoc, xmlns, xmlns_xsi,
                schemaLocation, exported_fname)) {
            /* write error */
            printf("Error writing Mesh Associativity Object as XML\n");
            return (-1);
        }

        ML_freeMeshLinkWriterXercesObj(&writer);
        ML_freeMeshAssociativityObj(&meshAssoc);

        /* Read Geometry-Mesh associativity from exported XML file */

        /* New mesh associativity object */
        if (ML_STATUS_OK != ML_createMeshAssociativityObj(&meshAssoc)) {
            printf("Error creating Mesh Associativity Object\n");
            return(1);
        }
        /* Xerces MeshLink XML parser */
        MeshLinkParserObj parser;
        if (ML_STATUS_OK != ML_createMeshLinkParserXercesObj(&parser)) {
            printf("Error creating Mesh Parser Object\n");
        }

        if (ML_STATUS_OK != ML_parserValidateFile(parser, meshlink_fname, schema_fname)) {
            /* validation error */
            printf("Error validating exported Xml file\n");
            return (-1);
        }

        if (ML_STATUS_OK != ML_parserReadMeshLinkFile(parser, meshlink_fname, meshAssoc)) {
            /* read error */
            printf("Error parsing exported Xml file\n");
            return (-1);
        }

        ML_freeMeshLinkParserXercesObj(&parser);
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif

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
    if (ML_STATUS_OK != ML_getActiveGeometryKernel(meshAssoc, &geom_kernel)) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        MLINT gref;
        GeometryGroupObj geom_group = NULL;
        if (ML_STATUS_OK != ML_getMeshTopoGref(meshTopo,
            &gref)) {
            printf("%s: bad mesh topo info\n", msgLead);
            ML_assert(0 == 1);
            return 1;
        }

        if (ML_STATUS_OK == ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
            /* project point onto associated geometry group */

            ProjectionDataObj projectionData = NULL;
            MLVector3D projectedPt;
            MLVector2D UV;
            MLREAL projDist;
            MLREAL projTol;
            char entity_name[MAX_STRING_SIZE];
            MLREAL dist = 0.0;

            ML_createProjectionDataObj(geom_kernel, &projectionData);

            if (0 != ML_projectPoint(
                geom_kernel, geom_group, point, projectionData)) {
                printf("%s: Point projection failed\n", msgLead);
                return 1;
            }

            if (0 != ML_getProjectionInfo(geom_kernel, projectionData,
                projectedPt, UV, entity_name, MAX_STRING_SIZE, &projDist, &projTol)) {
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
                ML_assert(dist < tol);
                return 1;
            }
            
            if (fabs(dist) > tol) {
                printf("%s: bad distance calculation\n", msgLead);
                ML_assert(fabs(dist) <= tol);
                return 1;
            }

            ML_freeProjectionDataObj(&projectionData);
        }
        else {
            printf("%s: missing Geometry Group\n", msgLead);
            ML_assert(NULL != geom_group);
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
    if (ML_STATUS_OK == ML_findLowestTopoPointByInd(meshModel, bottom_con_pt_ind, &meshPoint)) {

        /* test param vertex info */
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (ML_STATUS_OK == ML_getParamVerts(meshPoint, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs == 1) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref;
            MLINT mid;
            MLVector2D UV;

            if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[0], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                gref != bottom_con_pt_gref ||
                UV[0] != bottom_con_pt_u) {
                /* didn't get the parametric data we were expecting */
                printf("Error: incorrect point parametric data\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                MLINT num_entityNames;
                GeometryGroupObj geom_group = NULL;

                if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                    printf("Error: missing Geometry Group\n");
                    ML_assert(NULL != geom_group);
                    return 1;
                }

                if (ML_STATUS_OK != ML_getEntityNames(
                    geom_group,
                    &(entityNamesArr[0][0]),
                    entityNamesArrLen,
                    MAX_STRING_SIZE,
                    &num_entityNames) ||
                    num_entityNames != 1 ||
                    0 != strcmp(entityNamesArr[0], bottom_con_pt_entity_name)) {
                    printf("Error: evaluateParamPoint - bad Geometry Group\n");
                    ML_assert(0 == 1);
                    return 1;
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ML_assert(0 == 1);
            ret = 1;
        }

        /* test projection onto associated geometry */
        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt,
            bottom_con_pt, bottom_con_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }


        /* test parametric evaluation on associated geometry */
        if (0 != evaluateParamPoint(meshAssoc, meshPoint, bottom_con_pt)) {
            printf("Error: bad point parametric evaluation\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric evaluation OK\n");
        }
    }
    else {
        printf("Error: missing constrained point\n");
        ML_assert(NULL != meshPoint);
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
    if (ML_STATUS_OK == ML_findHighestTopoPointByInd(meshModel, bottom_con_pt_ind, &meshPoint)) {

        /* test param vertex info */
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (ML_STATUS_OK == ML_getParamVerts(meshPoint, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs == 1) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref;
            MLINT mid;
            MLVector2D UV;

            if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[0], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                gref != bottom_con_pt_gref ||
                UV[0] != bottom_con_pt_u) {
                /* didn't get the parametric data we were expecting */
                printf("Error: incorrect point parametric data\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                MLINT num_entityNames;
                GeometryGroupObj geom_group = NULL;

                if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                    printf("Error:missing Geometry Group\n");
                    ML_assert(NULL != geom_group);
                    return 1;
                }

                if (ML_STATUS_OK != ML_getEntityNames(
                    geom_group,
                    &(entityNamesArr[0][0]),
                    entityNamesArrLen,
                    MAX_STRING_SIZE,
                    &num_entityNames) ||
                    num_entityNames != 1 ||
                    0 != strcmp(entityNamesArr[0], bottom_con_pt_entity_name)) {
                    printf("Error: evaluateParamPoint -bad Geometry Group\n");
                    ML_assert(0 == 1);
                    return 1;
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ML_assert(0 == 1);
            ret = 1;
        }

        /* test projection onto associated geometry */
        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt,
            bottom_con_pt, bottom_con_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }


        /* test parametric evaluation on associated geometry */
        if (0 != evaluateParamPoint(meshAssoc, meshPoint, bottom_con_pt)) {
            printf("bad point parametric evaluation\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric evaluation OK\n");
        }


    }
    else {
        printf("Error: missing constrained point\n");
        ML_assert(NULL != meshPoint);
        ret = 1;
    }
    return ret;
}


/***********************************************************
*  geomGroupsHaveCommonEntity
*  Search entities within gref geometry groups for
*  a common geometry entity.
*/
int
geomGroupsHaveCommonEntity(
    MeshAssociativityObj meshAssoc,
    MLINT gref1,
    MLINT gref2,
    MLINT *hasCommonEntity
)
{
    *hasCommonEntity = 0;
    int ret = 0;
    GeometryGroupObj containerGeomGroup;
    GeometryGroupObj targetGeomGroup;

    char container_entityNamesBufArr[16][256];  /* array of entity name buffers */
    char target_entityNamesBufArr[16][256];     /* array of entity name buffers */
    const MLINT entityNamesArrLen = 16;         /* length of names array */
    const MLINT entityNameBufLen = 256;         /* buffer length of a each name */
    MLINT num_containerEntityNames;
    MLINT num_targetEntityNames;
    MLINT iname, jname;
    int matched = 1;

    /* test that PV entity name matches one of face group */
    if (ML_STATUS_OK != ML_getGeometryGroupByID(
        meshAssoc,
        gref1,
        &containerGeomGroup)) {
        /* error */
        ret = 1;
    }
    if (ML_STATUS_OK != ML_getGeometryGroupByID(
        meshAssoc,
        gref2,
        &targetGeomGroup)) {
        /* error */
        ret = 1;
    }
    if (ML_STATUS_OK != ML_getEntityNames(
        containerGeomGroup,
        &(container_entityNamesBufArr[0][0]),
        entityNamesArrLen,
        entityNameBufLen,
        &num_containerEntityNames)) {
        /* error */
        ret = 1;
    }
    if (ML_STATUS_OK != ML_getEntityNames(
        targetGeomGroup,
        &(target_entityNamesBufArr[0][0]),
        entityNamesArrLen,
        entityNameBufLen,
        &num_targetEntityNames)) {
        /* error */
        ret = 1;
    }
    for (iname = 0; 0 != matched && iname < num_containerEntityNames; ++iname) {
        for (jname = 0; 0 != matched && jname < num_targetEntityNames; ++jname) {
            matched = strcmp(container_entityNamesBufArr[iname], target_entityNamesBufArr[jname]);
        }
    }
    if (0 != matched) {
        /* error - face and pv geom do not match */
        ret = 1;
    }
    *hasCommonEntity = (matched == 0 ? 1 : 0);
    return ret;
}


static MLREAL vectorDot(MLVector3D v1, MLVector3D v2)
{
    MLREAL dot = 0.0;
    int n;
    for (n = 0; n < 3; ++n) {
        dot += v1[n] * v2[n];
    }
    return dot;
}

static void vectorCross(MLVector3D cross, MLVector3D v1, MLVector3D v2)
{
    cross[0] = v1[1] * v2[2] - v1[2] * v2[1];
    cross[1] = v1[2] * v2[0] - v1[0] * v2[2];
    cross[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

static MLREAL vectorDistance(MLVector3D v1, MLVector3D v2)
{
    MLREAL dist = 0.0;
    int n;
    for (n = 0; n < 3; ++n) {
        dist += (v1[n] - v2[n]) * (v1[n] - v2[n]);
    }
    return sqrt(dist);
}

int
testMeshSheets(
    MeshAssociativityObj meshAssoc,
    MeshModelObj meshModel,
    MLINT expectedNumSheets
)
{
    int ret = 0;
    MLINT i, j;
    MLINT numMeshSheets;
    MLINT sizeMeshSheets = 0;
    MeshSheetObj *meshSheets = NULL;
    MeshTopoObj *meshTopos = NULL;
    MLINT numMeshTopos;
    MLINT sizeMeshTopos = 0;
    MLINT inds[4], numInds;
    MLINT sheet_gref, mid;
    MLINT refBufLen = MAX_STRING_SIZE;
    MLINT nameBufLen = MAX_STRING_SIZE;
    char refBuf[MAX_STRING_SIZE];
    char nameBuf[MAX_STRING_SIZE];
    const MLINT sizeAttIDs = MAX_ATTID_SIZE;
    MLINT attIDs[MAX_ATTID_SIZE];
    MLINT numAttIDs;
    MeshFaceObj meshFace;

    /* Test MeshSheet data */
    numMeshSheets = ML_getNumMeshSheets(meshModel);
    if (expectedNumSheets != numMeshSheets) {
        /* error */
        return 1;
    }

    sizeMeshSheets = numMeshSheets;
    meshSheets = (MeshSheetObj*)malloc(sizeof(MeshSheetObj)*sizeMeshSheets);
    if (NULL == meshSheets) {
        /* error */
        return 1;
    }
    if (ML_STATUS_OK != ML_getMeshSheets(meshModel, meshSheets, sizeMeshSheets, &numMeshSheets) ||
        expectedNumSheets != numMeshSheets) {
        /* error */
        free(meshSheets); meshSheets = NULL;
        return 1;
    }
    for (i = 0; 0 == ret && i < numMeshSheets; ++i) {
        MLINT numFaces;

        /* Sheet association info */
        if (ML_STATUS_OK != ML_getMeshTopoInfo(meshAssoc, meshSheets[i],
            refBuf, refBufLen,
            nameBuf, nameBufLen,
            &sheet_gref,
            &mid,
            attIDs,
            sizeAttIDs,
            &numAttIDs)) {
            /* error */
            ret = 1;
        }

        /* loop over faces in the sheet */
        numFaces = ML_getNumSheetMeshFaces(meshSheets[i]);

        if (sizeMeshTopos < numFaces) {
            sizeMeshTopos = numFaces;
            MeshTopoObj* tptr = (MeshTopoObj*)realloc(meshTopos, sizeof(MeshTopoObj)*sizeMeshTopos);
            if (NULL == tptr) {
                /* error */
                free(meshSheets); meshSheets = NULL;
                if (NULL != meshTopos) {
                    free(meshTopos); meshTopos = NULL;
                }
                return 1;
            }
            meshTopos = tptr;
        }

        if (ML_STATUS_OK != ML_getSheetMeshFaces(meshSheets[i], meshTopos, sizeMeshTopos, &numMeshTopos) ||
            numFaces != numMeshTopos) {
            /* error */
            ret = 1;
        }

        for (j = 0; 0 == ret && j < numFaces; ++j) {
            ML_getFaceInds(meshTopos[j], inds, &numInds);
            if (numInds == 0) {
                /* reference face - lookup by inds unavailable */
            }
            else {
                /* find face in the model (in a sheet)
                   MeshFace gref should match MeshSheet gref
                */
                if (ML_STATUS_OK != ML_findFaceByInds(meshModel, inds, numInds, &meshFace)) {
                    /* error */
                    ret = 1;
                }
                else {
                    MLINT ipv;
                    MLINT face_gref, pv_gref;
                    ParamVertexConstObj paramVerts[4];
                    MLINT sizeParamVerts = 4;
                    MLINT numpParamVerts = 0;
                    MLVector2D UV;

                    if (ML_STATUS_OK != ML_getMeshFaceInfo(meshAssoc, meshFace,
                        refBuf, refBufLen,
                        nameBuf, nameBufLen,
                        &face_gref,
                        &mid,
                        attIDs,
                        sizeAttIDs,
                        &numAttIDs,
                        paramVerts,
                        sizeParamVerts,
                        &numpParamVerts)) {
                        /* error */
                        ret = 1;
                    }
                    /* sheet gref and face gref should be the same geom group */
                    if (face_gref != sheet_gref) {
                        /* error */
                        ret = 1;
                    }

                    for (ipv = 0; ipv < numpParamVerts; ++ipv) {
                        if (ML_STATUS_OK != ML_getParamVertInfo(paramVerts[ipv],
                            refBuf, refBufLen,
                            &pv_gref, &mid, UV)) {
                            /* error */
                            ret = 1;
                        }
                        else {
                            /* face gref and pv gref should have commmon geometric entity */
                            if (face_gref != pv_gref) {
                                /* assume face is associated with multiple geometric entities */
                                /* test that PV entity name matches one of face group */
                                MLINT hasCommonEntity;
                                if (0 != geomGroupsHaveCommonEntity(
                                    meshAssoc, face_gref, pv_gref, &hasCommonEntity) ||
                                    hasCommonEntity == 0) {
                                    /* error */
                                    ret = 1;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    free(meshTopos); meshTopos = NULL;
    free(meshSheets); meshSheets = NULL;

    return ret;
}




int
testMeshStrings(
    MeshAssociativityObj meshAssoc,
    MeshModelObj meshModel,
    MLINT expectedNumStrings
)
{
    int ret = 0;
    MLINT i, j;
    MLINT numMeshStrings;
    MLINT sizeMeshStrings = 0;
    MeshStringObj *meshStrings = NULL;
    MeshTopoObj *meshTopos = NULL;
    MLINT numMeshTopos;
    MLINT sizeMeshTopos = 0;
    MLINT inds[4], numInds;
    MLINT string_gref, mid;
    MLINT refBufLen = MAX_STRING_SIZE;
    MLINT nameBufLen = MAX_STRING_SIZE;
    char refBuf[MAX_STRING_SIZE];
    char nameBuf[MAX_STRING_SIZE];
    const MLINT sizeAttIDs = MAX_ATTID_SIZE;
    MLINT attIDs[MAX_ATTID_SIZE];
    MLINT numAttIDs;
    MeshEdgeObj meshEdge;

    /* Test MeshString data */
    numMeshStrings = ML_getNumMeshStrings(meshModel);
    if (expectedNumStrings != numMeshStrings) {
        /* error */
        return 1;
    }

    sizeMeshStrings = numMeshStrings;
    meshStrings = (MeshStringObj*)malloc(sizeof(MeshStringObj)*sizeMeshStrings);
    if (NULL == meshStrings) {
        /* error */
        return 1;
    }
    if (ML_STATUS_OK != ML_getMeshStrings(meshModel, meshStrings, sizeMeshStrings, &numMeshStrings) ||
        expectedNumStrings != numMeshStrings) {
        /* error */
        free(meshStrings); meshStrings = NULL;
        return 1;
    }
    for (i = 0; 0 == ret && i < numMeshStrings; ++i) {
        MLINT numEdges;

        /* MeshString geometry association info */
        if (ML_STATUS_OK != ML_getMeshTopoInfo(meshAssoc, meshStrings[i],
            refBuf, refBufLen,
            nameBuf, nameBufLen,
            &string_gref,
            &mid,
            attIDs,
            sizeAttIDs,
            &numAttIDs)) {
            /* error */
            ret = 1;
        }

        /* loop over edges in the string */
        numEdges = ML_getNumStringMeshEdges(meshStrings[i]);

        if (sizeMeshTopos < numEdges) {
            sizeMeshTopos = numEdges;
            MeshTopoObj* tptr = (MeshTopoObj*)realloc(meshTopos, sizeof(MeshTopoObj)*sizeMeshTopos);
            if (NULL == tptr) {
                /* error */
                free(meshStrings); meshStrings = NULL;
                if (NULL != meshTopos) {
                    free(meshTopos); meshTopos = NULL;
                }
                return 1;
            }
            meshTopos = tptr;
        }

        if (ML_STATUS_OK != ML_getStringMeshEdges(meshStrings[i], meshTopos, sizeMeshTopos, &numMeshTopos) ||
            numEdges != numMeshTopos) {
            /* error */
            ret = 1;
        }

        for (j = 0; 0 == ret && j < numEdges; ++j) {
            ML_getEdgeInds(meshTopos[j], inds, &numInds);
            if (numInds == 0) {
                /* reference edge - lookup by inds unavailable */
            }
            else {
                /* find edge in lowest topo representation
                   MeshEdge gref should match MeshString gref
                */
                if (ML_STATUS_OK != ML_findLowestTopoEdgeByInds(meshModel, inds, numInds, &meshEdge)) {
                    /* error */
                    ret = 1;
                }
                else {
                    MLINT ipv;
                    MLINT edge_gref, pv_gref;
                    ParamVertexConstObj paramVerts[4];
                    MLINT sizeParamVerts = 4;
                    MLINT numpParamVerts = 0;
                    MLVector2D UV;

                    if (ML_STATUS_OK != ML_getMeshEdgeInfo(meshAssoc, meshEdge,
                        refBuf, refBufLen,
                        nameBuf, nameBufLen,
                        &edge_gref,
                        &mid,
                        attIDs,
                        sizeAttIDs,
                        &numAttIDs,
                        paramVerts,
                        sizeParamVerts,
                        &numpParamVerts)) {
                        /* error */
                        ret = 1;
                    }
                    /* string gref and edge gref should be the same geom group */
                    if (edge_gref != string_gref) {
                        /* error */
                        ret = 1;
                    }

                    for (ipv = 0; ipv < numpParamVerts; ++ipv) {
                        if (ML_STATUS_OK != ML_getParamVertInfo(paramVerts[ipv],
                            refBuf, refBufLen,
                            &pv_gref, &mid, UV)) {
                            /* error */
                            ret = 1;
                        }
                        else {
                            /* edge gref and pv gref should have commmon geometric entity */
                            if (edge_gref != pv_gref) {
                                /* assume edge is associated with multiple geometry entities */
                                /* test that PV entity name matches one of edge group */
                                MLINT hasCommonEntity;
                                if (0 != geomGroupsHaveCommonEntity(
                                    meshAssoc, edge_gref, pv_gref, &hasCommonEntity) ||
                                    hasCommonEntity == 0) {
                                    /* error */
                                    ret = 1;
                                }
                            }
                        }
                    }
                }
            }
        }

    }
    free(meshTopos); meshTopos = NULL;
    free(meshStrings); meshStrings = NULL;

    return ret;
}


/* Print MeshLinkTransform data
 *
 * A demonstration of accessing transform information
 */
void printTransform(MeshAssociativityObj meshAssoc,
    MeshLinkTransformConstObj xform)
{
    MLREAL quat[4][4];
    MLINT nameBufLen = MAX_STRING_SIZE;
    char nameBuf[MAX_STRING_SIZE];
    const MLINT sizeAttIDs = MAX_ATTID_SIZE;
    MLINT attIDs[MAX_ATTID_SIZE];
    MLINT numAttIDs;
    char attName[MAX_STRING_SIZE];
    char attValue[MAX_STRING_SIZE];

    if (NULL == xform) return;
    if (ML_STATUS_OK != ML_getTransformQuaternion(xform, quat)) {
        /* error */
        printf("printTransform: error getting quaternion\n");
        exit(1);
    }
    else {
        int i, j;

        if (ML_STATUS_OK != ML_getMeshLinkTransformInfo(
            meshAssoc, xform,
            nameBuf, nameBufLen,
            attIDs,
            sizeAttIDs,
            &numAttIDs)) {
            /* error */
            printf("printTransform: error getting info\n");
            exit(1);
        }

        printf("  Transform: %s\n", nameBuf);

        for (MLINT iAtt = 0; iAtt < numAttIDs; ++iAtt) {
            if (ML_STATUS_OK != ML_getAttribute(meshAssoc,
                attIDs[iAtt], attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)) {
                /* error */
                printf("printTransform: error getting attribute\n");
                exit(1);
            }
            else {
                printf("    Attr %" MLINT_FORMAT " %s = %s\n", iAtt + 1, attName, attValue);
            }
        }

        printf("    Quaternion\n   ");
        for (i = 0; i < 4; ++i) {
            for (j = 0; j < 4; ++j) {
                printf("%11.2e", quat[i][j]);
            }
            printf("\n   ");
        }
    }
}

/* Print all MeshElementLinkages in the database 
 * 
 * A demonstration of accessing MEL and transform information
 */
void printMeshElementLinkages(MeshAssociativityObj meshAssoc)
{
#define MAX_ELEMENT_LINKS 256
    MLINT numLinks;
    MeshElementLinkageObj linkObjs[MAX_ELEMENT_LINKS];
    MLINT sizeLinkObj = MAX_ELEMENT_LINKS;
    MLINT linkCount;
    MLINT sourceEntityRefBufLen = MAX_STRING_SIZE;
    MLINT targetEntityRefBufLen = MAX_STRING_SIZE;
    MLINT nameBufLen = MAX_STRING_SIZE;
    char sourceEntityRefBuf[MAX_STRING_SIZE];
    char targetEntityRefBuf[MAX_STRING_SIZE];
    char nameBuf[MAX_STRING_SIZE];
    const MLINT sizeAttIDs = MAX_ATTID_SIZE;
    MLINT attIDs[MAX_ATTID_SIZE];
    MLINT numAttIDs;
    char attName[MAX_STRING_SIZE];
    char attValue[MAX_STRING_SIZE];

    numLinks = ML_getNumMeshElementLinkages(meshAssoc);
    if (ML_STATUS_OK != ML_getMeshElementLinkages(
        meshAssoc, linkObjs, sizeLinkObj, &linkCount) ||
        linkCount != numLinks) {
        /* error */
        printf("printMeshElementLinkages: Error getting linkages\n");
        exit(1);
    }

    for (MLINT ilink=0; ilink < linkCount; ++ilink) {
        MeshModelObj model;
        MeshSheetObj sheet;
        MeshStringObj string;
        MLINT count;
        const char *topoStr;
        const char *entStr;

        if (ML_STATUS_OK != ML_getMeshElementLinkageInfo(
            meshAssoc, linkObjs[ilink],
            nameBuf, nameBufLen,
            sourceEntityRefBuf, sourceEntityRefBufLen,
            targetEntityRefBuf, targetEntityRefBufLen,
            attIDs,
            sizeAttIDs,
            &numAttIDs)) {
            /* error */
            continue;
        }

        printf("\nLinkage: %s\n", nameBuf);
        for (MLINT iAtt = 0; iAtt < numAttIDs; ++iAtt) {
            if (ML_STATUS_OK != ML_getAttribute(meshAssoc,
                attIDs[iAtt], attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)) {
                /* error */
                printf("printTransform: error getting attribute\n");
                exit(1);
            }
            else {
                printf("  Attr %" MLINT_FORMAT " %s = %s\n", iAtt + 1, attName, attValue);
            }
        }

        printf("  Source Entity Name: %s\n", sourceEntityRefBuf);

        if (ML_STATUS_OK == ML_getMeshSheetByName(meshAssoc, 
            sourceEntityRefBuf, &model, &sheet)) {
            count = ML_getNumSheetMeshFaces(sheet);
            // get ordered array of faces in the sheet
            //std::vector<const MeshFace*> faces = sheet->getMeshFaces();
            topoStr = "MeshSheet";
            entStr = "faces";
        }
        else if (ML_STATUS_OK == ML_getMeshStringByName(meshAssoc,
            sourceEntityRefBuf, &model, &string)) {
            count = ML_getNumStringMeshEdges(string);
            // get ordered array of edges in the string
            //std::vector<const MeshEdge *> edges = string->getMeshEdges();
            topoStr = "MeshString";
            entStr = "edges";
        }
        else {
            printf("error: missing source entity\n");
            continue;
        }
        printf("    %s with %" MLINT_FORMAT " %s\n", topoStr, count, entStr);

        printf("  Target Entity Name: %s\n", targetEntityRefBuf);
        if (ML_STATUS_OK == ML_getMeshSheetByName(meshAssoc,
            targetEntityRefBuf, &model, &sheet)) {
            count = ML_getNumSheetMeshFaces(sheet);
            // get ordered array of faces in the sheet
            //std::vector<const MeshFace*> faces = sheet->getMeshFaces();
            topoStr = "MeshSheet";
            entStr = "faces";
        }
        else if (ML_STATUS_OK == ML_getMeshStringByName(meshAssoc,
            targetEntityRefBuf, &model, &string)) {
            count = ML_getNumStringMeshEdges(string);
            // get ordered array of edges in the string
            //std::vector<const MeshEdge *> edges = string->getMeshEdges();
            topoStr = "MeshString";
            entStr = "edges";
        }
        else {
            printf("error: missing target entity\n");
            continue;
        }
        printf("    %s with %" MLINT_FORMAT " %s\n", topoStr, count, entStr);

        MeshLinkTransformConstObj xform;
        ML_getTransform(meshAssoc, linkObjs[ilink], &xform);
        printTransform(meshAssoc, xform);
    }
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
    MLREAL modelSize = 1000.0;
    MLINT expectedNumModels = 1;
    MLINT numMeshModels;
    MLINT doEdgeTests = 1;
    MLINT doFaceTests = 1;

    /* Name of mesh model */
    const char *target_block_name = "/Base/sphere";

    printf("\n=====  Sphere_ml.xml Tests  =====\n");

    if (NULL == meshAssoc) {
        return 1;
    }

    numMeshModels = ML_getNumMeshModels(meshAssoc);
    if (expectedNumModels != numMeshModels) {
        /* error */
        return 1;
    }

    /* Find the MeshModel by name */
    if (ML_STATUS_OK == ML_getMeshModelByName(meshAssoc, target_block_name, &meshModel)) {

        MLINT expectedNumSheets = 8;
        MLINT expectedNumStrings = 16;

        /* Test Mesh Sheet data */
        if (0 != testMeshSheets(meshAssoc, meshModel, expectedNumSheets)) {
            /* error */
            printf("\nMESH Sheet Test: failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("\nMESH Sheet Test: OK\n");
        }


        /* Test Mesh String data */
        if (0 != testMeshStrings(meshAssoc, meshModel, expectedNumStrings)) {
            /* error */
            printf("\nMESH String Test: failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("\nMESH String Test: OK\n");
        }


        /* Load Project Geode Kernel and set as active kernel */
        if (ML_STATUS_OK != ML_createGeometryKernelGeodeObj(&geomKernel)) {
            /* error creating geom kernel */
            return 1;
        }

        /* Geometry kernel is 'owned' by meshAssoc - must call
         * ML_removeGeometryKernel before freeing geomKernel
         */
        if ((ML_STATUS_OK != ML_addGeometryKernel(meshAssoc, geomKernel)) ||
            (ML_STATUS_OK != ML_setActiveGeometryKernelByName(meshAssoc, "Geode"))) {
            /* error setting active geom kernel */
            return 1;
        }

        /* Read geometry files */
        numGeomFiles = ML_getNumGeometryFiles(meshAssoc);


        for (iFile = 0; iFile < numGeomFiles; ++iFile) {

            if (ML_STATUS_OK != ML_getGeometryFileObj(meshAssoc, iFile, &geomFileObj)) {
                /* error */
                continue;
            }

            if (ML_STATUS_OK != ML_getFilename(geomFileObj, geom_fname, MAX_STRING_SIZE)) {
                /* error */
                continue;
            }

            printf("\nGeometryFile Attributes\n");
            if (ML_STATUS_OK != ML_getFileAttIDs(meshAssoc, geomFileObj,
                attIDs, sizeAttIDs, &numAttIDs)) {
                /* error */
                continue;
            }
            for (iAtt = 0; iAtt < numAttIDs; ++iAtt) {
                if (ML_STATUS_OK != ML_getAttribute(meshAssoc,
                    attIDs[iAtt], attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)) {
                    /* error */
                    continue;
                }
                else {
                    printf("  %" MLINT_FORMAT " %s = %s\n", iAtt + 1, attName, attValue);

                    /* Get ModelSize attribute */
                    if (strcmp("model size", attName) == 0) {
                        MLREAL value;
                        if (1 == sscanf(attValue, "%lf", &value)) {
                            modelSize = value;
                        }
                    }

                }
            }

            /* Define ModelSize prior to reading geometry */
            /* Ensures proper tolerances when building the database */
            if (ML_STATUS_OK != ML_setGeomModelSize(geomKernel, modelSize)) {
                printf("Error defining model size\n  %lf\n", modelSize);
                return (1);
            }
            {
                MLREAL value;
                if (ML_STATUS_OK != ML_getGeomModelSize(geomKernel, &value) ||
                    value != modelSize) {
                    printf("Error defining model size\n  %lf\n", modelSize);
                    return (1);
                }
            }

            if (ML_STATUS_OK != ML_readGeomFile(geomKernel, geom_fname)) {
                /* error */
                continue;
            }
        }

        if (0 != test_lowest_mesh_point(meshAssoc, meshModel)) {
            printf("MESH Point Test: test_lowest_mesh_point failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("MESH Point Test: test_lowest_mesh_point OK\n");
        }


        if (0 != test_highest_mesh_point(meshAssoc, meshModel)) {
            printf("MESH Point Test: test_highest_mesh_point failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("MESH Point Test: test_highest_mesh_point OK\n");
        }


        if (doEdgeTests) {
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
            if (ML_STATUS_OK == ML_findLowestTopoEdgeByInds(meshModel, edgeInds, 2, &meshEdge)) {

                const MLINT pvObjsArrLen = MAX_PV_SIZE;
                ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
                MLINT num_pvObjs;
                if (ML_STATUS_OK == ML_getParamVerts(meshEdge, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
                    num_pvObjs == 2) {
                    /* have parametric data */
                    char vref[MAX_STRING_SIZE];
                    MLINT gref;
                    MLINT mid;
                    MLVector2D UV;
                    MLINT ipv;
                    for (ipv = 0; ipv < num_pvObjs; ++ipv) {

                        if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                            gref != edge_gref[ipv] ||
                            UV[0] != edge_u[ipv]) {
                            /* didn't get the parametric data we were expecting */
                            printf("MESH EDGE Test: incorrect point parametric data\n");
                            ML_assert(0 == 1);
                            ret = 1;
                        }
                        else {
                            const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                            char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                            MLINT num_entityNames;
                            GeometryGroupObj geom_group = NULL;

                            if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                                printf("MESH EDGE Test:missing Geometry Group\n");
                                ML_assert(NULL != geom_group);
                                return 1;
                            }

                            if (ML_STATUS_OK != ML_getEntityNames(
                                geom_group,
                                &(entityNamesArr[0][0]),
                                entityNamesArrLen,
                                MAX_STRING_SIZE,
                                &num_entityNames) ||
                                num_entityNames != 1 ||
                                0 != strcmp(entityNamesArr[0], edge_entity_name)) {
                                printf("evaluateParamPoint:bad Geometry Group\n");
                                ML_assert(0 == 1);
                                return 1;
                            }
                        }
                    }
                }
                else {
                    printf("MESH Edge Test: incorrect edge parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }

                /* interpolate parametric coords, evaluate XYZ and compare to
                   expected value "interpolatedMidEdgePt" */
                if (0 != interpolateEdgeMidPoint(meshAssoc, meshEdge, interpolatedMidEdgePt)) {
                    printf("bad edge parametric evaluation\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Edge parametric evaluation OK\n");
                }

                /* project edge mid-point XYZ to edge geometry and compare to
                   expected value "projectedMidEdgePt" and entity "edge_entity_name" */
                for (n = 0; n < 3; ++n) {
                    point[n] = 0.5*(edgePoints[0][n] + edgePoints[1][n]);
                }
                if (0 != projectToMeshTopoGeometry(meshAssoc, meshEdge, point,
                    projectedMidEdgePt, edge_entity_name, "Mesh Edge")) {
                    printf("bad edge projection\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Edge closest point projection OK\n");
                }

            }
            else {
                printf("missing constrained point\n");
                ML_assert(NULL != meshEdge);
                ret = 1;
            }
        }

        if (doFaceTests) {
            /*===============================================================================================
             * MESH FACE Test
             * sphere bottom dom2 face points and indices
             */
            MLVector3D facePoints[4] = { -0.5, 0.33348231, 0.37254469, -0.49742707, 0.00074147824, 0.49999283,
                                -0.75349552, 0.25348322, 0.34854497 , -0.83348231, 5.5113687e-10, 0.37254469 };
            MLINT faceInds[4] = { 18, 17, 27, 26 };
            /* known-good projected and parametric interpolated mid-face XYZs */
            MLVector3D projectedMidFacePt = { -0.66267164, 0.16359104, 0.44359431 };
            const char *face_entity_name = "surface-1";
            MLVector3D interpolatedMidFacePt = { -0.64859257709701390, 0.15925496600015798, 0.45006455296541237 };
            MeshFaceObj meshFace = NULL;
            MLVector3D point;

            if (ML_STATUS_OK == ML_findFaceByInds(meshModel, faceInds, 4, &meshFace)) {
                /* face is associated with a geometry group */

                /* interpolate parametric coords, evaluate XYZ and compare to
                   expected value "interpolatedMidFacePt" */
                if (0 != interpolateFaceMidPoint(meshAssoc, meshFace, interpolatedMidFacePt)) {
                    printf("bad face parametric interpolation\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Face parametric interpolation OK\n");
                }

                /* project face mid-point XYZ to face geometry and compare to
                   expected value "projectedMidFacePt" and entity "face_entity_name" */
                for (n = 0; n < 3; ++n) {
                    point[n] = 0.25*(facePoints[0][n] +
                        facePoints[1][n] +
                        facePoints[2][n] +
                        facePoints[3][n]);
                }
                if (0 != projectToMeshTopoGeometry(meshAssoc, meshFace, point,
                    projectedMidFacePt, face_entity_name, "Mesh Face")) {
                    printf("bad face projection\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Face closest point projection OK\n");
                }
            }
            else {
                printf("missing constrained face\n");
                ML_assert(meshFace != NULL);
                ret = 1;
            }
        }

        if (ML_STATUS_OK == ML_removeGeometryKernel(meshAssoc, geomKernel)) {
            ML_freeGeometryKernelGeodeObj(&geomKernel);
        }
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

        if (ML_STATUS_OK != ML_getMeshPointInfo(meshAssoc, meshPoint,
            ref, MAX_STRING_SIZE,
            name, MAX_STRING_SIZE,
            &gref,
            &mid,
            attIDs,
            sizeAttIDs,
            &numAttIDs,
            &paramVert)) {
            printf("evaluateParamPoint: bad point info\n");
            ML_assert(0 == 1);
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

            if (ML_STATUS_OK != ML_getParamVertInfo(paramVert, vref, MAX_STRING_SIZE, &gref, &mid, UV)) {
                /* didn't get the parametric data we were expecting */
                printf("evaluateParamPoint: bad point parametric info\n");
                ML_assert(0 == 1);
                return 1;
            }

            if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                printf("evaluateParamPoint:missing Geometry Group\n");
                ML_assert(NULL != geom_group);
                return 1;
            }

            if (ML_STATUS_OK != ML_getEntityNames(
                geom_group,
                (char *)entityNames, MAX_STRING_SIZE,
                size_entityNames,
                &num_entityNames) || num_entityNames != 1) {
                printf("evaluateParamPoint:bad Geometry Group\n");
                ML_assert(0 == 1);
                return 1;
            }

            if (ML_STATUS_OK != ML_evalXYZ(
                geom_kernel,
                UV,
                entityNames[0],
                evaluationPoint)) {
                printf("evaluateParamPoint:invalid param evaluation\n");
                ML_assert(0 == 1);
                return 1;
            }

            dist = distBetweenPoints(expectedEvaluationPoint, evaluationPoint);
            if (dist > tol) {
                printf("evaluateParamPoint:bad point evaluation\n");
                ML_assert(dist < tol);
                return 1;
            }
        }
        else {
            printf("evaluateParamPoint:missing param vert\n");
            ML_assert(NULL != paramVert);
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
    if (ML_STATUS_OK != ML_getActiveGeometryKernel(meshAssoc, &geom_kernel)) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (ML_STATUS_OK == ML_getParamVerts(meshEdge, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs == 2) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref[2];
            MLINT mid[2];
            MLVector2D UV[2];
            MLINT ipv;
            for (ipv = 0; ipv < num_pvObjs; ++ipv) {

                if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv], vref, MAX_STRING_SIZE,
                    &gref[ipv], &mid[ipv], UV[ipv])) {
                    /* didn't get the parametric data we were expecting */
                    printf("interpolateEdgeMidPoint: incorrect point parametric data\n");
                    ML_assert(0 == 1);
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

                if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref[0], &geom_group)) {
                    printf("interpolateEdgeMidPoint: missing Geometry Group\n");
                    ML_assert(NULL != geom_group);
                    return 1;
                }

                if (ML_STATUS_OK != ML_getEntityNames(
                    geom_group,
                    &(entityNamesArr[0][0]),
                    entityNamesArrLen,
                    MAX_STRING_SIZE,
                    &num_entityNames) ||
                    num_entityNames != 1 ) {
                    printf("interpolateEdgeMidPoint: bad Geometry Group\n");
                    ML_assert(0 == 1);
                    return 1;
                }

                if (ML_STATUS_OK != ML_evalXYZ(
                    geom_kernel,
                    interpUV,
                    entityNamesArr[0],
                    evaluationPoint)) {
                    printf("interpolateEdgeMidPoint:invalid param evaluation\n");
                    ML_assert(0 == 1);
                    return 1;
                }

                dist = distBetweenPoints(expectedEvaluationPoint, evaluationPoint);
                if (dist > tol) {
                    printf("interpolateEdgeMidPoint:bad point evaluation\n");
                    ML_assert(dist < tol);
                    return 1;
                }
            }
            else {
                printf("interpolateEdgeMidPoint: invalid param verts\n");
                ML_assert(gref[0] == gref[1]);
                return 1;
            }
        }
        else {
            printf("interpolateEdgeMidPoint: incorrect edge parametric data\n");
            ML_assert(0 == 1);
            return 1;
        }
    }

    return status;
}



/* Parametric interpolation at mid-point of constrained mesh face */
int interpolateFaceMidPoint(
    MeshAssociativityObj meshAssoc,
    MeshFaceObj meshFace,
    MLVector3D expectedEvaluationPoint
)
{
    int status = 0;
    if (NULL == meshFace) {
        return 1;
    }
    MLREAL tol = 1e-5;
    GeometryKernelObj geom_kernel = NULL;
    if (ML_STATUS_OK != ML_getActiveGeometryKernel(meshAssoc, &geom_kernel)) {
        printf("ERROR: no active geometry kernel\n");
        return 1;
    }
    else {
        const MLINT pvObjsArrLen = MAX_PV_SIZE;
        ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
        MLINT num_pvObjs;
        if (ML_STATUS_OK == ML_getParamVerts(meshFace, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
            num_pvObjs != 0) {
            /* have parametric data */
            char vref[MAX_STRING_SIZE];
            MLINT gref[4];
            MLINT common_gref;
            MLINT mid[4];
            MLVector2D UV[4];
            MLINT ipv;
            MLINT n;
            for (ipv = 0; ipv < num_pvObjs; ++ipv) {

                if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv], vref, MAX_STRING_SIZE,
                    &gref[ipv], &mid[ipv], UV[ipv])) {
                    /* didn't get the parametric data we were expecting */
                    printf("interpolateFaceMidPoint: incorrect point parametric data\n");
                    ML_assert(0 == 1);
                    return 1;
                }
                if (ipv == 0) {
                    common_gref = gref[ipv];
                }
                else {
                    if (common_gref != gref[ipv]) {
                        printf("interpolateFaceMidPoint: invalid param verts\n");
                        ML_assert(common_gref == gref[ipv]);
                        return 1;
                    }
                }
            }

            /* all vertices constrained to the same geometry entity */
            MLVector2D interpUV;
            const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
            char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
            MLINT num_entityNames;
            GeometryGroupObj geom_group = NULL;
            MLVector3D evaluationPoint;
            MLREAL dist;

            interpUV[0] = interpUV[1] = 0.0;
            for (ipv = 0; ipv < num_pvObjs; ++ipv) {
                for (n = 0; n < 2; ++n) {
                    interpUV[n] += UV[ipv][n];
                }
            }
            for (n = 0; n < 2; ++n) {
                interpUV[n] /= num_pvObjs;
            }

            if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref[0], &geom_group)) {
                printf("interpolateFaceMidPoint: missing Geometry Group\n");
                ML_assert(NULL != geom_group);
                return 1;
            }

            if (ML_STATUS_OK != ML_getEntityNames(
                geom_group,
                &(entityNamesArr[0][0]),
                entityNamesArrLen,
                MAX_STRING_SIZE,
                &num_entityNames) ||
                num_entityNames != 1) {
                printf("interpolateFaceMidPoint: bad Geometry Group\n");
                ML_assert(0 == 1);
                return 1;
            }

            if (ML_STATUS_OK != ML_evalXYZ(
                geom_kernel,
                interpUV,
                entityNamesArr[0],
                evaluationPoint)) {
                printf("interpolateFaceMidPoint:invalid param evaluation\n");
                ML_assert(0 == 1);
                return 1;
            }

            dist = distBetweenPoints(expectedEvaluationPoint, evaluationPoint);
            if (dist > tol) {
                printf("interpolateFaceMidPoint:bad point evaluation\n");
                ML_assert(dist < tol);
                return 1;
            }
        }
        else {
            printf("interpolateFaceMidPoint: incorrect edge parametric data\n");
            ML_assert(0 == 1);
            return 1;
        }
    }


    return status;
}



/*===============================================================================
 * Test the mesh-geometry associativity in om6.xml
 */
int
oneraM6_tests(MeshAssociativityObj meshAssoc)
{
    int ret = 0;
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
    MLREAL modelSize = 1000.0;
#define MAX_GEOM_GROUP_ID_SIZE 256
    const MLINT sizeGeomGroupIDs = MAX_GEOM_GROUP_ID_SIZE;
    MLINT numGeomGroupIDs;
    MLINT geomGroupIDs[MAX_GEOM_GROUP_ID_SIZE];
    MLINT geomGroupCount;

    /* Name of mesh model */
    const char *target_block_name = "/Base/oneraM6";
    const char *target_block_name2 = "volume";

    printf("\n=====  om6.xml Tests  =====\n");
    if (NULL == meshAssoc) {
        return 1;
    }

    geomGroupCount = ML_getNumGeometryGroups(meshAssoc);
    printf("\nNumber of GeometryGroups: %" MLINT_FORMAT "\n", geomGroupCount);
    ML_getGeometryGroupIDs(meshAssoc, geomGroupIDs, sizeGeomGroupIDs, &numGeomGroupIDs);
    if (numGeomGroupIDs != geomGroupCount) {
        /* error */
        printf("\nGeometry Group Test: failed\n");
        ML_assert(0 == 1);
        ret = 1;
    }
    printf("Geometry Groups:\n");
    for (n = 0; n < numGeomGroupIDs; ++n) {
        MLINT gid = geomGroupIDs[n];
        MLINT i;

        const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
        char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
        MLINT num_entityNames;
        GeometryGroupObj geom_group = NULL;

        if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gid, &geom_group)) {
            printf("Geometry Group Test:missing Geometry Group\n");
            ML_assert(NULL != geom_group);
            return 1;
        }

        if (ML_STATUS_OK != ML_getEntityNames(
            geom_group,
            &(entityNamesArr[0][0]),
            entityNamesArrLen,
            MAX_STRING_SIZE,
            &num_entityNames) ||
            num_entityNames < 1) {
            printf("Geometry Group Test:bad Geometry Group\n");
            ML_assert(0 == 1);
            return 1;
        }
        if (num_entityNames == 1) {
            printf(" GID=%" MLINT_FORMAT "  %s\n", gid, entityNamesArr[0]);
        }
        else {
            printf(" GID=%" MLINT_FORMAT "\n", gid);
            for (i = 0; i < num_entityNames; ++i) {
                printf("     %s\n", entityNamesArr[i]);
            }
        }
    }

    



    /* Find the MeshModel by name */
    if (ML_STATUS_OK == ML_getMeshModelByName(meshAssoc, target_block_name, &meshModel) ||
        ML_STATUS_OK == ML_getMeshModelByName(meshAssoc, target_block_name2, &meshModel)) {
        MLINT expectedNumSheets = 6;
        MLINT expectedNumStrings = 9;

        /* Test Mesh Sheet data */
        if (0 != testMeshSheets(meshAssoc, meshModel, expectedNumSheets)) {
            /* error */
            printf("\nMESH Sheet Test: failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("\nMESH Sheet Test: OK\n");
        }


        /* Test Mesh String data */
        if (0 != testMeshStrings(meshAssoc, meshModel, expectedNumStrings)) {
            /* error */
            printf("\nMESH String Test: failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("\nMESH String Test: OK\n");
        }


#if defined(HAVE_GEODE)
        /* Load Project Geode Kernel and set as active kernel */
        if (ML_STATUS_OK != ML_createGeometryKernelGeodeObj(&geomKernel)) {
            /* error creating geom kernel */
            return 1;
        }

        /* Geometry kernel is 'owned' by meshAssoc - must call
         * ML_removeGeometryKernel before freeing geomKernel
         */
        if ((ML_STATUS_OK != ML_addGeometryKernel(meshAssoc, geomKernel)) ||
            (ML_STATUS_OK != ML_setActiveGeometryKernelByName(meshAssoc, "Geode"))) {
            /* error setting active geom kernel */
            return 1;
        }

        /* Read geometry files */
        numGeomFiles = ML_getNumGeometryFiles(meshAssoc);

        for (iFile = 0; iFile < numGeomFiles; ++iFile) {

            if (ML_STATUS_OK != ML_getGeometryFileObj(meshAssoc, iFile, &geomFileObj)) {
                /* error */
                continue;
            }

            if (ML_STATUS_OK != ML_getFilename(geomFileObj, geom_fname, MAX_STRING_SIZE)) {
                /* error */
                continue;
            }

            printf("\nGeometryFile Attributes\n");
            if (ML_STATUS_OK != ML_getFileAttIDs(meshAssoc, geomFileObj,
                attIDs, sizeAttIDs, &numAttIDs)) {
                /* error */
                continue;
            }
            for (iAtt = 0; iAtt < numAttIDs; ++iAtt) {
                if (ML_STATUS_OK != ML_getAttribute(meshAssoc,
                    attIDs[iAtt], attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)) {
                    /* error */
                    continue;
                }
                else {
                    printf("  %" MLINT_FORMAT " %s = %s\n", iAtt + 1, attName, attValue);

                    /* Get ModelSize attribute */
                    if (strcmp("model size", attName) == 0) {
                        MLREAL value;
                        if (1 == sscanf(attValue, "%lf", &value)) {
                            modelSize = value;
                        }
                    }

                }
            }

            /* Define ModelSize prior to reading geometry */
            /* Ensures proper tolerances when building the database */
            if (ML_STATUS_OK != ML_setGeomModelSize(geomKernel, modelSize)) {
                printf("Error defining model size\n  %lf\n", modelSize);
                return (1);
            }
            {
                MLREAL value;
                if (ML_STATUS_OK != ML_getGeomModelSize(geomKernel, &value) ||
                    value != modelSize) {
                    printf("Error defining model size\n  %lf\n", modelSize);
                    return (1);
                }
            }

            if (ML_STATUS_OK != ML_readGeomFile(geomKernel, geom_fname)) {
                /* error */
                printf("Error reading geometry file: %s\n", geom_fname);
                return (1);
            }
        }

        /*===============================================================================================
         * Curve Evaluation Test
         */
        {
        const char *curve_entity_name = "wing_tip_upper_con_3"; /* rounded LE portion */
        MLVector2D UV = { 0.5, 0.0 };
        MLVector3D expectedXYZ = { 0.854953, 1.476017, -0.006876 };
        MLREAL expectedCurvature = 64.0;
        MLVector3D expectedPrincipalNormal = { 0.707, 0.0, 0.707 };
        MLVector3D expectedTangent = { 0.707, 0.0, -0.707 };
        MLVector3D expectedBinormal = { 0.0, -1.0, 0.0 };
        MLVector3D             XYZ;     /* Evaluated location on curve */
        MLVector3D         Tangent;     /* tangent to curve  */
        MLVector3D PrincipalNormal;     /* principal normal (pointing towards the center of curvature) */
        MLVector3D        Binormal;     /* binormal (tangent x principal normal) */
            /* curvature in radians per unit length
             * ALWAYS non-negative and in the direction of the principal normal
             * Radius of curvature = 1 / Curvature
             */
        MLREAL  Curvature;
        MLINT   Linear;           /* If non-zero, the curve is linear and has no unique normal */

        if (ML_STATUS_OK == ML_evalCurvatureOnCurve(geomKernel, UV, curve_entity_name,
            XYZ, Tangent, PrincipalNormal, Binormal,
            &Curvature,
            &Linear
        )) {
            MLREAL dot, dist, relDiff;
            dist = vectorDistance(expectedXYZ, XYZ);
            if (dist > 1e-5) {
                ML_assert(0 == 1);
                ret = 1;
            }

            dot = vectorDot(expectedPrincipalNormal, PrincipalNormal);
            if (dot < 0.99) {
                ML_assert(0 == 1);
                ret = 1;
            }

            dot = vectorDot(expectedTangent, Tangent);
            if (dot < 0.99) {
                ML_assert(0 == 1);
                ret = 1;
            }

            dot = vectorDot(expectedBinormal, Binormal);
            if (dot < 0.99) {
                ML_assert(0 == 1);
                ret = 1;
            }

            relDiff = fabs(Curvature - expectedCurvature) / expectedCurvature;
            if ( relDiff > 0.01) {
                ML_assert(0 == 1);
                ret = 1;
            }

            if (0 != Linear) {
                ML_assert(0 == 1);
                ret = 1;
            }

            if (0 != ret) {
                printf("\nCurve Evaluation Test: failed\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("\nCurve Evaluation Test: OK\n");
            }
        }
        else {
            printf("\nCurve Evaluation Test: failed\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        }


        /*===============================================================================================
         * Surface Evaluation Test
         */
        {
            const char *surface_entity_name = "TrimSurf-173"; /* rounded wingtip near LE */
            MLVector2D UV = { 0.5, 0.5 };
            MLVector3D expectedXYZ = { 0.86012956, 1.4820817, -0.0061773387 };
            MLREAL expectedMinCurvature = 31.0;
            MLREAL expectedMaxCurvature = 74.0;
            MLREAL expectedAvgCurvature = (expectedMinCurvature + expectedMaxCurvature) / 2.0;
            MLREAL expectedGaussCurvature = expectedMinCurvature * expectedMaxCurvature;
            MLVector3D expectedSurfaceNormal = { 0.5, -0.707, 0.5 };
            MLVector3D expectedTangent = { 0.0876790, -0.4544817, -0.8864303 };

            MLVector3D        XYZ;              /* Evaluated location on surface */
            MLVector3D        dXYZdU;           /* First partial derivative */
            MLVector3D        dXYZdV;           /* First partial derivative */
            MLVector3D        d2XYZdU2;         /* Second partial derivative */
            MLVector3D        d2XYZdUdV;        /* Second partial derivative */
            MLVector3D        d2XYZdV2;         /* Second partial derivative */
            MLVector3D        surfaceNormal;    /* Surface normal - unit vector */
            /* Unit vector tangent to surface where curvature = min
                * surfaceNormal cross principalV yields the direction where curvature = max
                * if the surface is locally planar (min and max are 0.0) or if the
                * surface is locally spherical (min and max are equal),
                * this will be an arbitrary vector tangent to the surface
                */
            MLVector3D        principalV;
            /* Minimum and maximum curvature, in radians per unit length
                * Defined so that positive values indicate the surface bends
                * in the direction of surfaceNormal, and negative values indicate
                * the surface bends away from surfaceNormal
                */
            MLREAL          minCurvature;
            MLREAL          maxCurvature;
            /* The average or mean curvature is defined as :
                *    avg = (min + max) / 2
                * The Gaussian curvature is defined as :
                *    gauss = min * max
                */
            MLREAL          avg;               /* Average curvature */
            MLREAL          gauss;             /* Gaussian curvature */
            MLORIENT        orientation;        /* Orientation of surface in model */
            MLREAL          minTolerance;       /* minimum model assembly tolerance on the surface */
            MLREAL          maxTolerance;       /* maximum model assembly tolerance on the surface */

            if (ML_STATUS_OK == ML_evalCurvatureOnSurface(geomKernel, UV, surface_entity_name,
                XYZ, dXYZdU, dXYZdV,
                d2XYZdU2, d2XYZdUdV, d2XYZdV2,
                surfaceNormal, principalV,
                &minCurvature, &maxCurvature, &avg, &gauss,
                &orientation )) {
                MLREAL dot, dist, relDiff;
                MLVector3D cross;
                dist = vectorDistance(expectedXYZ, XYZ);
                if (dist > 1e-5) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                dot = vectorDot(expectedSurfaceNormal, surfaceNormal);
                if (dot < 0.99) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                vectorCross(cross, surfaceNormal, principalV);
                dot = vectorDot(expectedTangent, cross);
                if (dot < 0.99) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                relDiff = fabs(minCurvature - expectedMinCurvature) / expectedMinCurvature;
                if (relDiff > 0.01) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                relDiff = fabs(maxCurvature - expectedMaxCurvature) / expectedMaxCurvature;
                if (relDiff > 0.01) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                relDiff = fabs(avg - expectedAvgCurvature) / expectedAvgCurvature;
                if (relDiff > 0.01) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                relDiff = fabs(gauss - expectedGaussCurvature) / expectedGaussCurvature;
                if (relDiff > 0.01) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                if (ML_ORIENT_SAME != orientation) {
                    ML_assert(0 == 1);
                    ret = 1;
                }

                /* Evaluate model assembly tolerance */
                if (ML_STATUS_OK != ML_evalSurfaceTolerance(
                    geomKernel, surface_entity_name,
                    &minTolerance, &maxTolerance)) {
                    printf("\nSurface Model Assembly Test: failed\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    MLREAL expectedMinTol = 1e-7;
                    MLREAL expectedMaxTol = 2.0e-5;
                    if (fabs(expectedMinTol - minTolerance) / expectedMinTol > 0.1 ||
                        fabs(expectedMaxTol - maxTolerance) / expectedMaxTol > 0.1) {
                        printf("\nSurface Model Assembly Test: failed\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }
                }


                if (0 != ret) {
                    printf("\nSurface Evaluation Test: failed\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nSurface Evaluation Test: OK\n");
                }
            }
            else {
                printf("\nSurface Evaluation Test: failed\n");
                ML_assert(0 == 1);
                ret = 1;
            }
        }


        /*===============================================================================================
         * MESH EDGE Test
         * test an edge in sheet "root/dom-2"
         */
        {
            MLVector3D edgePoints[2] = { 0.9285500947461318,  1.0124483762131067, -0.0324033516787493,
                                         0.7669554599867245,  0.9182977213461775, -0.0351266055529801 };
            MLINT edgeInds[2] = { 48, 35 };

            const char *edge_entity_name = "TrimSurf-55";
            MLINT  edge_gref[2] = { 1, 1 };

            MLREAL edge_u[2] = { 0.685932280326931,  0.622145350652728 };
            MLREAL edge_v[2] = { 0.500076367091483,  0.335526084684179 };

            /* known-good projected and parametric interpolated mid-edge XYZs */
            MLVector3D projectedMidEdgePt = { 0.8477620, 0.9653808, -0.034602723353 };
            MLVector3D interpolatedMidEdgePt = { 0.848922017, 0.965373048781, -0.0345895992 };

            MLVector3D point;

            MeshEdgeObj meshEdge = NULL;
            if (ML_STATUS_OK == ML_findLowestTopoEdgeByInds(meshModel, edgeInds, 2, &meshEdge)) {

                const MLINT pvObjsArrLen = MAX_PV_SIZE;
                ParamVertexConstObj pvObjsArr[MAX_PV_SIZE];
                MLINT num_pvObjs;
                if (ML_STATUS_OK == ML_getParamVerts(meshEdge, pvObjsArr, pvObjsArrLen, &num_pvObjs) &&
                    num_pvObjs == 2) {
                    /* have parametric data */
                    char vref[MAX_STRING_SIZE];
                    MLINT gref;
                    MLINT mid;
                    MLVector2D UV;
                    MLINT ipv;
                    for (ipv = 0; ipv < num_pvObjs; ++ipv) {

                        if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv], vref, MAX_STRING_SIZE, &gref, &mid, UV) ||
                            gref != edge_gref[ipv] ||
                            UV[0] != edge_u[ipv] ||
                            UV[1] != edge_v[ipv]) {
                            /* didn't get the parametric data we were expecting */
                            printf("MESH EDGE Test: incorrect point parametric data\n");
                            ML_assert(0 == 1);
                            ret = 1;
                        }
                        else {
                            const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                            char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                            MLINT num_entityNames;
                            GeometryGroupObj geom_group = NULL;

                            if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, gref, &geom_group)) {
                                printf("MESH EDGE Test:missing Geometry Group\n");
                                ML_assert(NULL != geom_group);
                                return 1;
                            }

                            if (ML_STATUS_OK != ML_getEntityNames(
                                geom_group,
                                &(entityNamesArr[0][0]),
                                entityNamesArrLen,
                                MAX_STRING_SIZE,
                                &num_entityNames) ||
                                num_entityNames != 1 ||
                                0 != strcmp(entityNamesArr[0], edge_entity_name)) {
                                printf("evaluateParamPoint:bad Geometry Group\n");
                                ML_assert(0 == 1);
                                return 1;
                            }
                        }
                    }
                }
                else {
                    printf("MESH Edge Test: incorrect edge parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }

                if (0 != interpolateEdgeMidPoint(meshAssoc, meshEdge, interpolatedMidEdgePt)) {
                    printf("bad edge parametric evaluation\n");
                    ML_assert(0 == 1);
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
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("\nMesh Edge closest point projection OK\n");
                }

            }
            else {
                printf("missing constrained point\n");
                ML_assert(NULL != meshEdge);
                ret = 1;
            }
        }


        /*===============================================================================================
         * MESH FACE Test
         * test a face in sheet "root/dom-2"
         */
        {
        MLVector3D facePoints[3] = { 0.9285500947461318,  1.0124483762131067, -0.0324033516787493,
                                        0.7669554599867245,  0.9182977213461775, -0.0351266055529801,
                                        0.7537868742800482,  1.0468527378572527, -0.0308678001813562 };
        MLINT faceInds[3] = { 48, 35, 34 };

        const char *face_entity_name = "TrimSurf-55";
        MLINT  face_gref[3] = { 1, 1, 1 };

        MLREAL face_u[3] = { 0.685932280326931,  0.622145350652728, 0.709241184551512 };
        MLREAL face_v[3] = { 0.500076367091483,  0.335526084684179, 0.227833120699047 };

        /* known-good projected and parametric interpolated mid-edge XYZs */
        MLVector3D projectedMidFacePt = { 0.81641696167, 0.992560226574, -0.034239069084 };
        MLVector3D interpolatedMidFacePt = { 0.81616651244, 0.992532945141, -0.034237164401 };

        MLVector3D point;
        MeshFaceObj meshFace = NULL;
        if (ML_STATUS_OK == ML_findFaceByInds(meshModel, faceInds, 3, &meshFace)) {
            /* face is associated with a geometry group */

            /* interpolate parametric coords, evaluate XYZ and compare to
               expected value "interpolatedMidFacePt" */
            if (0 != interpolateFaceMidPoint(meshAssoc, meshFace, interpolatedMidFacePt)) {
                printf("bad face parametric interpolation\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("\nMesh Face parametric interpolation OK\n");
            }

            /* project face mid-point XYZ to face geometry and compare to
               expected value "projectedMidFacePt" and entity "face_entity_name" */
            for (n = 0; n < 3; ++n) {
                point[n] = (facePoints[0][n] +
                    facePoints[1][n] +
                    facePoints[2][n] ) / 3.0;
            }
            if (0 != projectToMeshTopoGeometry(meshAssoc, meshFace, point,
                projectedMidFacePt, face_entity_name, "Mesh Face")) {
                printf("bad face projection\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("\nMesh Face closest point projection OK\n");
            }
        }
        else {
            printf("missing constrained face\n");
            ML_assert(meshFace != NULL);
            ret = 1;
        }
        }


        if (ML_STATUS_OK == ML_removeGeometryKernel(meshAssoc, geomKernel)) {
            ML_freeGeometryKernelGeodeObj(&geomKernel);
        }
#else
        printf("\nMissing Geode kernel: Skipping geometry evaluation tests.\n");
#endif
    }
    return ret;
}

static const char *glyph_header =
"package require PWI_Glyph 2.18.0\n"
"\n"
"source [file join [file dirname [info script]] \"rainbow_colormap.glf\"]\n"
"\n"
"proc mkpt {xyz color {value \"\"}} {\n"
"set p [pw::Point create]\n"
"$p setRenderAttribute ColorMode Entity\n"
"$p setPoint $xyz\n"
"$p setColor $color\n"
"if {$value != \"\"} {\n"
"    $p setName [format \"p_%.3e_\" $value]\n"
"}\n"
"}\n"
"\n"
"set color [list 1 1 .75]\n"
"set scale 9.0\n"
"\n"
"proc CachePVec {x y z px py pz value} {\n"
"    global  pvecs\n"
"    lappend pvecs [list $x $y $z $px $py $pz $value]\n"
"}\n"
"proc DrawPVecs {} {\n"
"    global  pvecs\n"
"    foreach pvec $pvecs {\n"
"       foreach {x y z px py pz value} $pvec {\n"
"          DrawPVec $x $y $z $px $py $pz $value"
"       }\n"
"    }\n"
"}\n"
"proc DrawPVec {x y z px py pz value} {\n"
"    global  scale\n"
"    global minVal maxVal\n"
"    set color [getRainbowColorRGB $value $minVal $maxVal]\n"
"    set pt [list $x $y $z]\n"
"    set pvec [list $px $py $pz]\n"
"    set pt2 [pwu::Vector3 add $pt [pwu::Vector3 scale  $pvec $scale]]\n"
"    #mkpt $pt $color\n"
"    set _TMP(mode_1) [pw::Application begin Create]\n"
"      set _TMP(PW_1) [pw::SegmentSpline create]\n"
"      $_TMP(PW_1) addPoint $pt\n"
"      $_TMP(PW_1) addPoint $pt2\n"
"      set curve [pw::Curve create]\n"
"      $curve addSegment $_TMP(PW_1)\n"
"    $_TMP(mode_1) end\n"
"    unset _TMP(mode_1)\n"
"    $curve setRenderAttribute ColorMode Entity\n"
"    $curve setColor $color\n"
"    $curve setName [format \"c_%.3e_\" $value]\n"
"}\n";

void
hsv2rgb( MLREAL h, MLREAL s, MLREAL v, MLREAL *rgb ) {
    MLREAL f, p, q, t;
    if (s <= 0.0) {
        /* achromatic */
        rgb[0] = rgb[0] = rgb[0] = (int) v;
    }
    else {
        if ( h >= 1.0 ) {
            h = 0.0;
        }
        h = 6.0 * h;
        f = h - (int)h;
        p = v * (1.0 - s);
        q = v * (1.0 - (s * f));
        t = v * (1.0 - (s * (1.0 - f)));

        switch((int)h) {
        case 0:
        { rgb[0] = v; rgb[1] = t; rgb[2] = p; break; }
        case 1:
        { rgb[0] = q; rgb[1] = v; rgb[2] = p; break; }
        case 2:
        { rgb[0] = p; rgb[1] = v; rgb[2] = t; break; }
        case 3:
        { rgb[0] = p; rgb[1] = q; rgb[2] = v; break; }
        case 4:
        { rgb[0] = t; rgb[1] = p; rgb[2] = v; break; }
        case 5:
        { rgb[0] = v; rgb[1] = p; rgb[2] = q; break; }
        }
    }
}

void
calcScalarColor(MLREAL value, MLREAL minVal, MLREAL maxVal, MLREAL *color)
{
    MLREAL s, hue;
    if (value <= minVal) {
        color[0] = 0.0;
        color[1] = 0.0;
        color[2] = 1.0;
        return;
    }
    if (value >= maxVal) {
        color[0] = 1.0;
        color[1] = 0.0;
        color[2] = 0.0;
        return;
    }

    s = (value - minVal) / (maxVal - minVal);
    hue = (1.0 - s) * 2.0 / 3.0;
    hsv2rgb(hue, 1.0, 1.0, color);
}



int
MeshStringProjectTest(
    MeshAssociativityObj meshAssoc,
    GeometryKernelObj geomKernel,
    MeshModelObj meshModel,
    MLREAL *minProjTol,
    MLREAL *maxProjTol 
)
{
    int ret = 0;
    MLINT refBufLen = MAX_STRING_SIZE;
    MLINT nameBufLen = MAX_STRING_SIZE;
    char refBuf[MAX_STRING_SIZE];
    char nameBuf[MAX_STRING_SIZE];
    ProjectionDataObj projectionData = NULL;
#define MAX_PV_OBJECTS 4  /* per edge */
    const MLINT pvObjsArrLen = MAX_PV_OBJECTS;
    ParamVertexConstObj pvObjsArr[MAX_PV_OBJECTS];
    MLINT num_pvObjs, numParamVerts;
    MLINT ipv,mid,string_gref;
    MLINT pv_gref;
    MLVector2D UV;
    MeshStringObj meshString;
    MeshTopoObj meshTopo;
    GeometryGroupObj string_geom_group = NULL;
    MLINT istring;
    const MLINT sizeAttIDs = MAX_ATTID_SIZE;
    MLINT attIDs[MAX_ATTID_SIZE];
    MLINT numAttIDs;
#define MAX_MESH_EDGES 1024  /* per string */
    const MLINT sizeMeshEdges = MAX_MESH_EDGES;
    MeshEdgeObj meshEdges[MAX_MESH_EDGES];
    MLINT numEdges;
    const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
    char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
    MLINT num_entityNames;

    ML_createProjectionDataObj(geomKernel, &projectionData);


    for (istring = 0; istring < 3; ++istring) {
        meshString = NULL;
        switch (istring) {
        case 0:
            if (ML_STATUS_OK != ML_getModelMeshStringByName(meshModel,
                "root/con-131", &meshString)) {
                /* error */
                ML_assert(0 == 1);
                return 1;
            }
            break;
        case 1:
            if (ML_STATUS_OK != ML_getModelMeshStringByName(meshModel,
                "root/con-132", &meshString)) {
                /* error */
                ML_assert(0 == 1);
                return 1;
            }
            break;
        case 2:
            if (ML_STATUS_OK != ML_getModelMeshStringByName(meshModel,
                "root/con-152", &meshString)) {
                /* error */
                ML_assert(0 == 1);
                return 1;
            }
            meshTopo = meshString;
            break;
        }


        /* String association info */
        if (ML_STATUS_OK != ML_getMeshTopoInfo(meshAssoc, meshString,
            refBuf, refBufLen,
            nameBuf, nameBufLen,
            &string_gref,
            &mid,
            attIDs,
            sizeAttIDs,
            &numAttIDs)) {
            /* error */
            ret = 1;
        }
        string_geom_group = NULL;
        if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, 
            string_gref, &string_geom_group)) {
            printf("MeshString: missing Geometry Group\n");
            ML_assert(NULL != string_geom_group);
            return 1;
        }

        if (ML_STATUS_OK != ML_getStringMeshEdges(meshString, meshEdges, 
            sizeMeshEdges, &numEdges)) {
            numEdges = 0;
        }
        for (int iedge = 0; iedge < numEdges; ++iedge) {
            meshTopo = meshEdges[iedge];
            numParamVerts = ML_getNumParamVerts(meshTopo);
            ML_assert(2 == numParamVerts);
            if (numParamVerts > pvObjsArrLen) {
                /* error */
                printf("Failed to allocate ParamVertex array\n");
                ML_assert(0 == 1);
                return 1;
            }

            if (ML_STATUS_OK != ML_getParamVerts(meshTopo, pvObjsArr, 
                pvObjsArrLen, &num_pvObjs) ||
                numParamVerts != num_pvObjs) {
                /* error */
                ML_assert(0 == 1);
                ret = 1;
            }

            for (ipv = 0; ipv < numParamVerts; ++ipv) {
                if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv],
                    refBuf, refBufLen,
                    &pv_gref, &mid, UV)) {
                    /* error */
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {

                    GeometryGroupObj geom_group = NULL;

                    if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, 
                        pv_gref, &geom_group)) {
                        printf("ParamVertex:missing Geometry Group\n");
                        ML_assert(NULL != geom_group);
                        return 1;
                    }

                    if (ML_STATUS_OK != ML_getEntityNames(
                        geom_group,
                        &(entityNamesArr[0][0]),
                        entityNamesArrLen,
                        MAX_STRING_SIZE,
                        &num_entityNames) ||
                        num_entityNames != 1) {
                        printf("Error: ParamVertex - bad Geometry Group\n");
                        ML_assert(0 == 1);
                        return 1;
                    }

                    {
                        MLVector3D        XYZ;              
                        MLVector3D projectedPt;
                        char proj_entity_name[MAX_STRING_SIZE];
                        MLREAL projDist, projTol;
                        MLVector3D xyz1, xyz2;

                        /* edge end1 ParamVert eval */
                        ML_evalXYZ(geomKernel, UV, entityNamesArr[0], xyz1);

                        /* edge end2 ParamVert eval */
                        ipv++;

                        if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv],
                            refBuf, refBufLen,
                            &pv_gref, &mid, UV)) {
                            /* error */
                            ML_assert(0 == 1);
                            ret = 1;
                        }
                        else {
                            if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, pv_gref, &geom_group)) {
                                printf("ParamVertex:missing Geometry Group\n");
                                ML_assert(NULL != geom_group);
                                return 1;
                            }

                            if (ML_STATUS_OK != ML_getEntityNames(
                                geom_group,
                                &(entityNamesArr[0][0]),
                                entityNamesArrLen,
                                MAX_STRING_SIZE,
                                &num_entityNames) ||
                                num_entityNames != 1) {
                                printf("Error: ParamVertex - bad Geometry Group\n");
                                ML_assert(0 == 1);
                                return 1;
                            }
                            ML_evalXYZ(geomKernel, UV, entityNamesArr[0], xyz2);
                        }

                        /* Subdivide MeshString edges incident on a target point in the mesh */
                        MLVector3D targetPt = { 40.878921, -836.06869, 17.632411 };
                        MLREAL tol = 0.01;
                        if (distBetweenPoints(targetPt, xyz1) > tol &&
                            distBetweenPoints(targetPt, xyz2) > tol) {
                            continue;
                        }

                        /* edge is incident on targetPt 
                         * subdivide the edge and project to MeshString geometry 
                         */
                        MLINT numSubDiv = 100;
                        MLREAL edgeLen = distBetweenPoints(xyz1, xyz2);
                        MLREAL spacing = 0.00025;
                        MLREAL maxDistFromTargetPt = 0.25;
                        numSubDiv = (MLINT)(edgeLen / spacing);
                        MLREAL delta = edgeLen / numSubDiv;
                        MLVector3D edgeVec;
                        getNormalizedVec(xyz1, xyz2, edgeVec);
                        MLINT idiv;
                        for (idiv = 0; idiv <= numSubDiv; ++idiv) {
                            /* interpolate linear edge */
                            for (int n = 0; n < 3; ++n) {
                                XYZ[n] = xyz1[n] + idiv * delta * edgeVec[n];
                            }

                            if (distBetweenPoints(targetPt, XYZ) > maxDistFromTargetPt) {
                                continue;
                            }

                            /* project to MeshString's geometry */
                            if (0 != ML_projectPoint(
                                geomKernel, string_geom_group, XYZ, projectionData)) {
                                printf("Point projection failed\n");
                                ML_assert(0 == 1);
                                return 1;
                            }

                            if (0 != ML_getProjectionInfo(geomKernel, projectionData,
                                projectedPt, UV, proj_entity_name, MAX_STRING_SIZE,
                                &projDist, &projTol)) {
                                printf("Point projection failed\n");
                                ML_assert(0 == 1);
                                return 1;
                            }

                            if (projTol < *minProjTol) *minProjTol = projTol;
                            if (projTol > *maxProjTol) *maxProjTol = projTol;
                        }
                    }
                }
            } /* pv loop */
        } /* edge loop */
    } /* string loop */
    ML_freeProjectionDataObj(&projectionData);
    return ret;
}


/*===============================================================================
 * Test the mesh-geometry associativity in drivAer model
 */
int
drivAer_tests(MeshAssociativityObj meshAssoc)
{
    int ret = 0;
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
    MLREAL modelSize = 1000.0;

    /* Name of mesh model */
    const char *target_block_name = "/Base/blk-1";

    printf("\n=====  drivAer Tests  =====\n");
    if (NULL == meshAssoc) {
        return 1;
    }

    /* Find the MeshModel by name */
    if (ML_STATUS_OK == ML_getMeshModelByName(meshAssoc, target_block_name, &meshModel) ||
        ML_STATUS_OK == ML_getMeshModelByName(meshAssoc, "volume", &meshModel)) {


#if defined(HAVE_GEODE)
        /* Load Project Geode Kernel and set as active kernel */
        if (ML_STATUS_OK != ML_createGeometryKernelGeodeObj(&geomKernel)) {
            /* error creating geom kernel */
            return 1;
        }

        /* Geometry kernel is 'owned' by meshAssoc - must call
         * ML_removeGeometryKernel before freeing geomKernel
         */
        if ((ML_STATUS_OK != ML_addGeometryKernel(meshAssoc, geomKernel)) ||
            (ML_STATUS_OK != ML_setActiveGeometryKernelByName(meshAssoc, "Geode"))) {
            /* error setting active geom kernel */
            return 1;
        }

        /* Read geometry files */
        numGeomFiles = ML_getNumGeometryFiles(meshAssoc);

        for (iFile = 0; iFile < numGeomFiles; ++iFile) {

            if (ML_STATUS_OK != ML_getGeometryFileObj(meshAssoc, iFile, &geomFileObj)) {
                /* error */
                continue;
            }

            if (ML_STATUS_OK != ML_getFilename(geomFileObj, geom_fname, MAX_STRING_SIZE)) {
                /* error */
                continue;
            }

            printf("\nGeometryFile Attributes\n");
            if (ML_STATUS_OK != ML_getFileAttIDs(meshAssoc, geomFileObj,
                attIDs, sizeAttIDs, &numAttIDs)) {
                /* error */
                continue;
            }
            for (iAtt = 0; iAtt < numAttIDs; ++iAtt) {
                if (ML_STATUS_OK != ML_getAttribute(meshAssoc,
                    attIDs[iAtt], attName, MAX_STRING_SIZE, attValue, MAX_STRING_SIZE)) {
                    /* error */
                    continue;
                }
                else {
                    printf("  %" MLINT_FORMAT " %s = %s\n", iAtt + 1, attName, attValue);

                    /* Get ModelSize attribute */
                    if (strcmp("model size", attName) == 0) {
                        MLREAL value;
                        if (1 == sscanf(attValue, "%lf", &value)) {
                            modelSize = value;
                        }
                    }

                }
            }

            /* Define ModelSize prior to reading geometry */
            /* Ensures proper tolerances when building the database */
            if (ML_STATUS_OK != ML_setGeomModelSize(geomKernel, modelSize)) {
                printf("Error defining model size\n  %lf\n", modelSize);
                return (1);
            }
            {
                MLREAL value;
                if (ML_STATUS_OK != ML_getGeomModelSize(geomKernel, &value) ||
                    value != modelSize) {
                    printf("Error defining model size\n  %lf\n", modelSize);
                    return (1);
                }
            }

            if (ML_STATUS_OK != ML_readGeomFile(geomKernel, geom_fname)) {
                /* error */
                printf("Error reading geometry file: %s\n", geom_fname);
                return (1);
            }
        }

        {
            /* Test MeshSheet curvature */
            MeshSheetObj meshSheets[400];
            MLINT sizeMeshSheets = 400;
            MLINT refBufLen = MAX_STRING_SIZE;
            MLINT nameBufLen = MAX_STRING_SIZE;
            char refBuf[MAX_STRING_SIZE];
            char nameBuf[MAX_STRING_SIZE];
            MeshSheetObj meshSheet = 0;
            MLINT numMeshSheets = 0;
            MLINT sheet_gref, mid;
            const MLINT sizeAttIDs = MAX_ATTID_SIZE;
            MLINT attIDs[MAX_ATTID_SIZE];
            MLINT numAttIDs;
            MLINT isheet;
            MLREAL minCur = 1e30;
            MLREAL maxCur = 0.0;
            MLREAL minProjTol = 1.0e20;
            MLREAL maxProjTol = -1.0;
            ProjectionDataObj projectionData = NULL;
            ParamVertexConstObj pvObjsArr[4];
            const MLINT pvObjsArrLen = 4;
            MLINT num_pvObjs, numParamVerts;
            MLINT ipv;
            MLINT pv_gref;
            MLVector2D UV;
            MeshTopoObj meshTopo;
            GeometryGroupObj sheet_geom_group = NULL;

            ML_createProjectionDataObj(geomKernel, &projectionData);

            /* Test subdivision of MeshString edges */
            if (0 != MeshStringProjectTest(meshAssoc,
                geomKernel, meshModel, &minProjTol, &maxProjTol)) {
                ML_assert(0 == 1);
                return 1;
            }

            numMeshSheets = ML_getNumMeshSheets(meshModel);

            if (ML_STATUS_OK != ML_getMeshSheets(meshModel, meshSheets, sizeMeshSheets, &numMeshSheets) ) {
                /* error */
                return 1;
            }
            for (isheet = 0; isheet < numMeshSheets; ++isheet) {
                
                meshSheet = meshSheets[isheet];

                /* Sheet association info */
                if (ML_STATUS_OK != ML_getMeshTopoInfo(meshAssoc, meshSheet,
                    refBuf, refBufLen,
                    nameBuf, nameBufLen,
                    &sheet_gref,
                    &mid,
                    attIDs,
                    sizeAttIDs,
                    &numAttIDs)) {
                    /* error */
                    ret = 1;
                }
                meshTopo = meshSheet;

                sheet_geom_group = NULL;
                if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, sheet_gref, &sheet_geom_group)) {
                    printf("MeshSheet: missing Geometry Group\n");
                    ML_assert(NULL != sheet_geom_group);
                    return 1;
                }


                /* Loop over ParamVerts in the sheet
                 * Because we're getting the sheet's ParamVerts, they
                 * will reference the sheet geometry (surfaces), 
                 * even at the boundaries.
                 */
                numParamVerts = ML_getNumParamVerts(meshTopo);

                MLINT numEdges;
#define SIZE_EDGES 1024
                MeshEdgeObj meshEdges[SIZE_EDGES];
                if (ML_STATUS_OK != ML_getSheetMeshFaceEdges(meshSheet, meshEdges, SIZE_EDGES, &numEdges)) {
                    numEdges = 0;
                }
                for (int iedge=0; iedge<numEdges; ++iedge) {
                    meshTopo = meshEdges[iedge];
                    numParamVerts = ML_getNumParamVerts(meshTopo);
                    if (numParamVerts != 2) {
                        /* edge not fully associated with geometry */
                        continue;
                    }
                    if (numParamVerts > pvObjsArrLen) {
                        /* error */
                        printf("Failed to allocate ParamVertex array\n");
                        ML_assert(0 == 1);
                        return 1;
                    }

                    if (ML_STATUS_OK != ML_getParamVerts(meshTopo, pvObjsArr, pvObjsArrLen, &num_pvObjs) ||
                        numParamVerts != num_pvObjs) {
                        /* error */
                        ML_assert(0 == 1);
                        ret = 1;
                    }

                    for (ipv = 0; ipv < numParamVerts; ++ipv) {
                        if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv],
                            refBuf, refBufLen,
                            &pv_gref, &mid, UV)) {
                            /* error */
                            ML_assert(0 == 1);
                            ret = 1;
                        }
                        else {

                            const MLINT entityNamesArrLen = MAX_NAMES_SIZE;
                            char entityNamesArr[MAX_NAMES_SIZE][MAX_STRING_SIZE];
                            MLINT num_entityNames;
                            GeometryGroupObj geom_group = NULL;

                            if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, pv_gref, &geom_group)) {
                                printf("ParamVertex:missing Geometry Group\n");
                                ML_assert(NULL != geom_group);
                                return 1;
                            }

                            if (ML_STATUS_OK != ML_getEntityNames(
                                geom_group,
                                &(entityNamesArr[0][0]),
                                entityNamesArrLen,
                                MAX_STRING_SIZE,
                                &num_entityNames) ||
                                num_entityNames != 1) {
                                printf("Error: ParamVertex - bad Geometry Group\n");
                                ML_assert(0 == 1);
                                return 1;
                            }

                            /*===============================================================================================
                                * Surface Evaluation Test
                                */
                            {
                                MLVector3D        XYZ;              /* Evaluated location on surface */
                                MLVector3D        dXYZdU;           /* First partial derivative */
                                MLVector3D        dXYZdV;           /* First partial derivative */
                                MLVector3D        d2XYZdU2;         /* Second partial derivative */
                                MLVector3D        d2XYZdUdV;        /* Second partial derivative */
                                MLVector3D        d2XYZdV2;         /* Second partial derivative */
                                MLVector3D        surfaceNormal;    /* Surface normal - unit vector */
                                /* Unit vector tangent to surface where curvature = min
                                    * surfaceNormal cross principalV yields the direction where curvature = max
                                    * if the surface is locally planar (min and max are 0.0) or if the
                                    * surface is locally spherical (min and max are equal),
                                    * this will be an arbitrary vector tangent to the surface
                                    */
                                MLVector3D        principalV;
                                /* Minimum and maximum curvature, in radians per unit length
                                    * Defined so that positive values indicate the surface bends
                                    * in the direction of surfaceNormal, and negative values indicate
                                    * the surface bends away from surfaceNormal
                                    */
                                MLREAL          minCurvature;
                                MLREAL          maxCurvature;
                                /* The average or mean curvature is defined as :
                                    *    avg = (min + max) / 2
                                    * The Gaussian curvature is defined as :
                                    *    gauss = min * max
                                    */
                                MLREAL          avg;               /* Average curvature */
                                MLREAL          gauss;             /* Gaussian curvature */
                                MLORIENT        orientation;        /* Orientation of surface in model */

                                MLVector3D projectedPt;
                                char proj_entity_name[MAX_STRING_SIZE];
                                MLREAL projDist, projTol;

                                if (ML_STATUS_OK == ML_evalCurvatureOnSurface(geomKernel, UV, entityNamesArr[0],
                                    XYZ, dXYZdU, dXYZdV,
                                    d2XYZdU2, d2XYZdUdV, d2XYZdV2,
                                    surfaceNormal, principalV,
                                    &minCurvature, &maxCurvature, &avg, &gauss,
                                    &orientation)) {

                                    if (orientation != ML_ORIENT_SAME) {
                                        /* Surface orientation in the model
                                            * is opposite - flip the normal */
                                        for (int n = 0; n < 3; ++n) {
                                            surfaceNormal[n] *= -1.0;
                                        }
                                    }

                                    if (fabs(minCurvature) > fabs(maxCurvature)) {
                                        /* Surface curves away from normal vec.
                                            * Swap principal vec and curvature values
                                            * so that we capture maximum curvature
                                            * for meshing operations. */
                                        MLREAL tvec[3];
                                        MLREAL minC = minCurvature;
                                        minCurvature = -maxCurvature;
                                        maxCurvature = -minC;
                                        VEC_CROSS(tvec, surfaceNormal, principalV);
                                        VEC_SET(principalV, tvec);
                                    }

                                    if (maxCurvature < minCur) minCur = maxCurvature;
                                    if (maxCurvature > maxCur) maxCur = maxCurvature;
                                }
                                else {
                                    printf("Error: bad surface curvature eval\n");
                                    ML_assert(0 == 1);
                                    return 1;
                                }


                                if (numParamVerts == 2 && ipv == 0) {
                                    MLVector3D xyz1, xyz2;
                                    ML_evalXYZ(geomKernel, UV, entityNamesArr[0], xyz1);

                                    ipv++;

                                    if (ML_STATUS_OK != ML_getParamVertInfo(pvObjsArr[ipv],
                                        refBuf, refBufLen,
                                        &pv_gref, &mid, UV)) {
                                        /* error */
                                        ML_assert(0 == 1);
                                        ret = 1;
                                    }
                                    else {

                                        if (ML_STATUS_OK != ML_getGeometryGroupByID(meshAssoc, pv_gref, &geom_group)) {
                                            printf("ParamVertex:missing Geometry Group\n");
                                            ML_assert(NULL != geom_group);
                                            return 1;
                                        }

                                        if (ML_STATUS_OK != ML_getEntityNames(
                                            geom_group,
                                            &(entityNamesArr[0][0]),
                                            entityNamesArrLen,
                                            MAX_STRING_SIZE,
                                            &num_entityNames) ||
                                            num_entityNames != 1) {
                                            printf("Error: ParamVertex - bad Geometry Group\n");
                                            ML_assert(0 == 1);
                                            return 1;
                                        }
                                        ML_evalXYZ(geomKernel, UV, entityNamesArr[0], xyz2);
                                    }

                                    /* Subdivide MeshSheet edges incident on a target point in the mesh */
                                    MLVector3D targetPt = { 40.878921, -836.06869, 17.632411 };
                                    MLREAL tol = 0.01;
                                    if (distBetweenPoints(targetPt, xyz1) > tol &&
                                        distBetweenPoints(targetPt, xyz2) > tol) {
                                        continue;
                                    }

                                    MLINT numSubDiv = 3;
                                    MLREAL edgeLen = distBetweenPoints(xyz1, xyz2);
                                    MLREAL spacing = 0.00025;
                                    numSubDiv = (MLINT)(edgeLen / spacing);
                                    MLREAL delta = edgeLen / numSubDiv;
                                    MLREAL maxDistFromTargetPt = 0.25;
                                    MLVector3D edgeVec;
                                    getNormalizedVec(xyz1, xyz2, edgeVec);
                                    MLINT idiv;
                                    for (idiv = 0; idiv <= numSubDiv; ++idiv) {
                                        for (int n = 0; n < 3; ++n) {
                                            XYZ[n] = xyz1[n] + idiv * delta * edgeVec[n];
                                        }

                                        if (distBetweenPoints(targetPt, XYZ) > maxDistFromTargetPt) {
                                            continue;
                                        }

                                        /* project to MeshSheet's geometry */
                                        if (0 != ML_projectPoint(
                                            geomKernel, sheet_geom_group, XYZ, projectionData)) {
                                            printf("Point projection failed\n");
                                            ML_assert(0 == 1);
                                            return 1;
                                        }

                                        if (0 != ML_getProjectionInfo(geomKernel, projectionData,
                                            projectedPt, UV, proj_entity_name, MAX_STRING_SIZE,
                                            &projDist, &projTol)) {
                                            printf("Point projection failed\n");
                                            ML_assert(0 == 1);
                                            return 1;
                                        }

                                        if (projTol < minProjTol) minProjTol = projTol;
                                        if (projTol > maxProjTol) maxProjTol = projTol;
                                    }


                                }
                                else {
                                    ML_evalXYZ(geomKernel, UV, entityNamesArr[0], XYZ);

                                    if (0 != ML_projectPoint(
                                        geomKernel, geom_group, XYZ, projectionData)) {
                                        printf("Point projection failed\n");
                                        ML_assert(0 == 1);
                                        return 1;
                                    }

                                    if (0 != ML_getProjectionInfo(geomKernel, projectionData,
                                        projectedPt, UV, proj_entity_name, MAX_STRING_SIZE,
                                        &projDist, &projTol)) {
                                        printf("Point projection failed\n");
                                        ML_assert(0 == 1);
                                        return 1;
                                    }

                                    if (projTol < minProjTol) minProjTol = projTol;
                                    if (projTol > maxProjTol) maxProjTol = projTol;
                                }

                            }
                        }
                    } /* pv loop */
                }
            } /* sheet loop */
            ML_freeProjectionDataObj(&projectionData);
            printf( "Minimum curvature %16.9e\n", minCur);
            printf( "Maxumum curvature %16.9e\n", maxCur);
            printf( "Min Projection Tol %16.9e\n", minProjTol);
            printf( "Max Projection Tol %16.9e\n", maxProjTol);

        }


        if (ML_STATUS_OK == ML_removeGeometryKernel(meshAssoc, geomKernel)) {
            ML_freeGeometryKernelGeodeObj(&geomKernel);
        }
#else
        printf("\nMissing Geode kernel: Skipping geometry evaluation tests.\n");
#endif
    }
    return ret;
}

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
