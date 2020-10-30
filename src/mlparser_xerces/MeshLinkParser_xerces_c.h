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

#ifndef MESH_LINK_PARSER_XERCES_C_API
#define MESH_LINK_PARSER_XERCES_C_API

#include "Types.h"

/**
 * \brief Allocates a new MeshLinkParserXerces Object.
 * @param[in,out] MeshLinkParserObj MeshLinkParserXerces object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
int  ML_createMeshLinkParserXercesObj(MeshLinkParserObj *parseObj);

/**
 * \brief Frees a MeshLinkParserXerces Object.
 * @param[in,out] MeshLinkParserObj MeshLinkParserXerces object
 * \returns ML_STATUS_OK / ML_STATUS_ERROR
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
void   ML_freeMeshLinkParserXercesObj(MeshLinkParserObj *parseObj);

/* Set the MeshLinkParser output message verbosity */

/**
 * \brief Set the MeshLinkParserXerces output message verbosity.
 * @param[in,out] MeshLinkParserObj MeshLinkParserXerces object
 * @param[in] level verbosity (0 = errors only, larger values generate more messages)
 */
/** @cond */ ML_EXTERN ML_STORAGE_CLASS /** @endcond */
void   ML_setMeshLinkParserVerboseLevel(MeshLinkParserObj parseObj,
    MLINT level);


#endif

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
