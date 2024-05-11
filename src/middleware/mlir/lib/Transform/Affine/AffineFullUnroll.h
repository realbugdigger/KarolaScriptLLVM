#pragma once

#include <mlir/Dialect/Affine/IR/AffineOps.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Pass/Pass.h>

// https://mlir.llvm.org/docs/Dialects/Affine/

namespace mlir {
    class AffineFullUnrollPass
            : public PassWrapper<AffineFullUnrollPass, mlir::OperationPass<mlir::func::FuncOp>> {
    private:
        void runOnOperation() override;

        StringRef getArgument() const final { return "affine-full-unroll"; }

        StringRef getDescription() const final {
            return "Fully unroll all affine loops";
        }
    };

    class AffineFullUnrollPassAsPatternRewrite
            : public PassWrapper<AffineFullUnrollPassAsPatternRewrite,
                    OperationPass<mlir::func::FuncOp>> {
    private:
        void runOnOperation() override;

        StringRef getArgument() const final { return "affine-full-unroll-rewrite"; }

        StringRef getDescription() const final {
            return "Fully unroll all affine loops using pattern rewrite engine";
        }
    };
}