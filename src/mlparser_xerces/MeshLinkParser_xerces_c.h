#ifndef MESH_LINK_PARSER_XERCES_C_API
#define MESH_LINK_PARSER_XERCES_C_API

#include "Types.h"


/* Allocate a new MeshLinkParserXercesObj */
ML_EXTERN ML_STORAGE_CLASS
int  ML_createMeshLinkParserXercesObj(MeshLinkParserObj *parseObj);

/* Free MeshLinkParserXercesObj memory */
ML_EXTERN ML_STORAGE_CLASS
void   ML_freeMeshLinkParserXercesObj(MeshLinkParserObj *parseObj);



#endif
