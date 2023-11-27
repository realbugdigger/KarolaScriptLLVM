#include <memory>

class Expr;
class Stmt;

using UniqueExprPtr = std::shared_ptr<Expr>;
using UniqueStmtPtr = std::shared_ptr<Stmt>;