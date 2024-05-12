#pragma once

#include "PolyDialect.h"
#include "PolyTypes.h"

#include <mlir/IR/BuiltinOps.h>
#include <mlir/IR/BuiltinTypes.h>
#include <mlir/IR/Dialect.h>

#define GET_OP_CLASSES
#include "PolyOps.h.inc"
