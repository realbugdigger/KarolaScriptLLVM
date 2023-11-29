# KarolaScript

Introducing KarolaScript, a toy programming language built to delve deeper into the intricate mechanics of parsers, interpreters, and compilers. 
KarolaScript, inspired by the fusion of Python and C, provides a procedurally-oriented, dynamically-typed programming environment. 
It distinguishes itself by recognizing classes and functions as first-class citizens; this allows for versatile usage scenarios such as assignment, passing as function parameters, and inclusion within complex data structures.

The present method of code execution in KarolaScript involves recursive evaluation of the AST. 
An upcoming plan includes developing a virtual machine and dedicated bytecode to enhance efficiency. 
Please note, the current feature list includes class and function operations, control flows, code blocks, and advanced parser capabilities. 
Updates will be made as progress continues.

### Grammar

```
program           → declaration* EOF ;

declaration       → classDecl | funDecl | varDecl | statement ;


classDecl         → "clazz" IDENTIFIER ( "<" IDENTIFIER )?
                    "{" member* "}" ;

member            → classMethodDecl | methodDecl ;
classMethodDecl   → "class" IDENTIFIER "(" parameters? ")" block ;
methodDecl        → function ;

funDecl           → "funct" function ;

function          → IDENTIFIER "(" parameters? ")" block ;
parameters        → IDENTIFIER ( "," IDENTIFIER )* ;

anonFunDecl       → "funct" anonFunction ;
anonFunction      → "(" parameters? ")" block ;


varDecl           → "let" IDENTIFIER ( "=" expression )? ";" ;



statement         → exprStmt | forStmt | ifStmt | printStmt | returnStmt | whileStmt | block | breakStmt;

returnStmt        → "return" expression? ";" ;

forStmt           → "for" "(" ( varDecl | exprStmt | ";" )
                    expression? ";"
                    expression? ")" statement ;

whileStmt         → "while" "(" expression ")" statement ;

exprStmt          → expression ";" ;

ifStmt            → "if" "(" expression ")" statement
                  ( "else" statement )? ;

printStmt         → "console" expression ";" ;
block             → "{" declaration* "}" ;
breakStmt         → "break" ";" ;



expression        → anonFunDecl | assignment ;

assignment        → ( call "." )? IDENTIFIER "=" logic_or | commaExpr ;

commaExpr         → logic_or ( "," logic_or )* ;

logic_or          → logic_and ( "or" logic_and )* ;
logic_and         → ternaryExpr ( "and" ternaryExpr )* ;

ternaryExpr       → equality ( "?" equality ":" ternaryExpr )? ;



equality          → comparison ( ( "!=" | "==" ) comparison )* ;
comparison        → term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
term              → factor ( ( "-" | "+" ) factor )* ;
factor            → unary ( ( "/" | "*" ) unary )* ;
unary             → unaryOperator* primary | call;

call              → primary ( "(" arguments? ")" | "." IDENTIFIER )* ;
arguments         → expression ( "," expression )* ;

primary           → errorPrimary | NUMBER | STRING | "true" | "false" | "nil" | "(" expression ")" | IDENTIFIER | "super" "." IDENTIFIER ;
errorPrimary      → binaryOperator expression ;
unaryOperator     → ( "!" | "-" )
binaryOperator    → ( "!=" | "==" | ">" | ">=" | "<" | "<=" | "-" | "+" | "/" | "*" )
```





KarolaScript is a toy programing language I made for my own better understanding of overall compilation process, compiler and interpreter knowledge.
KarolaScript is dynamicaly typed procedular objet oriented language. It looks like a mix of Python and C. In KarolaScript both classes and functions are considered first-class citizens!
That means language treats these entities (functions or classes) like any other objects and supports: assign operation to variables, passed as arguments to other functions, returned by other functions and can be included in data structures.

Currently the way KarolaScript code is being executed by recursively evaluating the AST.
More optimal way would be to make a virtual machine and bytecode for the language and the execute it on top of VM.
I'll add that when I have the time.

Currently supported features:
- provides REPL as well as single file interpretation
- assign operation
- classes
- inheritance
- functions
- anonymous functions
- function arguments
- static methods inside the class
- ternary operation
- binary operations
- MetaClasses ( currently one metaclass that initializes all other classes, so user can't currently create their own metaclass )
- if/else and for/while control flows and loops
- break and return statements
- blocks of code, with `{` and `}` braces
- Parser has ability to print out warning to a console if some variable was declared in certain scope but was never used or accessed (in inner or same level scope) !
- Parser will also report if some anonymous function was passed but never used

### Std lib

### Metaclasses

### Licence

### Future enhancements

### Blog link

### LLVM version link
