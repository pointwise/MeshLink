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

#include "MeshLinkParser_xerces.h"
#include "MeshLinkParser_xerces_c.h"

int
ML_createMeshLinkParserXercesObj(MeshLinkParserObj *parseObj)
{
    if (parseObj) {
        *parseObj = (MeshLinkParserObj) new MeshLinkParserXerces;
        if (*parseObj) {
            return 0;
        }
    }
    return 1;
}

void
ML_freeMeshLinkParserXercesObj(MeshLinkParserObj *parseObj)
{
    MeshLinkParserXerces *parser = (MeshLinkParserXerces *)*parseObj;
    if (parser) {
        delete parser;
        *parseObj = NULL;
    }
}

void
ML_setMeshLinkParserVerboseLevel(MeshLinkParserObj parseObj, MLINT level)
{
    MeshLinkParserXerces *parser = (MeshLinkParserXerces *)parseObj;
    if (parser) {
        parser->setVerboseLevel((int)level);
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
