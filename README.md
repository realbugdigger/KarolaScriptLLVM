# KarolaScript on LLVM (*project in progress*)

Moving my toy pl KarolaScript on LLVM infrastructure.

Idea is to make "tree-walking code generation"; so source code goes through lexer and parser and finally ends up in AST.

That AST was previouslly tree-walked by `Interpreter` class to eveluate statements and expressions.

Now `CodeGenVisitor` will perform tree-walking, but it will still need help from `Interpreter` since KarolaScript is dynamically typed language and LLVM IR has static types so we need to figure out the type of variable, parameter or return type.

Currently `CodeGenVisitor` reuses the code in `interpreter/Environment` and `interpreter/Resolver` but it will be better if we made new Environment and Resolver classes and put them in `llvm-gen` to provide loose coupling.

***

#### Notes to self:
- from `test.ll` (IR code) to `test.bc` (Machine-level bitcode) use `llvm-as`. `llvm-dis` can be used for dissasembling.

