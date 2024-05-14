#ifndef KAROLASCRIPT_TEST_DIALECT_H
#define KAROLASCRIPT_TEST_DIALECT_H

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/Dialect.h>
#include <mlir/Interfaces/ControlFlowInterfaces.h>
#include <mlir/Interfaces/SideEffectInterfaces.h>

// Include the auto-generated header file containing the declaration of the
// serene's dialect.
#include "dialect.h.inc"

// Include the auto-generated header file containing the declarations of the
// serene's operations.
// for more on GET_OP_CLASSES: https://mlir.llvm.org/docs/OpDefinitions/
#define GET_OP_CLASSES

#include "ops.h.inc"

#endif //KAROLASCRIPT_TEST_DIALECT_H
