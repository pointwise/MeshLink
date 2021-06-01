/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "StrMesh.h"
#include "MeshAssociativity.h"

#include <algorithm>
#include <string.h>


// Refinement constructor
// Create a new StrMesh from a given StrMesh with optional refinement
// New mesh points are created by linear interpolation
//   mesh - source StrMesh
//   refine_i - refinement multiple in I-direction
//   refine_j - refinement multiple in J-direction
//   refine_k - refinement multiple in K-direction
StrMesh::StrMesh(const StrMesh &mesh, MLINT refine_i, MLINT refine_j, MLINT refine_k)
{
    refine_i = (std::max)((MLINT)1, refine_i);
    refine_j = (std::max)((MLINT)1, refine_j);
    refine_k = (std::max)((MLINT)1, refine_k);
    printf("Creating refined block with multiples: %" MLINT_FORMAT " x %"
        MLINT_FORMAT " x %" MLINT_FORMAT "\n", refine_i, refine_j, refine_k);
    id_ = (mesh.id_ - 1) * refine_i + 1;
    jd_ = (mesh.jd_ - 1) * refine_j + 1;
    kd_ = (mesh.kd_ - 1) * refine_k + 1;
    printf("Refined block dimensions: %" MLINT_FORMAT " x %" MLINT_FORMAT " x %"
        MLINT_FORMAT "\n", id_, jd_, kd_);
    MLINT i, j, k, n;
    MLINT ri, rj, rk;
    MLINT ind, rind;
    for (n = 0; n < 3; ++n) {
        xyz_[n].resize(id_*jd_*kd_, 0.0);
        for (k = 0; k < mesh.kd_; ++k) {
            rk = k * refine_k;
            for (j = 0; j < mesh.jd_; ++j) {
                rj = j * refine_j;
                for (i = 0; i < mesh.id_; ++i) {
                    ri = i * refine_i;
                    ind = ijk2ind(i, j, k, mesh.id_, mesh.jd_);
                    rind = ijk2ind(ri, rj, rk, id_, jd_);
                    xyz_[n][rind] = mesh.xyz_[n][ind];
                }
            }
        }
    }

    MLINT iref, indm, indp;
    MLREAL factor;

    if (refine_i > 1) {
        factor = 1.0 / refine_i;
        for (n = 0; n < 3; ++n) {
            for (k = 0; k < kd_; ++k) {
                for (j = 0; j < jd_; ++j) {
                    for (i = 0; i < mesh.id_ - 1; ++i) {
                        ri = i * refine_i;
                        indm = ijk2ind(ri, j, k, id_, jd_);
                        indp = ijk2ind((i + 1) * refine_i, j, k, id_, jd_);
                        pw_assert(indm < (MLINT)xyz_[n].size());
                        pw_assert(indp < (MLINT)xyz_[n].size());
                        for (iref = 1; iref < refine_i; ++iref) {
                            ri++;
                            rind = ijk2ind(ri, j, k, id_, jd_);
                            pw_assert(rind < (MLINT)xyz_[n].size());
                            xyz_[n][rind] = (1.0 - iref*factor)*xyz_[n][indm] +
                                iref*factor*xyz_[n][indp];
                        }
                    }
                }
            }
        }
    }

    if (refine_j > 1) {
        factor = 1.0 / refine_j;
        for (n = 0; n < 3; ++n) {
            for (k = 0; k < kd_; ++k) {
                for (j = 0; j < mesh.jd_ - 1; ++j) {
                    for (i = 0; i < id_; ++i) {
                        rj = j * refine_j;
                        indm = ijk2ind(i, rj, k, id_, jd_);
                        indp = ijk2ind(i, (j + 1) * refine_j, k, id_, jd_);
                        pw_assert(indm < (MLINT)xyz_[n].size());
                        pw_assert(indp < (MLINT)xyz_[n].size());
                        for (iref = 1; iref < refine_j; ++iref) {
                            rj++;
                            rind = ijk2ind(i, rj, k, id_, jd_);
                            pw_assert(rind < (MLINT)xyz_[n].size());
                            xyz_[n][rind] = (1.0 - iref*factor)*xyz_[n][indm] +
                                iref*factor*xyz_[n][indp];
                        }
                    }
                }
            }
        }
    }

    if (refine_k > 1) {
        factor = 1.0 / refine_k;
        for (n = 0; n < 3; ++n) {
            for (k = 0; k < mesh.kd_ - 1; ++k) {
                for (j = 0; j < jd_; ++j) {
                    for (i = 0; i < id_; ++i) {
                        rk = k * refine_k;
                        indm = ijk2ind(i, j, rk, id_, jd_);
                        indp = ijk2ind(i, j, (k + 1) * refine_k, id_, jd_);
                        pw_assert(indm < (MLINT)xyz_[n].size());
                        pw_assert(indp < (MLINT)xyz_[n].size());
                        for (iref = 1; iref < refine_k; ++iref) {
                            rk++;
                            rind = ijk2ind(i, j, rk, id_, jd_);
                            pw_assert(rind < (MLINT)xyz_[n].size());
                            xyz_[n][rind] = (1.0 - iref*factor)*xyz_[n][indm] +
                                iref*factor*xyz_[n][indp];
                        }
                    }
                }
            }
        }
    }

}


