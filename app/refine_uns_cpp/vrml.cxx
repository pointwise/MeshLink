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
 **  SurfMesh Functions for working with VRML files.
 **/

#include "surf_mesh.h"
#include <stdio.h>
#include <cstring>
#include <string>

bool 
SurfMesh::readVrml(const std::string &fname)
{
    bool status = true;

    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp) return false;
    char line[256];
    char *cptr;
    bool inPoints = false;
    bool inFaces = false;
    MLINT inds[5];
    int numRead;
    int i;
    while (status && fgets(line, 256, fp)) {
        line[strcspn(line, "\n")] = 0;

        cptr = line;
        while (*cptr == ' ') { ++cptr; }
        if (0 == strcmp(cptr, "]")) {
            if (inPoints) inPoints = false;
            if (inFaces) inFaces = false;
            continue;
        }

        if (inPoints) {
            SurfPoint point;
            if (3 == sscanf(line, "%lf %lf %lf", &point.r_[0], &point.r_[1], &point.r_[2])) {
                points_.push_back(point);
            }
            else {
                status = false;
                printf("readVrml: error reading point\n%s\n", line);
            }
            continue;
        }

        if (inFaces) {
            numRead = sscanf(line, "%" MLINT_FORMAT ",%" MLINT_FORMAT ",%" MLINT_FORMAT ",%" MLINT_FORMAT ",%" MLINT_FORMAT, 
                &inds[0], 
                &inds[1],
                &inds[2],
                &inds[3],
                &inds[4] );
            if (4 == numRead || 5 == numRead) {
                SurfFace face;
                face.numNodes_ = numRead - 1;
                for (i = 0; i < face.numNodes_; ++i) {
                    face.nodes_[i] = inds[i];
                }
                faces_.push_back(face);
            }
            else {
                status = false;
                printf("readVrml: error reading face\n%s\n", line);
            }
            continue;
        }

        if (0 == strcmp(cptr, "point [")) {
            inPoints = true;
            continue;
        }
        if (0 == strcmp(cptr, "coordIndex [")) {
            inFaces = true;
            continue;
        }
    }

    fclose(fp);
    return status;
}


bool
SurfMesh::writeVrml(const std::string &fname)
{
    bool status = true;
    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp) return false;
    fprintf(fp, "#VRML V1.0 ascii\n");
    fprintf(fp, "#   exported from Refine_Uns\n");
    fprintf(fp, "Separator { \n");

    // write points
    fprintf(fp, " Coordinate3 {\n");
    fprintf(fp, "  point [\n");
    PointArray::iterator piter;
    for (piter = points_.begin(); piter != points_.end(); ++piter) {
        fprintf(fp, "    %.17g %.17g %.17g \n",
            piter->r_[0],
            piter->r_[1],
            piter->r_[2]);
    }
    fprintf(fp, "  ]\n }\n");

    // write faces
    fprintf(fp, " IndexedFaceSet {\n");
    fprintf(fp, "  coordIndex [\n");
    FaceArray::iterator fiter;
    for (fiter = faces_.begin(); fiter != faces_.end(); ++fiter) {
        fprintf(fp, "        %7" MLINT_FORMAT ",%7" MLINT_FORMAT ",%7" MLINT_FORMAT ",%7d,\n",
            fiter->nodes_[0],
            fiter->nodes_[1],
            fiter->nodes_[2],
            -1);
    }

    fprintf(fp, "  ]\n }\n}\n");

    fclose(fp);
    return status;
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
