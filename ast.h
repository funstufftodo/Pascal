#ifndef AST
#define AST

#include <utility>
#include <vector>
#include <stack>
#include <memory>
#include "lexer.h"
#include "types.h"
#include "symboltable.h"

namespace AbstractSyntaxTree {
    struct ASTNode {
        virtual ~ASTNode() = default;

        virtual std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) = 0;

        virtual void Show() = 0;

        virtual void FormatShow(int level) = 0;

        virtual std::string GenCCode(SymbolTable &table, bool isRef);
    };

    struct AbstractSyntaxTree {
        SymbolTable symTable;
        std::unique_ptr<ASTNode> astRoot;

        AbstractSyntaxTree() = default;

        explicit AbstractSyntaxTree(std::unique_ptr<ASTNode> &&astRoot) : astRoot(std::move(astRoot)) {
        }

        bool Check();

        std::string GenCCode() {
            symTable.Reset();
            return astRoot->GenCCode(symTable, false);
        }
    };

    std::unique_ptr<ASTNode> GenOriAstNode(CompilerFront::Token &token);

    std::unique_ptr<ASTNode> GenAstNode(std::string expressionLeft, std::string expressionFirst,
                                        std::vector<std::unique_ptr<ASTNode>> &subNodes);

    template<typename T>
    inline std::unique_ptr<T> Unpack(std::unique_ptr<ASTNode> &node) {
        return UniquePtrCast<T>(node);
    }

    template<typename T>
    inline std::unique_ptr<ASTNode> Pack(std::unique_ptr<T> &node) {
        return UniquePtrCast<ASTNode>(node);
    }

    void logErrMsg(int line, int column, std::string msg);

    struct OriASTNode : public ASTNode {
        std::string content;
        std::string info;
        int Line = 0;
        int Column = 0;

        OriASTNode() = default;

        OriASTNode(std::string content, std::string info, int line, int column) : content(std::move(content)),
                                                                                  info(std::move(info)),
                                                                                  Line(line), Column(column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };

    struct ProgramStruct;
    struct ProgramHead;
    struct ProgramBody;
    struct Identifiers;
    struct ConstantDeclarations;
    struct VariableDeclarations;
    struct SubProgramDeclarations;
    struct CompoundStatement;
    struct ConstantDeclaration;
    struct VariableDeclaration;
    struct SubProgram;
    struct StatementList;
    struct BasicTypeDecl;
    struct TypeDeclarations;
    struct SubProgramHead;
    struct SubProgramBody;
    struct Statement;
    struct ParameterList;
    struct Parameter;


    struct ProgramStruct : ASTNode {
        std::unique_ptr<ProgramHead> programHead;
        std::unique_ptr<ProgramBody> programBody;

        ProgramStruct() = default;

        ProgramStruct(std::unique_ptr<ProgramHead> &&head, std::unique_ptr<ProgramBody> &&body) : programHead(
                std::move(head)), programBody(std::move(body)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ProgramHead : ASTNode {
        std::string name;
        std::unique_ptr<Identifiers> identifiers;

        ProgramHead() = default;

        ProgramHead(std::string name, std::unique_ptr<Identifiers> &&ids) : name(std::move(name)),
                                                                            identifiers(std::move(ids)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };

    struct ProgramBody : ASTNode {
        std::unique_ptr<ConstantDeclarations> constantDeclarations;
        std::unique_ptr<VariableDeclarations> variableDeclarations;
        std::unique_ptr<SubProgramDeclarations> subProgramDeclarations;
        std::unique_ptr<CompoundStatement> compoundStatemnet;

        ProgramBody() = default;

        ProgramBody(std::unique_ptr<ConstantDeclarations> &&constantDeclarations,
                    std::unique_ptr<VariableDeclarations> &&variableDeclarations,
                    std::unique_ptr<SubProgramDeclarations> &&subProgramDeclarations,
                    std::unique_ptr<CompoundStatement> &&compoundStatemnet) : constantDeclarations(
                std::move(constantDeclarations)),
                                                                              variableDeclarations(
                                                                                      std::move(variableDeclarations)),
                                                                              subProgramDeclarations(std::move(
                                                                                      subProgramDeclarations)),
                                                                              compoundStatemnet(
                                                                                      std::move(compoundStatemnet)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ConstantDeclarations : public ASTNode {
        std::vector<std::unique_ptr<ConstantDeclaration>> constantDeclarations;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SubProgramDeclarations : public ASTNode {
        std::vector<std::unique_ptr<SubProgram>> subPrograms;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct CompoundStatement : ASTNode {
        std::unique_ptr<StatementList> statementList;

        CompoundStatement() = default;

        explicit CompoundStatement(std::unique_ptr<StatementList> &&statementList) : statementList(
                std::move(statementList)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Identifiers : public ASTNode {
        std::vector<std::string> identifiers;
        std::vector<int> Lines;
        std::vector<int> Columns;

        Identifiers() = default;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct VariableDeclarations : public ASTNode {
        std::vector<std::unique_ptr<VariableDeclaration>> variableDeclarations;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ConstantDeclaration : public ASTNode {
        std::string name;
        std::unique_ptr<BasicTypeDecl> type;
        std::string content;
        int Line = 0;
        int Column = 0;

        ConstantDeclaration() = default;

        ConstantDeclaration(std::string name,
                            std::unique_ptr<BasicTypeDecl> &&type,
                            std::string content,
                            int Line,
                            int Column)
                : name(std::move(name)), type(std::move(type)), content(std::move(content)), Line(Line),
                  Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct VariableDeclaration : public ASTNode {
        std::unique_ptr<TypeDeclarations> type;
        std::unique_ptr<Identifiers> identifiers;

        VariableDeclaration() = default;

        VariableDeclaration(std::unique_ptr<TypeDeclarations> &&type,
                            std::unique_ptr<Identifiers> &&identifiers) : type(std::move(type)),
                                                                          identifiers(std::move(identifiers)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SubProgram : public ASTNode {
        std::unique_ptr<SubProgramHead> head;
        std::unique_ptr<SubProgramBody> body;

        SubProgram() = default;

        SubProgram(std::unique_ptr<SubProgramHead> &&head,
                   std::unique_ptr<SubProgramBody> &&body) : head(std::move(head)), body(std::move(body)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct StatementList : public ASTNode {
        std::vector<std::unique_ptr<Statement>> statements; //ELEM MAY NULL

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SubProgramHead : public ASTNode {
        std::string name;
        std::unique_ptr<ParameterList> parameters;
        std::unique_ptr<BasicTypeDecl> returnType; //NAY NULL
        int Line = 0;
        int Column = 0;

        SubProgramHead() = default;

        SubProgramHead(std::string name,
                       std::unique_ptr<ParameterList> &&parameters,
                       std::unique_ptr<BasicTypeDecl> &&returnType,
                       int Line,
                       int Column)
                : name(std::move(name)), parameters(std::move(parameters)), returnType(std::move(returnType)),
                  Line(Line),
                  Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SubProgramBody : public ASTNode {
        std::unique_ptr<ConstantDeclarations> constantDeclarations;
        std::unique_ptr<VariableDeclarations> variableDeclarations;
        std::unique_ptr<CompoundStatement> compoundStatement;

        SubProgramBody() = default;

        SubProgramBody(std::unique_ptr<ConstantDeclarations> &&constantDeclarations,
                       std::unique_ptr<VariableDeclarations> &&variableDeclarations,
                       std::unique_ptr<CompoundStatement> &&compoundStatement)
                : constantDeclarations(std::move(constantDeclarations)),
                  variableDeclarations(std::move(variableDeclarations)),
                  compoundStatement(std::move(compoundStatement)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ParameterList : public ASTNode {
        std::vector<std::unique_ptr<Parameter>> parameters;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Parameter : public ASTNode {
        bool isRef = false;
        std::unique_ptr<BasicTypeDecl> type;
        std::unique_ptr<Identifiers> identifiers;

        Parameter() = default;

        Parameter(bool isRef,
                  std::unique_ptr<BasicTypeDecl> &&type,
                  std::unique_ptr<Identifiers> &&identifiers) : isRef(isRef), type(std::move(type)),
                                                                identifiers(std::move(identifiers)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Range : public ASTNode {
        int l{};
        int r{};
        int Line{};
        int Column{};

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        Range() = default;

        Range(int l, int r, int Line, int Column) : l(l), r(r), Line(Line), Column(Column) {}

        void Show() override;

        void FormatShow(int level) override;
    };

    struct Ranges : public ASTNode {
        std::vector<std::unique_ptr<Range>> ranges;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };

    struct TypeDeclarations : ASTNode {
        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };

    struct BasicTypeDecl : public TypeDeclarations {
        std::string basicType;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ArrayTypeDecl : public TypeDeclarations {
        std::unique_ptr<Ranges> ranges;
        std::unique_ptr<BasicTypeDecl> type;

        ArrayTypeDecl() = default;

        ArrayTypeDecl(std::unique_ptr<Ranges> &&ranges,
                      std::unique_ptr<BasicTypeDecl> &&type) : ranges(std::move(ranges)), type(std::move(type)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };


    struct VarPart;

    struct Variable : public ASTNode {
        std::string name;
        std::unique_ptr<VarPart> varPart;
        bool isAssignLeft = false;
        int Line = 0;
        int Column = 0;

        Variable() = default;

        Variable(std::string name,
                 std::unique_ptr<VarPart> &&varPart,
                 int Line,
                 int Column)
                : name(std::move(name)), varPart(std::move(varPart)), isAssignLeft(false), Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct VariableList : public ASTNode {
        std::vector<std::unique_ptr<Variable>> variables;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Factor : public ASTNode {
        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };

    struct Expression;

    struct ExpressionFactor : public Factor {
        std::unique_ptr<Expression> expression;

        ExpressionFactor() = default;

        explicit ExpressionFactor(std::unique_ptr<Expression> &&expression) : expression(std::move(expression)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;

    };

    struct NumFactor : public Factor {
        std::string val;
        std::string type;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct InvFactor : public Factor {
        std::unique_ptr<Factor> subFactor;
        int Line = 0;
        int Column = 0;

        InvFactor() = default;

        InvFactor(std::unique_ptr<Factor> &&subFactor, int Line, int Column)
                : subFactor(std::move(subFactor)), Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct PosFactor : public Factor {
        std::unique_ptr<Factor> subFactor;
        int Line = 0;
        int Column = 0;

        PosFactor() = default;

        PosFactor(std::unique_ptr<Factor> &&subFactor, int Line, int Column)
                : subFactor(std::move(subFactor)), Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct VariableFactor : public Factor {
        std::unique_ptr<Variable> variable;

        VariableFactor() = default;

        explicit VariableFactor(std::unique_ptr<Variable> &&variable) : variable(std::move(variable)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct NotFactor : public Factor {
        std::unique_ptr<Factor> subFactor;
        int Line = 0;
        int Column = 0;

        NotFactor() = default;

        NotFactor(std::unique_ptr<Factor> &&subFactor, int Line, int Column)
                : subFactor(std::move(subFactor)), Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Term;

    struct MulOpPart : public ASTNode {
        std::string mulOp;
        std::unique_ptr<Factor> secondFactor;
        std::unique_ptr<MulOpPart> followPart; //MAY NULL
        int Line = 0;
        int Column = 0;

        MulOpPart() = default;

        MulOpPart(std::string mulOp,
                  std::unique_ptr<Factor> &&secondFactor,
                  std::unique_ptr<MulOpPart> &&followPart,
                  int Line,
                  int Column) : mulOp(std::move(mulOp)), secondFactor(std::move(secondFactor)),
                                followPart(std::move(followPart)), Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Term : public ASTNode {
        std::unique_ptr<Factor> firstFactor;
        std::unique_ptr<MulOpPart> mulOpPart; //MAY NULL

        Term() = default;

        Term(std::unique_ptr<Factor> &&firstFactor,
             std::unique_ptr<MulOpPart> &&mulOpPart) : firstFactor(std::move(firstFactor)),
                                                       mulOpPart(std::move(mulOpPart)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SimpleExpression;

    struct AddOpPart : public ASTNode {
        std::string addOp;
        std::unique_ptr<Term> secondTerm;
        std::unique_ptr<AddOpPart> followPart; //MAY NULL
        int Line = 0;
        int Column = 0;

        AddOpPart() = default;

        AddOpPart(std::string addOp,
                  std::unique_ptr<Term> &&secondTerm,
                  std::unique_ptr<AddOpPart> &&followPart,
                  int Line,
                  int Column) : addOp(std::move(addOp)), secondTerm(std::move(secondTerm)),
                                followPart(std::move(followPart)),
                                Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SimpleExpression : public ASTNode {
        std::unique_ptr<Term> firstTerm;
        std::unique_ptr<AddOpPart> addOpPart; //MAY NULL

        SimpleExpression() = default;

        SimpleExpression(std::unique_ptr<Term> &&firstTerm,
                         std::unique_ptr<AddOpPart> &&addOpPart)
                : firstTerm(std::move(firstTerm)), addOpPart(std::move(addOpPart)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct RelPart : public ASTNode {
        std::string relop;
        std::unique_ptr<SimpleExpression> secondExpression;
        int Line = 0;
        int Column = 0;

        RelPart() = default;

        RelPart(std::string relop,
                std::unique_ptr<SimpleExpression> &&secondExpression,
                int Line,
                int Column)
                : relop(std::move(relop)), secondExpression(std::move(secondExpression)), Line(Line), Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct Expression : public ASTNode {
        std::unique_ptr<SimpleExpression> firstExpression;
        std::unique_ptr<RelPart> relPart; //MAY NULL

        Expression() = default;

        Expression(std::unique_ptr<SimpleExpression> &&firstExpression,
                   std::unique_ptr<RelPart> &&relPart) : firstExpression(std::move(firstExpression)),
                                                         relPart(std::move(relPart)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ExpressionList : public ASTNode {
        std::vector<std::unique_ptr<Expression>> expressions;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct VarPart : public ASTNode {
        bool isProcedureCall = false;
        std::vector<bool> argIsRef;
        std::vector<int> indexOffset;
        std::unique_ptr<ExpressionList> expressionList;

        VarPart() = default;

        VarPart(bool isProcedureCall,
                std::unique_ptr<ExpressionList> &&expressionList) : isProcedureCall(isProcedureCall),
                                                                    expressionList(std::move(expressionList)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct CompoundStatement;

    struct Statement : public ASTNode {
        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;
    };

    struct VariableAssignStatement : public Statement {
        std::unique_ptr<Variable> variable;
        std::unique_ptr<Expression> expression;

        VariableAssignStatement() = default;

        VariableAssignStatement(std::unique_ptr<Variable> &&variable,
                                std::unique_ptr<Expression> &&expression) : variable(std::move(variable)),
                                                                            expression(std::move(expression)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ProcedureCallStatement : public Statement {
        std::unique_ptr<Variable> variable;

        ProcedureCallStatement() = default;

        explicit ProcedureCallStatement(std::unique_ptr<Variable> &&variable) : variable(std::move(variable)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct BreakStatement : public Statement {
        BreakStatement() = default;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct SubCompoundStatement : public Statement {
        std::unique_ptr<CompoundStatement> compoundStatement;

        SubCompoundStatement() = default;

        explicit SubCompoundStatement(std::unique_ptr<CompoundStatement> &&compoundStatement) : compoundStatement(
                std::move(compoundStatement)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct IfElseStatement : public Statement {
        std::unique_ptr<Expression> ifExpression;
        std::unique_ptr<Statement> thenStatement; //MAY NULL
        std::unique_ptr<Statement> elseStatement; //MAY NULL

        IfElseStatement() = default;

        IfElseStatement(std::unique_ptr<Expression> &&ifExpression,
                        std::unique_ptr<Statement> &&thenStatement,
                        std::unique_ptr<Statement> &&elseStatement)
                : ifExpression(std::move(ifExpression)), thenStatement(std::move(thenStatement)),
                  elseStatement(std::move(elseStatement)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct WhileStatement : public Statement {
        std::unique_ptr<Expression> initExpression;
        std::unique_ptr<Statement> loopStatement; //MAY NULL
        WhileStatement() = default;

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        WhileStatement(
                std::unique_ptr<Expression> &&initExpression,
                std::unique_ptr<Statement> &&loopStatement
        ) :
                initExpression(std::move(initExpression)),
                loopStatement(std::move(loopStatement)) {}

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ForLoopStatement : public Statement {
        std::string counter;
        std::unique_ptr<Expression> initExpression;
        std::unique_ptr<Expression> termiExpression;
        std::unique_ptr<Statement> loopStatement; //MAY NULL
        int Line = 0;
        int Column = 0;

        ForLoopStatement() = default;

        ForLoopStatement(std::string counter,
                         std::unique_ptr<Expression> &&initExpression,
                         std::unique_ptr<Expression> &&termiExpression,
                         std::unique_ptr<Statement> &&loopStatement,
                         int Line,
                         int Column) : counter(std::move(counter)),
                                       initExpression(std::move(initExpression)),
                                       termiExpression(std::move(termiExpression)),
                                       loopStatement(std::move(loopStatement)),
                                       Line(Line),
                                       Column(Column) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct ReadStatement : public Statement {
        std::unique_ptr<VariableList> variableList;

        ReadStatement() = default;

        explicit ReadStatement(std::unique_ptr<VariableList> &&variableList) : variableList(std::move(variableList)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

    struct WriteStatement : public Statement {
        std::unique_ptr<ExpressionList> expressionList;
        std::string typeStr;

        WriteStatement() = default;

        explicit WriteStatement(std::unique_ptr<ExpressionList> &&expressionList) : expressionList(
                std::move(expressionList)) {}

        std::unique_ptr<TypeBase> Check(SymbolTable &table, bool &ok) override;

        void Show() override;

        void FormatShow(int level) override;

        std::string GenCCode(SymbolTable &table, bool isRef) override;
    };

}

#endif