// Convert IJK to linear index (zero based)
MLINT
StrMesh::ijk2ind(MLINT i, MLINT j, MLINT k, MLINT id, MLINT jd)
{
    return k *id*jd + j*id + i;
}


// Convert linear index (zero based) to IJK
void
StrMesh::ind2ijk2(MLINT ind, MLINT &i, MLINT &j, MLINT &k, MLINT id, MLINT jd)
{
    MLINT idjd = id*jd;
    k = ind / idjd + 1;
    MLINT remainder = ind % idjd;
    if (remainder == 0) {
        k = k - 1;
        remainder = ind - (k - 1)*idjd;
    }
    j = remainder / id + 1;
    remainder = ind % id;
    if (remainder == 0) {
        j = j - 1;
        remainder = ind - (k - 1)*idjd - (j - 1)*id;
    }
    i = remainder;
    --i; --j; --k;
}


bool
StrMesh::writePLOT3D(std::string fname)
{
    printf("Writing PLOT3D Mesh: %s\n", fname.c_str());
    bool status = true;
    MLINT num_points = id_ * jd_ * kd_;
    if (6 > num_points) return false;

    FILE *fp = fopen(fname.c_str(), "w");
    if (!fp) return false;
    MLINT num_blocks = 1;

    fprintf(fp, "%" MLINT_FORMAT "\n", num_blocks);
    fprintf(fp, "%" MLINT_FORMAT " %" MLINT_FORMAT " %" MLINT_FORMAT "\n", id_,
        jd_, kd_);

    MLINT i, n, icol;
    MLINT max_col = 4;
    for (n = 0; n < 3; ++n) {
        for (i = 0, icol = 0; i < num_points; ++i, ++icol) {
            if (icol == max_col) {
                fprintf(fp, "\n");
                icol = 0;
            }
            fprintf(fp, "%23.16e ", xyz_[n][i]);
        }
        if (icol > 1) {
            fprintf(fp, "\n");
        }
    }
    fclose(fp);
    return status;
}


