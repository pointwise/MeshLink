﻿/***********************************************************************
**  MeshLink Library Example
**  Use MeshLink file to refine an unstructured tri mesh.
**
**  Read MeshLink file.
**     + read NMB geometry file.
**  Read input mesh VRML file.
**  Create a refined mesh from the input mesh.
**  Adhere associated mesh topology to geometry.
**  Write refined mesh to VRML file.
**/

// MeshLink Library Headers
#include "Types.h"
#include "GeomKernel_Geode.h"
#include "MeshAssociativity.h"
#if defined(HAVE_XERCES)
#include "MeshLinkParser_xerces.h"
#endif

// Refine_Uns Project Headers
#include "surf_mesh.h"
#include "quality.h"
#include "queue.h"

#include <stdio.h>


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
    std::string vrml_fname     = "oneraM6_wingsurf.wrl";
    std::string vrml_out_fname = "oneraM6_wingsurf_refined.wrl";

    MeshAssociativity meshAssoc;
    SurfMesh surfMesh;

    // Refinement Criteria
    // qualityThreshold - maximum edge circular arc subtension (degrees) of underlying geometry
    MLREAL qualityThreshold = 20.0;

    MLINT maxNumEdgeSplits = 10000;  // maximum number of edge splits to perform
    MLINT maxGenerations = 9;        // maximum number of edge split generations

    // Mesh quality constraints
    surfMesh.setMeshConstraints(
        0.005,          // min allowed edge length
        20.0,           // max allowed tri aspect ratio
        5.0);           // min allowed tri included angle
    

#if defined(HAVE_XERCES)
    // Read Geometry-Mesh associativity
    {
        // Xerces MeshLink XML parser
        MeshLinkParserXerces parser;

        // Validate first
        parser.validate(meshlink_fname, schema_fname);

        if (!parser.parseMeshLinkFile(meshlink_fname, &meshAssoc)) {
            printf("Error parsing geometry-mesh associativity\n");
            return (-1);
        }
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif

    // Read VRML file into SurfMesh and build Face and Edge Arrays
    if (!surfMesh.readVrml(vrml_fname) || !surfMesh.createEdges()) {
        return (-1);
    }

    const char *target_block_name = "volume";
    MeshModel* meshModel = NULL;
    if (NULL == (meshModel = meshAssoc.getMeshModelByName(target_block_name)) ) {
        printf("\nError: missing MeshModel \"%s\"\n", target_block_name);
        return (-1);
    }

    // Load Project Geode Kernel and set as active kernel
    GeometryKernelGeode geomKernel;
    meshAssoc.addGeometryKernel(&geomKernel);
    meshAssoc.setActiveGeometryKernelByName(geomKernel.getName());

    // Read geometry files specified in MeshLink file
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

    GeometryKernel *geom_kernel = NULL;
    if (NULL == (geom_kernel = meshAssoc.getActiveGeometryKernel()) ) {
        printf("ERROR: no active geometry kernel\n");
        return (-1);
    }

    // print geometry resolution of the input mesh
    surfMesh.computeGeometryResolutionStatistics(meshAssoc, *meshModel);

    // Refine edges to minimize the edge quality variable
    // Refinement proceeds in rounds of generations
    // Within a generation, an edge is only allowed to
    // be split once (no child edges split)
    MLINT i;
    MLREAL quality;
    MLINT generation = 0;
    MLINT numSplitsTotal = 0;
    std::vector<MLINT> newEdgeIndsToCheck;
    std::vector<MLINT>::iterator newEdgeIndsIter;

    // A queue of the candidate edges for splitting is used
    EdgeRefineQueue queue;              // active queue of candidate edges for splitting
    EdgeRefineQueue queueForNextGen;    // inactive queue of child edges to be considered in the next generation

    // Begin by adding all edges violating the qualityThreshold
    // to the next-gen queue
    MLINT numEdges = surfMesh.edges_.size();
    for (i = 0; i < numEdges; ++i) {
        EdgeQual edgeQ(i);
        edgeQ.computeQuality(surfMesh, meshAssoc, *meshModel,
            geom_kernel, surfMesh.minAllowedEdgeLength_);
        if (edgeQ.quality_ > qualityThreshold) {
            queueForNextGen.push_back(edgeQ);
        }
    }

    // In an effort to minimize impact on mesh quality, 
    // blend quality to neighbors of those in the queue
    addNeighborsToQueue(surfMesh, meshAssoc,
        *meshModel, geom_kernel, qualityThreshold, queueForNextGen);

    printf("\nEdge Refinement Start\n");
    printf("   qualityThreshold = %.1f\n", qualityThreshold);
    printf("   minAllowedEdgeLength = %.3f\n", surfMesh.minAllowedEdgeLength_);
    printf("   maxGenerations = %" MLINT_FORMAT "\n", maxGenerations);
    printf("   maxNumEdgeSplits = %" MLINT_FORMAT "\n", maxNumEdgeSplits);

    while (generation < maxGenerations && queueForNextGen.size() > 0) {
        ++generation;
        MLINT numSplits = 0;

        printf("\nRefinement Generation %" MLINT_FORMAT " Begin: %" MLINT_FORMAT " edges queued\n",
            generation, (MLINT)queueForNextGen.size());

        // Swap the next-gen queue into the active queue
        queue.swap(queueForNextGen);
        std::sort(queue.begin(), queue.end());

        while (queue.size() > 0 && numSplitsTotal < maxNumEdgeSplits) {
            EdgeQual &edgeQ = queue.back();
            i = edgeQ.index_;
            MLREAL parentEdgeLen = edgeQ.edgeLen_;
            quality = edgeQ.quality_;
            queue.pop_back();

            SurfEdge &surfEdge = surfMesh.edges_[i];
            if (quality > qualityThreshold) {
                // split the edge and project the new point onto the
                // associated geometry
                numEdges = surfMesh.edges_.size();
                if (surfMesh.splitEdge(meshAssoc, meshModel, surfEdge, newEdgeIndsToCheck)) {
                    // edge split was successful
                    numSplits++;
                    numSplitsTotal++;

                    // add new edges to the next-gen queue
                    ml_assert(i == *(newEdgeIndsToCheck.begin()));  // original edge should be first

                    for (newEdgeIndsIter = newEdgeIndsToCheck.begin();
                        newEdgeIndsIter != newEdgeIndsToCheck.end(); ++newEdgeIndsIter) {
                        i = *newEdgeIndsIter;

                        EdgeQual edgeQnew(i);
                        edgeQnew.computeQuality(surfMesh, meshAssoc,
                            *meshModel, geom_kernel, surfMesh.minAllowedEdgeLength_);

                        if (edgeQnew.quality_ > qualityThreshold) {
                            queueForNextGen.push_back(edgeQnew);
                        }
                    }
                }
            }
        }

        // blend quality to queue neighbors
        addNeighborsToQueue(surfMesh, meshAssoc,
            *meshModel, geom_kernel, qualityThreshold, queueForNextGen);

        printf("Refinement Generation %" MLINT_FORMAT "   End: %" MLINT_FORMAT " edge splits performed\n",
            generation, numSplits);
    }

    printf("\nPerformed %" MLINT_FORMAT " edge splits in %" MLINT_FORMAT " generations\n",
        numSplitsTotal, generation);

    // print geometry resolution of the current mesh
    surfMesh.computeGeometryResolutionStatistics(meshAssoc, *meshModel);

    // write the refined mesh
    if (!surfMesh.writeVrml(vrml_out_fname)) {
        return (-1);
    }

    return ret;
}
