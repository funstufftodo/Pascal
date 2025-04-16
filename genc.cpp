#include "ast.h"

namespace AbstractSyntaxTree {

    inline std::string ConvCRelop(std::string s) {
        if (s == "=")
            return "==";
        if (s == "<>")
            return "!=";
        return s;
    }

    inline std::string ConvCAddop(std::string s) {
        if (s == "or")
            return "|";
        return s;
    }

    inline std::string ConvCMulop(std::string s) {
        if (s == "div")
            return "/";
        if (s == "and")
            return "&";
        if (s == "mod")
            return "%";
        return s;
    }

    std::string ASTNode::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string Identifiers::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string BasicTypeDecl::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ConstantDeclaration::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ConstantDeclarations::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string VariableDeclaration::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string VariableDeclarations::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string Parameter::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ParameterList::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string Variable::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string VariableList::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ExpressionFactor::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string NumFactor::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string InvFactor::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string VariableFactor::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string NotFactor::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string MulOpPart::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string Term::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string AddOpPart::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string SimpleExpression::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string RelPart::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string Expression::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ExpressionList::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string VarPart::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string VariableAssignStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ProcedureCallStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string SubCompoundStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string IfElseStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ForLoopStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ReadStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string WriteStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string StatementList::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string CompoundStatement::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string SubProgramHead::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string SubProgramBody::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string SubProgram::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string SubProgramDeclarations::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ProgramBody::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }

    std::string ProgramStruct::GenCCode(SymbolTable &table, bool isRef) {
        return "";
    }
}