bool
StrMesh::readPLOT3D(std::string fname)
{
    printf("Reading PLOT3D Mesh: %s\n", fname.c_str());
    bool status = true;
    FILE *fp = fopen(fname.c_str(), "r");
    if (!fp) return false;
    char line[1024];
    MLINT num_blocks, num_points;
    if (!fgets(line, 1000, fp)) {
        status = false;
        goto cleanup;
    }
    line[strcspn(line, "\n")] = 0;
    if (1 != sscanf(line, "%" MLINT_FORMAT , &num_blocks) || num_blocks != 1) {
        status = false;
        goto cleanup;
    }

    if (!fgets(line, 1000, fp)) {
        status = false;
        goto cleanup;
    }
    line[strcspn(line, "\n")] = 0;
    if (3 != sscanf(line, "%" MLINT_FORMAT " %" MLINT_FORMAT " %" MLINT_FORMAT ,
            &id_, &jd_, &kd_)) {
        status = false;
        goto cleanup;
    }
    if (id_ < 2 || jd_ < 2 || kd_ < 2) {
        status = false;
        goto cleanup;
    }
    num_points = id_ * jd_ * kd_;
    MLREAL points[4];
    MLINT n, i;
    for (n = 0; n < 3; ++n) {
        xyz_[n].resize(num_points);
        MLINT num_points_read = 0;
        MLINT num_read;
        while (num_points_read < num_points && fgets(line, 1000, fp)) {
            line[strcspn(line, "\n")] = 0;
            if (1 >(num_read = sscanf(line, "%lf %lf %lf %lf",
                        &points[0], &points[1], &points[2], &points[3]))) {
                status = false;
                goto cleanup;
            }
            for (i = 0; i < num_read; ++i) {
                xyz_[n][num_points_read++] = points[i];
            }
        }
    }
cleanup:
    fclose(fp);
    if (!status) {
        clear();
    }
    else {
        printf("Block dimensions: %" MLINT_FORMAT " x %" MLINT_FORMAT " x %"
            MLINT_FORMAT "\n", id_, jd_, kd_);
    }
    return status;
}


