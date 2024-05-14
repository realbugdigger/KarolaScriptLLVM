# KarolaScript on LLVM (*project in progress*)


The initial idea was to make "tree-walking code generation"; so source code goes through lexer and parser and finally ends up in AST.

That AST will be walked and LLVM IR would be generated during walk. Look in `middleware/llvm-gen` for these files.

```
.ks file --> AST --> LLVM IR --> assembly
          ^       ^           ^
        parser  lowering   backend (llc)
```

While working on that I found about something called MLIR.

The framework built for language hackers which can be used for sequential lowering.

It will help us when writing language specific optimizations.

#### Usefull resorces while on LLVM and MLIR journey:
- https://github.com/j2kun/mlir-tutorial
- https://mlir.llvm.org/docs/Tutorials/Toy/Ch-1/
- https://medium.com/sniper-ai/mlir-tutorial-create-your-custom-dialect-lowering-to-llvm-ir-dialect-system-1-1f125a6a3008
- https://www.youtube.com/watch?v=Ij4LswX1tZU&list=PLlONLmJCfHTo9WYfsoQvwjsa5ZB6hjOG5&index=1
- https://llvm.org/docs/TableGen/ProgRef.html
- https://mlir.llvm.org/docs/DefiningDialects/Operations/
- https://mlir.llvm.org/docs/PassManagement/#operation-pass-static-filtering-by-op-type
- Interesting Passes and Dialects:
  - https://mlir.llvm.org/docs/Dialects/MathOps/
  - https://mlir.llvm.org/docs/Dialects/ArithOps/
  - https://mlir.llvm.org/docs/Dialects/Linalg/
  - https://mlir.llvm.org/docs/Dialects/ControlFlowDialect/


***

#### Notes to self:
- from `test.ll` (IR code) to `test.bc` (Machine-level bitcode) use `llvm-as`. `llvm-dis` can be used for dissasembling.

##### To get the final executable from `.ll`:
1. Compile LLVM IR to bitcode:
```
llvm-as yourfile.ll -o yourfile.bc
```
2. Compile bitcode to native assembly:
```
llc yourfile.bc -o yourfile.s
```
3. Link the assembly to an executable:
```
clang yourfile.s -o yourfile
```
