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

