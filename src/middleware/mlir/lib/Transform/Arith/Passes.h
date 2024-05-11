#ifndef LIB_TRANSFORM_ARITH_PASSES_H_
#define LIB_TRANSFORM_ARITH_PASSES_H_

#include "MulToAdd.h"

namespace mlir {

#define GEN_PASS_REGISTRATION
#include "Passes.h.inc"

}

#endif  // LIB_TRANSFORM_ARITH_PASSES_H_