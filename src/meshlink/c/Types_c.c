#include "Types.h"

#if defined(DEBUG)
#include <assert.h>
#endif


/* Helper function for trapping assertions */
void ml_assert(int val) {
#if defined(DEBUG)
    if (!val) {
        assert(val);
    }
#endif
}

/* Python wrapper test functions */
void printVec3(MLVector3D vec);
void printVec2(MLVector2D vec);

void addVec3(MLVector3D vec1, MLVector3D vec2);