// Project block boundary edge and face interpolated points
// to the associated CAD group
bool
StrMesh::adhere(MLINT refine_i, MLINT refine_j, MLINT refine_k,
    MeshModel &meshModel, MeshAssociativity &meshAssoc)
{
    printf("Adhering refinement points to associated CAD geoemtry\n");
    bool status = true;

    GeometryKernel *geom_kernel = meshAssoc.getActiveGeometryKernel();
    if (!geom_kernel) {
        status = false;
        printf("ERROR: no active geometry kernel\n");
        return status;
    }
    ProjectionData projectionData(geom_kernel);

    MLINT orig_id = (id_ - 1) / refine_i + 1;
    MLINT orig_jd = (jd_ - 1) / refine_j + 1;
    MLINT orig_kd = (kd_ - 1) / refine_k + 1;

    MLINT i, j, k, iref, n;
    MLINT ri, rj, rk;
    MLINT indm, indp, rind;
    MeshEdge *edge;
    MLINT indmm, indmp, indpm, indpp, iref1, iref2;
    MeshFace *face;
    MLVector3D point;
    
    // I-direction edge projection
    if (refine_i > 1) {
        // Loop over original mesh
        for (k = 0; k < orig_kd; ++k) {
            rk = k * refine_k;
            for (j = 0; j < orig_jd; ++j) {
                rj = j * refine_j;
                for (i = 0; i < orig_id - 1; ++i) {
                    // refined mesh indices
                    ri = i * refine_i;

                    // original mesh edge indices
                    indm = ijk2ind(i, j, k, orig_id, orig_jd) + 1;
                    indp = ijk2ind(i + 1, j, k, orig_id, orig_jd) + 1;

                    edge = meshModel.findLowestTopoEdgeByInds(indm, indp);
                    if (edge) {
                        // original edge was associated with geometry group
                        // project interpolated points onto geometry group
                        GeometryGroup *geom_group =
                            meshAssoc.getGeometryGroupByID(edge->getGref());
                        if (geom_group) {
                            for (iref = 1; iref < refine_i; ++iref) {
                                ri++;
                                rind = ijk2ind(ri, rj, rk, id_, jd_);
                                pw_assert(rind < (MLINT)xyz_[0].size());

                                for (n = 0; n < 3; ++n) point[n] = xyz_[n][rind];
                                if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                                    printf("Point projection failed\n");
                                }
                                else {
                                    geom_kernel->getProjectionXYZ(projectionData, point);
                                    for (n = 0; n < 3; ++n) xyz_[n][rind] = point[n];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // J-direction edge projection
    if (refine_j > 1) {
        // Loop over original mesh
        for (k = 0; k < orig_kd; ++k) {
            rk = k * refine_k;
            for (j = 0; j < orig_jd - 1; ++j) {
                rj = j * refine_j;
                for (i = 0; i < orig_id; ++i) {
                    // refined mesh indices
                    ri = i * refine_i;
                    rj = j * refine_j;

                    // original mesh edge indices
                    indm = ijk2ind(i, j, k, orig_id, orig_jd) + 1;
                    indp = ijk2ind(i, j + 1, k, orig_id, orig_jd) + 1;

                    edge = meshModel.findLowestTopoEdgeByInds(indm, indp);
                    if (edge) {
                        // original edge was associated with geometry group
                        // project interpolated points onto geometry group
                        GeometryGroup *geom_group =
                            meshAssoc.getGeometryGroupByID(edge->getGref());
                        if (geom_group) {
                            for (iref = 1; iref < refine_j; ++iref) {
                                rj++;
                                rind = ijk2ind(ri, rj, rk, id_, jd_);
                                pw_assert(rind < (MLINT)xyz_[0].size());

                                for (n = 0; n < 3; ++n) point[n] = xyz_[n][rind];
                                if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                                    printf("Point projection failed\n");
                                }
                                else {
                                    geom_kernel->getProjectionXYZ(projectionData, point);
                                    for (n = 0; n < 3; ++n) xyz_[n][rind] = point[n];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // K-direction edge projection
    if (refine_k > 1) {
        // Loop over original mesh
        for (k = 0; k < orig_kd - 1; ++k) {
            for (j = 0; j < orig_jd; ++j) {
                rj = j * refine_j;
                for (i = 0; i < orig_id; ++i) {
                    // refined mesh indices
                    rk = k * refine_k;
                    ri = i * refine_i;

                    // original mesh edge indices
                    indm = ijk2ind(i, j, k, orig_id, orig_jd) + 1;
                    indp = ijk2ind(i, j, k + 1, orig_id, orig_jd) + 1;

                    edge = meshModel.findLowestTopoEdgeByInds(indm, indp);
                    if (edge) {
                        // original edge was associated with geometry group
                        // project interpolated points onto geometry group
                        GeometryGroup *geom_group =
                            meshAssoc.getGeometryGroupByID(edge->getGref());
                        if (geom_group) {
                            for (iref = 1; iref < refine_k; ++iref) {
                                rk++;
                                rind = ijk2ind(ri, rj, rk, id_, jd_);
                                pw_assert(rind < (MLINT)xyz_[0].size());

                                for (n = 0; n < 3; ++n) point[n] = xyz_[n][rind];
                                if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                                    printf("Point projection failed\n");
                                }
                                else {
                                    geom_kernel->getProjectionXYZ(projectionData, point);
                                    for (n = 0; n < 3; ++n) xyz_[n][rind] = point[n];
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // I-plane face projection
    if (refine_k > 1 && refine_j > 1) {
        // Loop over original mesh
        for (k = 0; k < orig_kd - 1; ++k) {
            for (j = 0; j < orig_jd - 1; ++j) {
                for (i = 0; i < orig_id; i += orig_id - 1) {  // Imin,Imax only
                    ri = i * refine_i;

                    // original mesh face indices
                    indmm = ijk2ind(i, j, k, orig_id, orig_jd) + 1;
                    indmp = ijk2ind(i, j, k + 1, orig_id, orig_jd) + 1;
                    indpm = ijk2ind(i, j + 1, k, orig_id, orig_jd) + 1;
                    indpp = ijk2ind(i, j + 1, k + 1, orig_id, orig_jd) + 1;

                    face = meshModel.findFaceByInds(indmm, indpm, indpp, indmp);
                    if (face) {
                        // original face was associated with geometry group
                        // project interpolated points onto geometry group
                        GeometryGroup *geom_group =
                            meshAssoc.getGeometryGroupByID(face->getGref());
                        if (geom_group) {
                            // refined mesh indices
                            rk = k * refine_k;
                            for (iref1 = 1; iref1 < refine_k; ++iref1) {
                                rk++;
                                rj = j * refine_j;
                                for (iref2 = 1; iref2 < refine_j; ++iref2) {
                                    rj++;
                                    rind = ijk2ind(ri, rj, rk, id_, jd_);
                                    pw_assert(rind < (MLINT)xyz_[0].size());

                                    for (n = 0; n < 3; ++n) point[n] = xyz_[n][rind];
                                    if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                                        printf("Point projection failed\n");
                                    }
                                    else {
                                        geom_kernel->getProjectionXYZ(projectionData, point);
                                        for (n = 0; n < 3; ++n) xyz_[n][rind] = point[n];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }


    // J-plane face projection
    if (refine_i > 1 && refine_k > 1) {
        // Loop over original mesh
        for (k = 0; k < orig_kd - 1; ++k) {
            for (j = 0; j < orig_jd; j += orig_jd - 1) {  // Jmin,Jmax only
                rj = j * refine_j;
                for (i = 0; i < orig_id - 1; ++i) {
                    // original mesh face indices
                    indmm = ijk2ind(i, j, k, orig_id, orig_jd) + 1;
                    indmp = ijk2ind(i, j, k + 1, orig_id, orig_jd) + 1;
                    indpm = ijk2ind(i + 1, j, k, orig_id, orig_jd) + 1;
                    indpp = ijk2ind(i + 1, j, k + 1, orig_id, orig_jd) + 1;

                    face = meshModel.findFaceByInds(indmm, indpm, indpp, indmp);
                    if (face) {
                        // original face was associated with geometry group
                        // project interpolated points onto geometry group
                        GeometryGroup *geom_group =
                            meshAssoc.getGeometryGroupByID(face->getGref());
                        if (geom_group) {
                            // refined mesh indices
                            rk = k * refine_k;
                            for (iref1 = 1; iref1 < refine_k; ++iref1) {
                                rk++;
                                ri = i * refine_i;
                                for (iref2 = 1; iref2 < refine_i; ++iref2) {
                                    ri++;
                                    rind = ijk2ind(ri, rj, rk, id_, jd_);
                                    pw_assert(rind < (MLINT)xyz_[0].size());

                                    for (n = 0; n < 3; ++n) point[n] = xyz_[n][rind];
                                    if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                                        printf("Point projection failed\n");
                                    }
                                    else {
                                        geom_kernel->getProjectionXYZ(projectionData, point);
                                        for (n = 0; n < 3; ++n) xyz_[n][rind] = point[n];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    // K-plane face projection
    if (refine_i > 1 && refine_j > 1) {
        // Loop over original mesh
        for (k = 0; k < orig_kd; k += orig_kd - 1) {  // Kmin,Kmax only
            rk = k * refine_k;
            for (j = 0; j < orig_jd - 1; ++j) {
                for (i = 0; i < orig_id - 1; ++i) {

                    // original mesh face indices
                    indmm = ijk2ind(i, j, k, orig_id, orig_jd) + 1;
                    indmp = ijk2ind(i, j + 1, k, orig_id, orig_jd) + 1;
                    indpm = ijk2ind(i + 1, j, k, orig_id, orig_jd) + 1;
                    indpp = ijk2ind(i + 1, j + 1, k, orig_id, orig_jd) + 1;

                    face = meshModel.findFaceByInds(indmm, indpm, indpp, indmp);
                    if (face) {
                        // original face was associated with geometry group
                        // project interpolated points onto geometry group
                        GeometryGroup *geom_group =
                            meshAssoc.getGeometryGroupByID(face->getGref());
                        if (geom_group) {
                            // refined mesh indices
                            ri = i * refine_i;
                            for (iref1 = 1; iref1 < refine_i; ++iref1) {
                                ri++;
                                rj = j * refine_j;
                                for (iref2 = 1; iref2 < refine_j; ++iref2) {
                                    rj++;
                                    rind = ijk2ind(ri, rj, rk, id_, jd_);
                                    pw_assert(rind < (MLINT)xyz_[0].size());

                                    for (n = 0; n < 3; ++n) point[n] = xyz_[n][rind];
                                    if (!geom_kernel->projectPoint(geom_group, point, projectionData)) {
                                        printf("Point projection failed\n");
                                    }
                                    else {
                                        geom_kernel->getProjectionXYZ(projectionData, point);
                                        for (n = 0; n < 3; ++n) xyz_[n][rind] = point[n];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }

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
