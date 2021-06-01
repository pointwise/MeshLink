/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
 * It is provided freely for demonstration purposes only.
 * SEE THE WARRANTY DISCLAIMER AT THE BOTTOM OF THIS FILE.
 *
 ***************************************************************************/

#include "MeshAssociativity.h"

#include "MeshLinkWriter_xerces.h"
#include "MeshLinkWriter_xerces_c.h"

int
ML_createMeshLinkWriterXercesObj(MeshLinkWriterObj *writerObj)
{
    if (writerObj) {
        *writerObj = (MeshLinkWriterObj) new MeshLinkWriterXerces;
        if (*writerObj) {
            return 0;
        }
    }
    return 1;
}

void
ML_freeMeshLinkWriterXercesObj(MeshLinkWriterObj *writerObj)
{
    MeshLinkWriterXerces *writer = (MeshLinkWriterXerces *)*writerObj;
    if (writer) {
        delete writer;
        *writerObj = NULL;
    }
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
