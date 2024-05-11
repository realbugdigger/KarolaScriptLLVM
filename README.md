# KarolaScript on LLVM (*project in progress*)

Idea is to make "tree-walking code generation"; so source code goes through lexer and parser and finally ends up in AST.

That AST was previouslly tree-walked by `Interpreter` class to eveluate statements and expressions.

```
.ks file --> AST --> LLVM IR --> assembly
          ^       ^           ^
        parser  lowering   backend (llc)
```

Currently `CodeGenVisitor` reuses the code in `interpreter/Environment` and `interpreter/Resolver` but it will be better if we made new Environment and Resolver classes and put them in `middleware` to provide loose coupling.

***

### Runtime type tagging

Since KarolaScript is dynamically typed language and LLVM IR is strongly types so we need to figure out the type of variable, parameter or return type.

Type tagging is a strategy ised in dynamically typed languages to determine the type of a variable at runtime.
In runtime type tagging, each object contains some sort of "tag" or "field" denoting what type it is.

In this project that can be `Object` class with little modification to support storing stuff like `llvm::Value` or `llvm::Type`.

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
