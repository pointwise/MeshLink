/****************************************************************************
 *
 * Copyright (c) 2019-2020 Pointwise, Inc.
 * All rights reserved.
 *
 * This sample Pointwise source code is not supported by Pointwise, Inc.
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
 * DISCLAIMER:
 * TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, POINTWISE DISCLAIMS
 * ALL WARRANTIES, EITHER EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED
 * TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE, WITH REGARD TO THIS SCRIPT. TO THE MAXIMUM EXTENT PERMITTED
 * BY APPLICABLE LAW, IN NO EVENT SHALL POINTWISE BE LIABLE TO ANY PARTY
 * FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
 * WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF
 * BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS) ARISING OUT OF THE
 * USE OF OR INABILITY TO USE THIS SCRIPT EVEN IF POINTWISE HAS BEEN
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGES AND REGARDLESS OF THE
 * FAULT OR NEGLIGENCE OF POINTWISE.
 *
 ***************************************************************************/
