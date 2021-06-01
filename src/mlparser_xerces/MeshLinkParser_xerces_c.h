/****************************************************************************
 *
 * (C) 2021 Cadence Design Systems, Inc. All rights reserved worldwide.
 *
 * This sample source code is not supported by Cadence Design Systems, Inc.
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
 * This file is licensed under the Cadence Public License Version 1.0 (the
 * "License"), a copy of which is found in the included file named "LICENSE",
 * and is distributed "AS IS." TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE
 * LAW, CADENCE DISCLAIMS ALL WARRANTIES AND IN NO EVENT SHALL BE LIABLE TO
 * ANY PARTY FOR ANY DAMAGES ARISING OUT OF OR RELATING TO USE OF THIS FILE.
 * Please see the License for the full text of applicable terms.
 *
 ****************************************************************************/
