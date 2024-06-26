#include "PolyOps.h"

#include "mlir/Dialect/CommonFolders.h"
#include "llvm/Support/Debug.h"

namespace mlir {
        namespace poly {

            OpFoldResult ConstantOp::fold(ConstantOp::FoldAdaptor adaptor) {
                return adaptor.getCoefficients();
            }

            OpFoldResult AddOp::fold(AddOp::FoldAdaptor adaptor) {
                return constFoldBinaryOp<IntegerAttr, APInt>(
                        adaptor.getOperands(), [&](APInt a, APInt b) { return a + b; });
            }

            OpFoldResult SubOp::fold(SubOp::FoldAdaptor adaptor) {
                return constFoldBinaryOp<IntegerAttr, APInt>(
                        adaptor.getOperands(), [&](APInt a, APInt b) { return a - b; });
            }

            OpFoldResult MulOp::fold(MulOp::FoldAdaptor adaptor) {
                auto lhs = dyn_cast<DenseIntElementsAttr>(adaptor.getOperands()[0]);
                auto rhs = dyn_cast<DenseIntElementsAttr>(adaptor.getOperands()[1]);

                if (!lhs || !rhs)
                    return nullptr;

                auto degree = getResult().getType().cast<PolynomialType>().getDegreeBound();
                auto maxIndex = lhs.size() + rhs.size() - 1;

                SmallVector<APInt, 8> result;
                result.reserve(maxIndex);
                for (int i = 0; i < maxIndex; ++i) {
                    result.push_back(APInt((*lhs.begin()).getBitWidth(), 0));
                }

                int i = 0;
                for (auto lhsIt = lhs.value_begin<APInt>(); lhsIt != lhs.value_end<APInt>();
                     ++lhsIt) {
                    int j = 0;
                    for (auto rhsIt = rhs.value_begin<APInt>(); rhsIt != rhs.value_end<APInt>();
                         ++rhsIt) {
                        // index is modulo degree because poly's semantics are defined modulo x^N = 1.
                        result[(i + j) % degree] += *rhsIt * (*lhsIt);
                        ++j;
                    }
                    ++i;
                }

                return DenseIntElementsAttr::get(
                        RankedTensorType::get(static_cast<int64_t>(result.size()),
                                              IntegerType::get(getContext(), 32)),
                        result);
            }

            OpFoldResult FromTensorOp::fold(FromTensorOp::FoldAdaptor adaptor) {
                // Returns null if the cast failed, which corresponds to a failed fold.
                return dyn_cast<DenseIntElementsAttr>(adaptor.getInput());
            }

            LogicalResult EvalOp::verify() {
                return getPoint().getType().isSignlessInteger(32)
                       ? success()
                       : emitOpError("argument point must be a 32-bit integer");
            }

        }
}