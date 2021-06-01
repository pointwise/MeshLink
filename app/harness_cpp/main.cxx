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
**  MeshLink Library Test Harness (C++)
**
**  Read MeshLink file.
**     + print diagnostic info to console
**
**  If Project Geode geometry kernel present:
**     + read NMB geometry file
**     + test various computational geometry access functions
**/

#include <Types.h>

#if defined(HAVE_GEODE)
#include "GeomKernel_Geode.h"
#endif

#include "MeshAssociativity.h"

#if defined(HAVE_XERCES)
#include "MeshLinkParser_xerces.h"
#include "MeshLinkWriter_xerces.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>


// Test the mesh-geometry associativity in sphere_ml.xml
static int sphere_ml_tests(MeshAssociativity &meshAssoc);

// Test the mesh-geometry associativity in hemi_cyl.xml
static int hemi_cyl_tests(MeshAssociativity &meshAssoc);

// Test the mesh-geometry associativity in om6.xml
static int oneraM6_tests(MeshAssociativity &meshAssoc);

// Print MeshElementLinkage data
static void printMeshElementLinkages(MeshAssociativity &meshAssoc);

// Closest point projection onto geometry of constrained meshTopo entity
static int projectToMeshTopoGeometry(
    MeshAssociativity &meshAssoc,
    MeshTopo *meshTopo,
    MLVector3D point,
    MLVector3D expectedProjectedPoint,
    const char *expectedProjectionEntityName,
    const char *msgLead
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
    if (argc < 2) {
        printf("usage: <program name> <xml file name>\n");
        ::exit(1);
    }

    // Name of geometry-mesh associativity file
    std::string meshlink_fname(argv[1]);
    std::string schema_fname;  // empty schema filename causes schemaLocation in meshlink file to be used

    // Needed for writing out Xml file
    std::string xmlns;
    std::string xmlns_xsi;
    std::string schemaLocation;

    MeshAssociativity *meshAssoc = new MeshAssociativity();

#if defined(HAVE_XERCES)
    // Read Geometry-Mesh associativity
    {
        // Xerces MeshLink XML parser
        MeshLinkParserXerces parser;

        // Validate first
        parser.validate(meshlink_fname, schema_fname);

        printf("\nParsing %s...\n\n", meshlink_fname.c_str());
        if (!parser.parseMeshLinkFile(meshlink_fname, meshAssoc)) {
            printf("Error parsing geometry-mesh associativity\n");
            return (-1);
        }
        parser.getMeshLinkAttributes(xmlns, xmlns_xsi, schemaLocation);
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif

    printMeshElementLinkages(*meshAssoc);

#if defined(HAVE_GEODE)
    if (meshlink_fname.compare("sphere_ml.xml") == 0) {
        // Test the mesh-geometry associativity in sphere_ml.xml
        if (0 != sphere_ml_tests(*meshAssoc)) {
            printf("Error testing sphere_ml.xml geometry-mesh associativity\n");
            return (-1);
        }
    }
    else if (meshlink_fname.compare("hemi_cyl.xml") == 0) {
        // Test the mesh-geometry associativity in hemi_cyl.xml
        if (0 != hemi_cyl_tests(*meshAssoc)) {
            printf("Error testing hemi_cyl.xml geometry-mesh associativity\n");
            return (-1);
        }
    }
    else if (meshlink_fname.compare("om6.xml") == 0) {
        // Test the mesh-geometry associativity in om6.xml
        if (0 != oneraM6_tests(*meshAssoc)) {
            printf("Error testing om6.xml geometry-mesh associativity\n");
            return (-1);
        }
    }
#endif

#if defined(HAVE_XERCES)
    // Write Geometry-Mesh associativity
    {
        // Xerces MeshLink XML writer
        MeshLinkWriterXerces writer;

        // Write
        writer.setMeshLinkAttributes(xmlns, xmlns_xsi, schemaLocation);

        meshlink_fname = std::string("exported_") + meshlink_fname;
        printf("\nWriting %s...\n\n", meshlink_fname.c_str());
        // true: Base64 encoding
        if (!writer.writeMeshLinkFile(meshlink_fname, meshAssoc, true)) {
            printf("Error writing geometry-mesh associativity\n");
            return (-1);
        }
    }
    // Read Geometry-Mesh associativity
    {
        // New MeshAssociativity
        if (nullptr != meshAssoc) {
            delete meshAssoc;
        }
        meshAssoc = new MeshAssociativity();
        // Xerces MeshLink XML parser
        MeshLinkParserXerces parser;

        // Validate first
        parser.validate(meshlink_fname, schema_fname);

        printf("\nParsing %s...\n\n", meshlink_fname.c_str());
        if (!parser.parseMeshLinkFile(meshlink_fname, meshAssoc)) {
            printf("Error parsing geometry-mesh associativity\n");
            return (-1);
        }
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif
    delete meshAssoc;
    return ret;
}



/***********************************************************
*  geomGroupsHaveCommonEntity
*  Search entities within gref geometry groups for
*  a common geometry entity.
*/
int
geomGroupsHaveCommonEntity(
    MeshAssociativity &meshAssoc,
    MLINT gref1,
    MLINT gref2,
    MLINT *hasCommonEntity
)
{
    *hasCommonEntity = 0;
    int ret = 0;
    GeometryGroup *containerGeomGroup;
    GeometryGroup *targetGeomGroup;
    MLINT num_container_entityNames;
    MLINT num_target_entityNames;

    MLINT iname, jname;
    int matched = 1;

    /* test that PV entity name matches one of face group */
    containerGeomGroup = meshAssoc.getGeometryGroupByID(gref1);
    if (NULL == containerGeomGroup) {
        /* error */
        ret = 1;
    }
    targetGeomGroup = meshAssoc.getGeometryGroupByID(gref2);
    if (NULL == targetGeomGroup) {
        /* error */
        ret = 1;
    }
    const std::vector<std::string> container_entityNames = containerGeomGroup->getEntityNames();
    num_container_entityNames = (MLINT)container_entityNames.size();
    if (0 == num_container_entityNames) {
        /* error */
        ret = 1;
    }
    const std::vector<std::string> target_entityNames = targetGeomGroup->getEntityNames();
    num_target_entityNames = (MLINT)target_entityNames.size();
    if (0 == num_target_entityNames) {
        /* error */
        ret = 1;
    }
    for (iname = 0; 0 != matched && iname < num_container_entityNames; ++iname) {
        for (jname = 0; 0 != matched && jname < num_target_entityNames; ++jname) {
            matched = container_entityNames[iname].compare(target_entityNames[jname]);
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
    MeshAssociativity &meshAssoc,
    MeshModel *meshModel,
    MLINT expectedNumSheets
)
{
    int ret = 0;
    MLINT i, j;
    MLINT numMeshSheets;
    MLINT sizeMeshSheets = 0;
    MLINT inds[4], numInds;
    MLINT sheet_gref;
    MeshFace *meshFace;

    /* Test MeshSheet data */
    numMeshSheets = meshModel->getMeshSheetCount();
    if (expectedNumSheets != numMeshSheets) {
        /* error */
        return 1;
    }

    std::vector<MeshSheet *> meshSheets;
    meshModel->getMeshSheets(meshSheets);
    sizeMeshSheets = (MLINT) meshSheets.size();
    if (sizeMeshSheets != numMeshSheets) {
        /* error */
        return 1;
    }
    for (i = 0; 0 == ret && i < numMeshSheets; ++i) {
        MLINT numFaces;

        /* Sheet association info */
        sheet_gref = meshSheets[i]->getGref();

        /* loop over faces in the sheet */
        std::vector<const MeshFace *> meshFaces;
        meshSheets[i]->getMeshFaces(meshFaces);
        numFaces = meshSheets[i]->getNumFaces();
        if ((MLINT)meshFaces.size() != numFaces) {
            /* error */
            return 1;
        }

        for (j = 0; 0 == ret && j < numFaces; ++j) {
            meshFaces[j]->getInds(inds, &numInds);
            if (numInds == 0) {
                /* reference face - lookup by inds unavailable */
            }
            else {
                /* find face in the model (in a sheet)
                   MeshFace gref should match MeshSheet gref
                */
                meshFace = meshModel->findFaceByInds(inds[0], inds[1], inds[2], inds[3]);
                if (NULL == meshFace) {
                    /* error */
                    ret = 1;
                }
                else {
                    MLINT ipv;
                    MLINT face_gref, pv_gref;
                    MLINT numParamVerts = 0;

                    /* sheet gref and face gref should be the same geom group */
                    face_gref = meshFace->getGref();

                    if (face_gref != sheet_gref) {
                        /* error */
                        ret = 1;
                    }

                    std::vector<ParamVertex *> paramVerts;
                    meshFace->getParamVerts(paramVerts);
                    numParamVerts = meshFace->getNumParamVerts();

                    for (ipv = 0; ipv < numParamVerts; ++ipv) {
                        pv_gref = paramVerts[ipv]->getGref();
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

    return ret;
}




int
testMeshStrings(
    MeshAssociativity &meshAssoc,
    MeshModel *meshModel,
    MLINT expectedNumStrings
)
{
    int ret = 0;
    MLINT i, j;
    MLINT numMeshStrings;
    MLINT sizeMeshStrings = 0;
    MLINT inds[4], numInds;
    MLINT string_gref;
    MeshEdge *meshEdge;

    /* Test MeshString data */
    numMeshStrings = (MLINT)meshModel->getMeshStringCount();
    if (expectedNumStrings != numMeshStrings) {
        /* error */
        return 1;
    }

    std::vector<MeshString *> meshStrings;
    meshModel->getMeshStrings(meshStrings);
    sizeMeshStrings = (MLINT)meshStrings.size();
    if (sizeMeshStrings != numMeshStrings) {
        /* error */
        return 1;
    }
    for (i = 0; 0 == ret && i < numMeshStrings; ++i) {
        MLINT numEdges;

        /* MeshString geometry association info */
        string_gref = meshStrings[i]->getGref();

        /* loop over edges in the string */
        std::vector<const MeshEdge *> meshEdges;
        meshStrings[i]->getMeshEdges(meshEdges);
        numEdges = (MLINT)meshEdges.size();

        for (j = 0; 0 == ret && j < numEdges; ++j) {
            meshEdges[j]->getInds(inds, &numInds);
            if (numInds == 0) {
                /* reference edge - lookup by inds unavailable */
            }
            else {
                /* find edge in lowest topo representation
                   MeshEdge gref should match MeshString gref
                */
                meshEdge = meshModel->findLowestTopoEdgeByInds(inds[0], inds[1]);
                if (NULL == meshEdge) {
                    /* error */
                    ret = 1;
                }
                else {
                    MLINT ipv;
                    MLINT edge_gref, pv_gref;
                    MLINT numpParamVerts = 0;

                    /* string gref and edge gref should be the same geom group */
                    edge_gref = meshEdge->getGref();

                    if (edge_gref != string_gref) {
                        /* error */
                        ret = 1;
                    }
                    std::vector<ParamVertex *> paramVerts;
                    meshEdge->getParamVerts(paramVerts);
                    numpParamVerts = paramVerts.size();

                    for (ipv = 0; ipv < numpParamVerts; ++ipv) {
                        pv_gref = paramVerts[ipv]->getGref();
                        /* edge gref and pv gref should have commmon geometric entity */
                        if (edge_gref != pv_gref) {
                            /* assume edge is associated with multiple geometric entities */
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
                ML_assert(0 == 1);
                ret = 1;
            }

            GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
            if (NULL == geomGroup) {
                printf("Error: incorrect point parametric data\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                if (entityNames.size() != 1 ||
                    entityNames[0].compare(bottom_con_pt_entity_name) != 0) {
                    printf("Error: incorrect point parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }

                if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(), 
                    bottom_con_pt, bottom_con_pt_radius)) {
                    printf("Error: bad point parametric evaluation\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  parametric evaluation OK\n");
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ML_assert(0 == 1);
            ret = 1;
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt, bottom_con_pt,
            bottom_con_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }

    }
    else {
        printf("Error: missing constrained point\n");
        ML_assert(NULL != meshPoint);
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
                ML_assert(0 == 1);
                ret = 1;
            }

            GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
            if (NULL == geomGroup) {
                printf("Error: incorrect point parametric data\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                if (entityNames.size() != 1 ||
                    entityNames[0].compare(dom4_pt_entity_name) != 0) {
                    printf("Error: incorrect point parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }

                if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                    dom4_pt, dom4_pt_radius)) {
                    printf("Error: bad point parametric evaluation\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  parametric evaluation OK\n");
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ML_assert(0 == 1);
            ret = 1;
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, dom4_pt, dom4_pt,
            dom4_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
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
                ML_assert(0 == 1);
                ret = 1;
            }

            GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
            if (NULL == geomGroup) {
                printf("Error: incorrect point parametric data\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                if (entityNames.size() != 1 ||
                    entityNames[0].compare(bottom_con_pt_entity_name) != 0) {
                    printf("Error: incorrect point parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }

                if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                    bottom_con_pt, bottom_con_pt_radius)) {
                    printf("Error: bad point parametric evaluation\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  parametric evaluation OK\n");
                }
            }
        }
        else {
            printf("Error: incorrect point parametric data\n");
            ML_assert(0 == 1);
            ret = 1;
        }

        if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, bottom_con_pt, bottom_con_pt,
            bottom_con_pt_entity_name, "Mesh Point")) {
            printf("Error: bad point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
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
                    ML_assert(0 == 1);
                    ret = 1;
                }

                GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
                if (NULL == geomGroup) {
                    printf("Error: incorrect point parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                    if (entityNames.size() != 1 ||
                        entityNames[0].compare(edge_entity_name) != 0) {
                        printf("Error: incorrect point parametric data\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }

                    if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                        edgePoints[i], edge_radius[i])) {
                        printf("Error: bad point parametric evaluation\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }
                    else {
                        printf("  parametric evaluation OK\n");
                    }
                }
            }
            else {
                printf("Error: incorrect point parametric data\n");
                ML_assert(0 == 1);
                ret = 1;
            }
        }


        // original edge was associated with geometry group
        // project interpolated point onto geometry group
        for (int n = 0; n < 3; ++n) {
            point[n] = 0.5*(edgePoints[0][n] + edgePoints[1][n]);
        }
        if (0 != projectToMeshTopoGeometry(meshAssoc, edge, point, projectedMidEdgePt,
            edge_entity_name, "Mesh Edge")) {
            printf("bad edge point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }

        if (0 != interpolateEdgeMidPoint(meshAssoc, edge, interpolatedMidEdgePt)) {
            printf("Error: bad edge parametric interpolation\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric interpolation OK\n");
        }
    }
    else {
        printf("Error: missing constrained edge\n");
        ML_assert(NULL != edge);
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
    const char *face_entity_name = "surface-1";
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

        if (0 != projectToMeshTopoGeometry(meshAssoc, face, point, projectedMidFacePt,
            face_entity_name, "Mesh Face")) {
            printf("Error: bad face point projection\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  closest point projection OK\n");
        }

        if (0 != interpolateFaceMidPoint(meshAssoc, face, interpolatedMidFacePt)) {
            printf("Error: bad face parametric interpolation\n");
            ML_assert(0 == 1);
            ret = 1;
        }
        else {
            printf("  parametric interpolation OK\n");
        }
    }
    else {
        printf("Error: missing constrained face\n");
        ML_assert(NULL != face);
        ret = 1;
    }
    return ret;
}



//===============================================================================
// Test the mesh-geometry associativity in hemi_cyl.xml
int
hemi_cyl_tests(MeshAssociativity &meshAssoc)
{
    int ret = 0;
#if defined(HAVE_GEODE)

    printf("\nhemi_cyl.xml Tests\n");

    // Name of mesh model
    const char *target_block_name = "/Base/blk-1";
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
            MLREAL modelSize = 1000.0;

            printf("\nGeometryFile Attributes\n");
            std::vector<MLINT> attIDs = geomFile.getAttributeIDs(meshAssoc);
            MLINT numAtts = attIDs.size();
            MLINT iAtt;
            for (iAtt = 0; iAtt < numAtts; ++iAtt) {
                const char *attName, *attValue;
                if (meshAssoc.getAttribute(attIDs[iAtt], &attName, &attValue)) {
                    printf("  %" MLINT_FORMAT " %s = %s\n", iAtt, attName, attValue);

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
            geomKernel.setModelSize(modelSize);
            if (geomKernel.getModelSize() != modelSize) {
                printf("Error defining model size\n  %lf\n", modelSize);
                return (-1);
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

            /* Test a point which we know to be defined in the "dom-6" MeshSheet level,
             * but not at the MeshString level
             */
            printf(" MeshPoint defined in MeshSheet name = \"root/dom-6\"\n");
            /* Test data for MeshPoint defined in MeshSheet name="root/dom-6" */
            MLVector3D dom6_pt = { 1.28269, -1.76547, 0.0 };
            MLINT dom6_pt_ind = 322;
            MLINT dom6_pt_gref = 1;
            MLREAL dom6_pt_UV[2] = { 1.28268969952326, -1.76547095480407 };
            const char *dom6_pt_entity_name = "plane-1";
            const MLREAL dom6_pt_radius = 1.0e9;

            /* Find the point at the lowest topological level (MeshString, MeshSheet, or MeshModel) */
            MeshPoint *meshPoint = meshModel->findLowestTopoPointByInd(dom6_pt_ind);
            if (meshPoint) {
                ParamVertex *const paramVert = meshPoint->getParamVert();
                if (paramVert) {
                    // have parametric data
                    MLINT gref;
                    gref = paramVert->getGref();
                    MLVector2D UV;
                    paramVert->getUV(&(UV[0]), &(UV[1]));
                    if (gref != dom6_pt_gref ||
                        UV[0] != dom6_pt_UV[0] ||
                        UV[1] != dom6_pt_UV[1]
                        ) {
                        printf("Error: incorrect point parametric data\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }

                    GeometryGroup *geomGroup = meshAssoc.getGeometryGroupByID(gref);
                    if (NULL == geomGroup) {
                        printf("Error: incorrect point parametric data\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }
                    else {
                        const std::vector<std::string> entityNames = geomGroup->getEntityNames();
                        if (entityNames.size() != 1 ||
                            entityNames[0].compare(dom6_pt_entity_name) != 0) {
                            printf("Error: incorrect point parametric data\n");
                            ML_assert(0 == 1);
                            ret = 1;
                        }

                        if (0 != evaluateParamPoint(meshAssoc, UV, entityNames[0].c_str(),
                            dom6_pt, dom6_pt_radius)) {
                            printf("Error: bad point parametric evaluation\n");
                            ML_assert(0 == 1);
                            ret = 1;
                        }
                        else {
                            printf("  parametric evaluation OK\n");
                        }
                    }
                }
                else {
                    printf("Error: incorrect point parametric data\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }

                if (0 != projectToMeshTopoGeometry(meshAssoc, meshPoint, dom6_pt, dom6_pt,
                    dom6_pt_entity_name, "Mesh Point")) {
                    printf("Error: bad point projection\n");
                    ML_assert(0 == 1);
                    ret = 1;
                }
                else {
                    printf("  closest point projection OK\n");
                }
            }
            else {
                printf("Error: missing constrained point\n");
                ML_assert(NULL != meshPoint);
                ret = 1;
            }

        }
    }
    else {
        printf("missing Mesh Model\n");
        ML_assert(NULL != meshModel);
        ret = 1;
    }
#endif
    return ret;
}

void printTransformQuaternion(const MeshLinkTransform *xform)
{
    MLREAL quat[4][4];
    xform->getQuaternion(quat);
    int i, j;
    printf("  Transform:\n   ");
    for (i = 0; i < 4; ++i) {
        for (j = 0; j < 4; ++j) {
            printf("%11.2e", quat[i][j]);
        }
        printf("\n   ");
    }

}

void printMeshElementLinkages(MeshAssociativity &meshAssoc)
{
    std::vector<MeshElementLinkage *> links;
    meshAssoc.getMeshElementLinkages(links);
    for (auto link : links) {
        const std::string &name = link->getName();
        printf("\nLinkage: %s\n", name.c_str());

        std::string sourceEntityRef;
        std::string targetEntityRef;
        link->getEntityRefs(sourceEntityRef, targetEntityRef);
        printf("  Source Entity Name: %s\n", sourceEntityRef.c_str());

        MeshModel *model;
        MeshSheet *sheet;
        MeshString *string;
        MLINT count;
        const char *topoStr;
        const char *entStr;
        if (meshAssoc.getMeshSheetByName(sourceEntityRef, &model, &sheet)) {
            count = sheet->getNumFaces();
            // get ordered array of faces in the sheet
            //std::vector<const MeshFace*> faces = sheet->getMeshFaces();
            topoStr = "MeshSheet";
            entStr = "faces";
        }
        else if (meshAssoc.getMeshStringByName(sourceEntityRef, &model, &string)) {
            count = string->getNumEdges();
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

        printf("  Target Entity Name: %s\n", targetEntityRef.c_str());
        if (meshAssoc.getMeshSheetByName(targetEntityRef, &model, &sheet)) {
            count = sheet->getNumFaces();
            // get ordered array of faces in the sheet
            //std::vector<const MeshFace*> faces = sheet->getMeshFaces();
            topoStr = "MeshSheet";
            entStr = "faces";
        }
        else if (meshAssoc.getMeshStringByName(targetEntityRef, &model, &string)) {
            count = string->getNumEdges();
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

        printTransformQuaternion(link->getTransform(meshAssoc));
    }
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


            if (0 != test_mesh_edge(meshAssoc, meshModel)) {
                printf("MESH Edge Test: test_mesh_edge failed\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("MESH Edge Test: test_mesh_edge OK\n");
            }


            if (0 != test_mesh_face(meshAssoc, meshModel)) {
                printf("MESH Face Test: test_mesh_face failed\n");
                ML_assert(0 == 1);
                ret = 1;
            }
            else {
                printf("MESH Face Test: test_mesh_face OK\n");
            }

        }
    }
    else {
        printf("missing Mesh Model\n");
        ML_assert(NULL != meshModel);
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
                printf("%s: Point projection failed\n", msgLead);
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
                    printf("%s: bad projection\n", msgLead);
                    ML_assert(dist < tol);
                    return 1;
                }

                std::string entity_name;
                geom_kernel->getProjectionEntityName(projectionData, entity_name);
                if (0 != strcmp(expectedProjectionEntityName, entity_name.c_str())) {
                    printf("%s: bad projection entity\n", msgLead);
                    return 1;
                }

            }
        }
        else {
            printf("missing Geometry Group\n");
            ML_assert(NULL != geom_group);
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
                ML_assert(dist < tol);
                return 1;
            }
        }
        else {
            printf("invalid param evaluation\n");
            ML_assert(0 == 1);
            return 1;
        }

        // Test geometry radius of curvature at ParamVert location
        MLREAL minRadOfCurvature, maxRadOfCurvature;
        if (!geom_kernel->evalRadiusOfCurvature(UV, entityName, 
            &minRadOfCurvature, &maxRadOfCurvature) ||
            std::abs(minRadOfCurvature - expectedRadiusOfCurvature) > tol) {
            printf("Error: bad radius of curvature evaluation\n");
            ML_assert(0 == 1);
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
                    MLVector2D interpUV;
                    for (int n = 0; n < 2; ++n) {
                        paramVerts[n]->getUV(&(u[n]), &(v[n]));
                    }

                    interpUV[0] = 0.5*(u[0] + u[1]);
                    interpUV[1] = 0.5*(v[0] + v[1]);


                    GeometryGroup *geom_group =
                        meshAssoc.getGeometryGroupByID(gref[0]);
                    const std::vector<std::string> entityNames = geom_group->getEntityNames();
                    ML_assert(1 == entityNames.size());

                    MLVector3D interpolatedPoint;
                    if (geom_kernel->evalXYZ(interpUV, entityNames[0], interpolatedPoint)) {
                        MLREAL dist = 0.0;
                        for (int n = 0; n < 3; ++n) {
                            dist += std::pow((expectedInterpolatedPoint[n] - interpolatedPoint[n]), 2.0);
                        }
                        dist = std::sqrt(dist);
                        if (dist > tol) {
                            printf("bad point interpolation\n");
                            ML_assert(dist < tol);
                            return 1;
                        }
                    }
                    else {
                        printf("invalid param interpolation\n");
                        ML_assert(0 == 1);
                        return 1;
                    }
                }
                else {
                    printf("invalid param verts\n");
                    ML_assert(gref[0] == gref[1]);
                    return 1;
                }
            }
            else {
                printf("missing param verts\n");
                ML_assert(paramVerts[0] && paramVerts[1]);
                return 1;
            }
        }
        else {
            printf("missing Geometry Group\n");
            ML_assert(NULL != geom_group);
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
            MLINT numPV = face->getNumParamVerts();
            if (numPV > 0) {
                // have parametric data
                MLINT gref[4];
                MLINT common_gref;
                bool haveCommonGref = true;
                for (int n = 0; n < numPV; ++n) {
                    gref[n] = paramVerts[n]->getGref();
                    if (n == 0) {
                        common_gref = gref[n];
                    }
                    else {
                        if (common_gref != gref[n]) {
                            haveCommonGref = false;
                        }
                    }
                }
                if (haveCommonGref) {
                    // all vertices constrained to the same geometry entity
                    MLREAL u[4], v[4];
                    MLVector2D UV = { 0.0, 0.0 };
                    for (int n = 0; n < numPV; ++n) {
                        paramVerts[n]->getUV(&(u[n]), &(v[n]));
                        UV[0] += u[n];
                        UV[1] += v[n];
                    }
                    UV[0] /= numPV;
                    UV[1] /= numPV;

                    GeometryGroup *geom_group =
                        meshAssoc.getGeometryGroupByID(gref[0]);
                    const std::vector<std::string> entityNames = geom_group->getEntityNames();
                    ML_assert(1 == entityNames.size());


                    MLVector3D interpolatedPoint;
                    if (geom_kernel->evalXYZ(UV, entityNames[0], interpolatedPoint)) {

                        MLREAL dist = 0.0;
                        for (int n = 0; n < 3; ++n) {
                            dist += std::pow((expectedInterpolatedPoint[n] - interpolatedPoint[n]), 2.0);
                        }
                        dist = std::sqrt(dist);
                        if (dist > tol) {
                            printf("bad point interpolation\n");
                            ML_assert(dist < tol);
                        }
                    }
                    else {
                        printf("invalid param interpolation\n");
                        ML_assert(0 == 1);
                    }
                }
                else {
                    printf("invalid param verts\n");
                    ML_assert(0 == 1);
                }
            }
            else {
                printf("missing param verts\n");
                ML_assert(paramVerts[0] && paramVerts[1] && paramVerts[2] && paramVerts[3]);
            }
        }
        else {
            printf("missing Geometry Group\n");
            ML_assert(NULL != geom_group);
            return 1;
        }

    }

    return status;
}




//===============================================================================
// Test the mesh-geometry associativity in om6.xml
int
oneraM6_tests(MeshAssociativity &meshAssoc)
{
    int ret = 0;

    printf("om6.xml Tests\n");

    // Name of mesh model
    const char *target_block_name = "/Base/oneraM6";
    MeshModel* meshModel = meshAssoc.getMeshModelByName(target_block_name);
    if (meshModel) {

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

        // Load Project Geode Kernel and set as active kernel
        GeometryKernelGeode geomKernel;
        meshAssoc.addGeometryKernel(&geomKernel);
        meshAssoc.setActiveGeometryKernelByName(geomKernel.getName());

        // Read geometry files
        MLREAL modelSize = 1000.0;
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

                    /* Get ModelSize attribute */
                    if (strcmp("model size", attName) == 0) {
                        MLREAL value;
                        if (1 == sscanf(attValue, "%lf", &value) &&
                            value > 0.0) {
                            modelSize = value;
                        }
                    }

                }
            }


            /* Define ModelSize prior to reading geometry */
            /* Ensures proper tolerances when building the database */
            geomKernel.setModelSize(modelSize);
            if (geomKernel.getModelSize() != modelSize) {
                printf("Error defining model size\n  %lf\n", modelSize);
                return (1);
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

            /*===============================================================================================
             * Curve Evaluation Test
             */
            {
                const std::string curve_entity_name("wing_tip_upper_con_3"); /* rounded LE portion */
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
                bool    Linear;           /* If true, the curve is linear and has no unique normal */

                if (geomKernel.evalCurvatureOnCurve(UV, curve_entity_name,
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
                    if (relDiff > 0.01) {
                        ML_assert(0 == 1);
                        ret = 1;
                    }

                    if (Linear) {
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
                const std::string surface_entity_name("TrimSurf-173"); /* rounded wingtip near LE */
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
                MLORIENT        orientation;       /* Orientation of surface in model */
                MLREAL          minTolerance;      /* minimum model assembly tolerance on the surface */
                MLREAL          maxTolerance;      /* maximum model assembly tolerance on the surface */

                if (geomKernel.evalCurvatureOnSurface(UV, surface_entity_name,
                    XYZ, dXYZdU, dXYZdV,
                    d2XYZdU2, d2XYZdUdV, d2XYZdV2,
                    surfaceNormal, principalV,
                    &minCurvature, &maxCurvature, &avg, &gauss,
                    &orientation)) {
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

                    // Determine the minimum and maximum physical tolerance required to
                    // connect the surface boundaries with neighbors in the model.
                    if (!geomKernel.evalSurfaceTolerance(surface_entity_name, minTolerance, maxTolerance)) {
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


            MLVector3D edgePoints[2] = { 0.9285500947461318,  1.0124483762131067, -0.0324033516787493, 
                                         0.7669554599867245,  0.9182977213461775, -0.0351266055529801 };
            MLINT edgeInds[2] = { 48, 35 };

            const char *edge_entity_name = "bottom_con_1";
            MLINT  edge_gref[2] = { 15, 15 };

            MLREAL edge_u[2] = { 0, 0.625156631213186, };
            MLREAL edge_radius[2] = { 0.239211, 0.501719 };


            // known-good projected and parametric interpolated mid-edge XYZs
            MLVector3D projectedMidEdgePt = { -0.45307208568368834, 0.17708402010909388, 0.46523007284560064 };
            MLVector3D interpolatedMidEdgePt = { -0.45617166, 0.23260459, 0.440425 };


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

                MeshEdge *meshEdge = meshModel->findLowestTopoEdgeByInds( 
                    edgeInds[0], edgeInds[1]);
                if (meshEdge) {

                    std::vector<ParamVertex *> paramVerts;
                    MLINT num_pvObjs;
                    meshEdge->getParamVerts(paramVerts);
                    num_pvObjs = meshEdge->getNumParamVerts();
                    if (num_pvObjs == 2) {
                        /* have parametric data */
                        MLINT gref;
                        MLVector2D UV;
                        MLINT ipv;
                        for (ipv = 0; ipv < num_pvObjs; ++ipv) {
                            gref = paramVerts[ipv]->getGref();
                            paramVerts[ipv]->getUV(&(UV[0]), &(UV[1]));
                            if (gref != edge_gref[ipv] ||
                                UV[0] != edge_u[ipv] ||
                                UV[1] != edge_v[ipv]) {
                                /* didn't get the parametric data we were expecting */
                                printf("MESH EDGE Test: incorrect point parametric data\n");
                                ML_assert(0 == 1);
                                ret = 1;
                            }
                            else {
                                GeometryGroup *geom_group = meshAssoc.getGeometryGroupByID(gref);
                                if (NULL == geom_group) {
                                    printf("MESH EDGE Test:missing Geometry Group\n");
                                    ML_assert(NULL != geom_group);
                                    return 1;
                                }

                                const std::vector<std::string> entityNames = geom_group->getEntityNames();
                                MLINT num_entityNames = (MLINT)entityNames.size();
                                if ( num_entityNames != 1 ||
                                    0 != strcmp(entityNames[0].c_str(), edge_entity_name)) {
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

                    for (int n = 0; n < 3; ++n) {
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
                MLVector3D projectedMidFacePt =    { 0.81641696167, 0.992560226574, -0.034239069084 };
                MLVector3D interpolatedMidFacePt = { 0.81616651244, 0.992532945141, -0.034237164401 };

                MLVector3D point;

                MeshFace *meshFace = meshModel->findFaceByInds(
                    faceInds[0], faceInds[1], faceInds[2]);
                if (meshFace) {

                    std::vector<ParamVertex *> paramVerts;
                    MLINT num_pvObjs;
                    meshFace->getParamVerts(paramVerts);
                    num_pvObjs = meshFace->getNumParamVerts();
                    if (num_pvObjs == 3) {
                        /* have parametric data */
                        MLINT gref;
                        MLVector2D UV;
                        MLINT ipv;
                        for (ipv = 0; ipv < num_pvObjs; ++ipv) {
                            gref = paramVerts[ipv]->getGref();
                            paramVerts[ipv]->getUV(&(UV[0]), &(UV[1]));
                            if (gref != face_gref[ipv] ||
                                UV[0] != face_u[ipv] ||
                                UV[1] != face_v[ipv]) {
                                /* didn't get the parametric data we were expecting */
                                printf("MESH FACE Test: incorrect point parametric data\n");
                                ML_assert(0 == 1);
                                ret = 1;
                            }
                            else {
                                GeometryGroup *geom_group = meshAssoc.getGeometryGroupByID(gref);
                                if (NULL == geom_group) {
                                    printf("MESH FACE Test:missing Geometry Group\n");
                                    ML_assert(NULL != geom_group);
                                    return 1;
                                }

                                const std::vector<std::string> entityNames = geom_group->getEntityNames();
                                MLINT num_entityNames = (MLINT)entityNames.size();
                                if (num_entityNames != 1 ||
                                    0 != strcmp(entityNames[0].c_str(), face_entity_name)) {
                                    printf("evaluateParamPoint:bad Geometry Group\n");
                                    ML_assert(0 == 1);
                                    return 1;
                                }
                            }
                        }
                    }
                    else {
                        printf("MESH Face Test: incorrect face parametric data\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }

                    /* interpolate parametric coords, evaluate XYZ and compare to
                       expected value "interpolatedMidFacePt" */
                    if (0 != interpolateFaceMidPoint(meshAssoc, meshFace, interpolatedMidFacePt)) {
                        printf("bad face parametric evaluation\n");
                        ML_assert(0 == 1);
                        ret = 1;
                    }
                    else {
                        printf("\nMesh Face parametric evaluation OK\n");
                    }

                    for (int n = 0; n < 3; ++n) {
                        point[n] = (facePoints[0][n] + facePoints[1][n] + facePoints[2][n])/3.0;
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
                    printf("missing constrained point\n");
                    ML_assert(NULL != meshFace);
                    ret = 1;
                }
            }


        }
#endif
    }
    else {
        printf("missing Mesh Model\n");
        ML_assert(NULL != meshModel);
        ret = 1;
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
