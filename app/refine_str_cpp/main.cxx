/***********************************************************************
**  MeshLink Library Example
**  Use MeshLink file to refine a structured block.
**
**  Read MeshLink file.
**     + read NMB geometry file.
**  Read input mesh PLOT3D file.
**  Create a refined mesh from the input mesh.
**  Adhere associated mesh topology to geometry.
**  Write refined block PLOT3D file.
**/

#include "Types.h"
#include "GeomKernel_Geode.h"
#include "MeshAssociativity.h"
#include "StrMesh.h"
#if defined(HAVE_XERCES)
#include "MeshLinkParser_xerces.h"
#endif

#include <stdio.h>

void pw_assert(bool val) {
    if (!val) {
        assert(val);
    }
}


int main(int argc, char** argv)
{
    int ret = 0;
    if (argc != 2) {
        printf("usage: <program name> <xml file name>\n");
        exit(1);
    }

    // Name of mesh file
    const char *target_block_name = "/Base/sphere";

    std::string target_mesh_fname("sphere.x");

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

        if (!parser.parseMeshLinkFile(meshlink_fname, &meshAssoc)) {
            printf("Error parsing geometry-mesh associativity\n");
            return (-1);
        }
    }
#else
    printf("Error parsing geometry-mesh associativity\n");
    return (-1);
#endif

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

    // Read mesh files
    MLINT numMeshFiles = meshAssoc.getNumMeshFiles();
    const std::vector<MeshFile> &meshFiles = meshAssoc.getMeshFiles();

    for (iFile = 0; iFile < numMeshFiles; ++iFile) {
        const MeshFile &meshFile = meshFiles[iFile];
        const char * mesh_fname = meshFile.getFilename();



        printf("\nMeshFile Attributes\n");
        std::vector<MLINT> attIDs = meshFile.getAttributeIDs(meshAssoc);
        MLINT numAtts = attIDs.size();

        MLINT iAtt;
        for (iAtt = 0; iAtt < numAtts; ++iAtt) {
            const char *attName, *attValue;
            if (meshAssoc.getAttribute(attIDs[iAtt], &attName, &attValue)) {
                printf("  %" MLINT_FORMAT " %s = %s\n", iAtt, attName, attValue);
            }
        }


        // Read mesh file
        if (target_mesh_fname.compare(mesh_fname) == 0) {
            // Read PLOT3D mesh file
            StrMesh origBlock;
            if (!origBlock.readPLOT3D(mesh_fname)) {
                printf("Error reading mesh file\n");
                return (-1);
            }

            // Create refined mesh by interpolation
            MLINT refine_i = 8;
            MLINT refine_j = 5;
            MLINT refine_k = 6;
            StrMesh refineBlock(origBlock, refine_i, refine_j, refine_k);

            std::string mesh_fname_str = mesh_fname;
            size_t offset = mesh_fname_str.find_last_of('.');
            std::string mesh_rootname = mesh_fname_str.substr(0, offset);
            refineBlock.writePLOT3D(mesh_rootname + "_refine.x");

            // Adhere refined block's interpolated points to geometry
            MeshModel* meshModel = meshAssoc.getMeshModelByName(target_block_name);
            if (!meshModel || !refineBlock.adhere(refine_i, refine_j, refine_k, *meshModel, meshAssoc)) {
                printf("Error adhering refined mesh to geometry\n");
                return (-1);
            }
            refineBlock.writePLOT3D(mesh_rootname + "_refine_adhered.x");
        }
    }

    return ret;
}
