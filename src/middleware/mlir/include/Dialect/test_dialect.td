#ifndef KS_DIALECT
#define KS_DIALECT

include "mlir/IR/OpBase.td"
include "mlir/IR/OpAsmInterface.td"
include "mlir/IR/SymbolInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/CallInterfaces.td"
include "mlir/Interfaces/CastInterfaces.td"
include "mlir/Interfaces/SideEffectInterfaces.td"
include "mlir/Interfaces/ControlFlowInterfaces.td"
include "mlir/Interfaces/DataLayoutInterfaces.td"
include "mlir/Interfaces/VectorInterfaces.td"


// Dialect definition. It will directly generate the KS_Dialect class
def KS_Dialect : Dialect {
    let name = "karolaScript";
    let cppNamespace = "::ksir";
    let summary = "Primary IR of KarolaScript language.";
    let description = [{
        This dialect tries to map the special forms of a lisp into
        IR level operations.
    }];
}

// Base class for KarolaScript dialect operations. This operation inherits from the base
// `Op` class in OpBase.td, and provides:
//   * The parent dialect of the operation.
//   * The mnemonic for the operation, or the name without the dialect prefix.
//   * A list of traits for the operation.
class KS_Op<string mnemonic, list<OpTrait> traits = []> :
    Op<KS_Dialect, mnemonic, traits>;


// All of the types will extend this class.
class KS_Type<string name> : TypeDef<KS_Dialect, name> { }


def ValueOp: KS_Op<"value"> {

    let summary = "This operation represent a value";
    let description = [{
        ValueOp
    }];

    let arguments = (ins I64Attr:$value);
    let results = (outs I64);

    //let verifier = [{ return ::kssir::verify(*this); }];

    let builders = [
        OpBuilder<(ins "int":$value), [{
            // Build from fix 64 bit int
            build(odsBuilder, odsState, odsBuilder.getI64Type(), (uint64_t) value);
        }]>,

    ];
}



// TODO: Add the FunctionLike trait here and include its header file in dialect.h
def FnOp: KS_Op<"fn", [
  AffineScope, AutomaticAllocationScope,
  IsolatedFromAbove,
]> {

    let summary = "This operation is just a place holder for a function";
    let description = [{
        A place holder for an anonymous function. For example consider an expression
        like `(def a (fn (x) x))`, in this case we don't immediately create an anonymous
        function since we need to set the name and create the function later.
    }];

    let arguments = (ins StrAttr:$name,
                         DictionaryAttr:$args,
                         OptionalAttr<StrAttr>:$sym_visibility);

    let regions = (region AnyRegion:$body);
    let results = (outs I64);

}

def ReturnOp: KS_Op<"return", [NoSideEffect, HasParent<"FnOp">,
                                   ReturnLike, Terminator]> {

    let summary = "This operation marks the return value of a function";
    let description = [{
        ReturnOp
    }];

    let arguments = (ins AnyType:$operand);
    let assemblyFormat =
          [{ attr-dict $operand  `:` type($operand) }];
}

#endif // KS_DIALECT
