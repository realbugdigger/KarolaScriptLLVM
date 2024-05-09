#pragma once

#include <mlir/Pass/Pass.h>

namespace mlir {

#define GEN_PASS_DECL_AFFINEFULLUNROLLPATTERNREWRITE
#include "Passes.h.inc"

}