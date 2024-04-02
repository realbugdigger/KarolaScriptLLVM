# KarolaScript on LLVM (*project in progress*)

Idea is to make "tree-walking code generation"; so source code goes through lexer and parser and finally ends up in AST.

That AST was previouslly tree-walked by `Interpreter` class to eveluate statements and expressions.

```
.ks file --> AST --> LLVM IR --> assembly
          ^       ^           ^
        parser  lowering   backend (llc)
```

Now `CodeGenVisitor` will perform tree-walking, but it will still need help from `Interpreter` since KarolaScript is dynamically typed language and LLVM IR is strongly types so we need to figure out the type of variable, parameter or return type.

Currently `CodeGenVisitor` reuses the code in `interpreter/Environment` and `interpreter/Resolver` but it will be better if we made new Environment and Resolver classes and put them in `llvm-gen` to provide loose coupling.

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
