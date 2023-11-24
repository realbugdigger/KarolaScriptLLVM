#include <memory>

class Expr;
class Stmt;

using UniqueExprPtr = std::unique_ptr<Expr>;
using UniqueStmtPtr = std::unique_ptr<Stmt>;