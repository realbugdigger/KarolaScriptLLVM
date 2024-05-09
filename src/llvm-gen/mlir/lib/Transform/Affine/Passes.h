#pragma once

#include "AffineFullUnroll.h"
#include "AffineFullUnrollPatternRewrite.h"

namespace mlir {

#define GEN_PASS_REGISTRATION
#include "Passes.h.inc"

}