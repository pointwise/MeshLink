/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

/****************************************************************************
*
* StrMesh class
*
* A class for storing and working with a structured mesh block.
*
***************************************************************************/

#ifndef STR_MESH_CLASS
#define STR_MESH_CLASS

#include "Types.h"

#include <algorithm>
#include <string>
#include <vector>
#include <assert.h>

void pw_assert(bool val);

class MeshAssociativity;
class MeshModel;

class StrMesh {
public:
    MLINT id_;
    MLINT jd_;
    MLINT kd_;
    std::vector<MLREAL> xyz_[3];

    StrMesh() :
        id_(0),
        jd_(0),
        kd_(0)
    {
    }

    // Refinement constructor
    // Create a new StrMesh from a given StrMesh with optional refinement
    // New mesh points are created by linear interpolation
    //   mesh - source StrMesh
    //   refine_i - refinement multiple in I-direction
    //   refine_j - refinement multiple in J-direction
    //   refine_k - refinement multiple in K-direction
    StrMesh(const StrMesh &mesh, MLINT refine_i, MLINT refine_j, MLINT refine_k);

    ~StrMesh() {
        clear();
    }

    void clear() {
        id_ = jd_ = kd_ = 0;
        xyz_[0].clear();
        xyz_[1].clear();
        xyz_[2].clear();
    }

    // Convert IJK to linear index (zero based)
    static MLINT ijk2ind(MLINT i, MLINT j, MLINT k, MLINT id, MLINT jd);

    // Convert linear index (zero based) to IJK
    static void ind2ijk2(MLINT ind, MLINT &i, MLINT &j, MLINT &k, MLINT id,
        MLINT jd);

    bool writePLOT3D(std::string fname);

    bool readPLOT3D(std::string fname);

    // Project block boundary edge and face interpolated points
    // to the associated CAD group
    bool adhere(MLINT refine_i, MLINT refine_j, MLINT refine_k,
        MeshModel &meshModel, MeshAssociativity &meshAssoc);
};

#endif

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
