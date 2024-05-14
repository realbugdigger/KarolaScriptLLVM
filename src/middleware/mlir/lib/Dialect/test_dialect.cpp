#include "test_dialect.h"

#include "dialect.cpp.inc"

#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/OpImplementation.h>

namespace ksir {

/// Dialect initialization, the instance will be owned by the context. This is
/// the point of registration of types and operations for the dialect.
        void KS_Dialect::initialize() {
            addOperations<
                #define GET_OP_LIST
                #include "ops.cpp.inc"
            >();
        }

}

#define GET_OP_CLASSES
#include "ops.cpp.inc"