#include "ast.h"

namespace AbstractSyntaxTree {
    namespace {
        void CheckDuplicateIdentifier(SymbolTable &table, bool &ok, const std::string &name,
                                      int line, int col, const std::string &context = "") {
            if (table.SymbolAtCurScope(name)) {
                ok = false;
                logErrMsg(line, col, "Error: Duplicate identifier \"" + name + "\"" +
                                     (context.empty() ? "" : " in " + context));
            }
        }

        void CheckIdentifierExists(SymbolTable &table, bool &ok, const std::string &name,
                                   int line, int col, const std::string &context = "") {
            bool has;
            int layer;
            table.FindSymbol(name, has, layer);
            if (!has) {
                ok = false;
                logErrMsg(line, col, "Error: Identifier not found \"" + name + "\"" +
                                     (context.empty() ? "" : " in " + context));
            }
        }

        void CheckVariableExpected(SymbolTable &table, bool &ok, const std::string &name,
                                   int line, int col, const std::string &context = "") {
            bool has;
            int layer;
            const auto &item = table.FindSymbol(name, has, layer);
            if (has && item->second.isConstant) {
                ok = false;
                logErrMsg(line, col, "Error: Variable identifier expected" +
                                     (context.empty() ? "" : " in " + context));
            }
        }

        void CheckAssignCompatible(std::unique_ptr<TypeBase> &targetType,
                                   std::unique_ptr<TypeBase> &sourceType, bool &ok,
                                   int line, int col, const std::string &context = "") {
            std::string errMsg;
            if (!targetType->AssignCompatible(sourceType->Copy(), errMsg)) {
                ok = false;
                logErrMsg(line, col, "Error: " + errMsg +
                                     (context.empty() ? "" : " in " + context));
            }
        }

        std::unique_ptr<TypeBase> CheckBinaryOpCompatible(std::unique_ptr<TypeBase> type1,
                                                          std::unique_ptr<TypeBase> type2,
                                                          const std::string &op, bool &ok,
                                                          int line, int col,
                                                          const std::string &context = "") {
            std::string errMsg;
            auto result = type1->CalcType(std::move(type2), op, ok, errMsg);
            if (!errMsg.empty()) {
                logErrMsg(line, col, "Error: " + errMsg +
                                     (context.empty() ? "" : " in " + context));
            }
            return result;
        }
    }

    bool AbstractSyntaxTree::Check() {
        bool ok = true;
        astRoot->Check(symTable, ok);
        return ok;
    }

    std::unique_ptr<TypeBase> OriASTNode::Check(SymbolTable &table, bool &ok) {
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> Identifiers::Check(SymbolTable &table, bool &ok) {
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> Range::Check(SymbolTable &table, bool &ok) {
        if (l > r) {
            ok = false;
            logErrMsg(Line, Column, "Error: Upper bound of range is less than lower bound");
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> Ranges::Check(SymbolTable &table, bool &ok) {
        for (auto &range: ranges)
            range->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> TypeDeclarations::Check(SymbolTable &table, bool &ok) {
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> BasicTypeDecl::Check(SymbolTable &table, bool &ok) {
        return GenTypeByStr(basicType);
    }

    std::unique_ptr<TypeBase> ArrayTypeDecl::Check(SymbolTable &table, bool &ok) {
        ranges->Check(table, ok);
        std::vector<std::pair<int, int>> arrRanges;
        for (auto &range: ranges->ranges)
            arrRanges.push_back(std::pair<int, int>(range->l, range->r));

        TypeBase *arrType = new ArrayType(arrRanges, type->Check(table, ok)->Copy());
        return std::unique_ptr<TypeBase>(arrType);
    }

    std::unique_ptr<TypeBase> ConstantDeclaration::Check(SymbolTable &table, bool &ok) {
        CheckDuplicateIdentifier(table, ok, name, Line, Column, "constant declaration");
        if (ok) {
            TypeBase *lValueType = new LValueType(type->Check(table, ok));
            table.InsertSymbol(name, std::unique_ptr<TypeBase>(lValueType), true, content);
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ConstantDeclarations::Check(SymbolTable &table, bool &ok) {
        for (auto &declaration: constantDeclarations)
            declaration->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> VariableDeclaration::Check(SymbolTable &table, bool &ok) {
        auto tp(type->Check(table, ok));
        identifiers->Check(table, ok);
        for (int i = 0; i < identifiers->identifiers.size(); i++) {
            std::string id = identifiers->identifiers[i];
            CheckDuplicateIdentifier(table, ok, id, identifiers->Lines[i], identifiers->Columns[i],
                                     "variable declaration");
            if (ok) {
                TypeBase *lValueType = new LValueType(tp->Copy());
                table.InsertSymbol(id, std::unique_ptr<TypeBase>(lValueType), false, "");
            }
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> VariableDeclarations::Check(SymbolTable &table, bool &ok) {
        for (auto &declaration: variableDeclarations)
            declaration->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> Parameter::Check(SymbolTable &table, bool &ok) {
        identifiers->Check(table, ok);
        TypeBase *ret;
        if (isRef) {
            ret = new RefType(type->Check(table, ok));
        } else {
            ret = new LValueType(type->Check(table, ok));
        }
        return std::unique_ptr<TypeBase>(ret);
    }

    std::unique_ptr<TypeBase> ParameterList::Check(SymbolTable &table, bool &ok) {
        std::vector<std::unique_ptr<TypeBase>> types;
        if(!parameters.empty() && parameters[0] != nullptr) {
            for (auto &parameter: parameters) {
                auto pType(parameter->Check(table, ok));
                auto &identifiers = parameter->identifiers;
                for (int i = 0; i < identifiers->identifiers.size(); i++) {
                    std::string id = identifiers->identifiers[i];
                    CheckDuplicateIdentifier(table, ok, id, identifiers->Lines[i], identifiers->Columns[i],
                                             "parameter list");
                    if (ok) {
                        table.InsertSymbol(id, pType->Copy(), false, "");
                    }
                    types.push_back(pType->Copy());
                }
            }
            TypeBase *tupleType = new TupleType(std::move(types));
            return std::unique_ptr<TypeBase>(tupleType);
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> Variable::Check(SymbolTable &table, bool &ok) {
        bool has;
        int layer;
        auto symbolIt = table.FindSymbol(name, has, layer);

        if (!has) {
            ok = false;
            logErrMsg(Line, Column, std::string("Error: Identifier not found \"") + name + "\"");

            TypeBase *ret = new RValueType(GenType(VOID));
            return std::unique_ptr<TypeBase>(ret);
        }
        std::unique_ptr<TypeBase> &type = symbolIt->second.type;
        auto targetType(((WrapperType *) type.get())->DeWrap());

        std::string errMsg;

        if (targetType->GetTypeId() == FUNC) {
            if (varPart != nullptr) {
                auto argTypes(UniquePtrCast<FuncType>(targetType)->GetArgTypes());
                for (auto &argType: argTypes)
                    varPart->argIsRef.push_back(argType->GetTypeId() == REF);

                if (varPart->isProcedureCall) {
                    auto ret = type->CalcFuncType(UniquePtrCast<TupleType>(varPart->Check(table, ok)), ok, errMsg);
                    if (!errMsg.empty())
                        logErrMsg(Line, Column, errMsg);
                    return ret;
                }
                auto ret = type->CalcArrayType(UniquePtrCast<TupleType>(varPart->Check(table, ok)), ok, errMsg);
                if (!errMsg.empty())
                    logErrMsg(Line, Column, errMsg);
                return ret;
            }

            if (isAssignLeft && table.SymbolAtCurScope(name)) {
                return ((FuncType *) targetType.get())->RetAsLValue();
            }

            TupleType *emptyTuple = new TupleType();
            auto ret = type->CalcFuncType(std::unique_ptr<TupleType>(emptyTuple), ok, errMsg);
            if (!errMsg.empty())
                logErrMsg(Line, Column, errMsg);
            return ret;
        }
        if (varPart != nullptr) {
            if (varPart->isProcedureCall) {
                auto ret = type->CalcFuncType(UniquePtrCast<TupleType>(varPart->Check(table, ok)), ok, errMsg);
                if (!errMsg.empty())
                    logErrMsg(Line, Column, errMsg);
                return ret;
            }

            if (targetType->GetTypeId() == ARRAY)
                varPart->indexOffset = ((ArrayType *) targetType.get())->GetOffset();
            auto ret = type->CalcArrayType(UniquePtrCast<TupleType>(varPart->Check(table, ok)), ok, errMsg);
            if (!errMsg.empty())
                logErrMsg(Line, Column, errMsg);
            return ret;
        }
        return type->Copy();
    }

    std::unique_ptr<TypeBase> VariableList::Check(SymbolTable &table, bool &ok) {
        std::vector<std::unique_ptr<TypeBase>> types;
        for (auto & variable : variables) {
            auto type(variable->Check(table, ok));
            CheckVariableExpected(table, ok, variable->name, variable->Line, variable->Column, "variable list");

            if ((type->GetTypeId() != LVALUE) && (type->GetTypeId() != REF)) {
                ok = false;
                logErrMsg(variable->Line, variable->Column, "Error: lvalue/ref expected");
            }
            types.push_back(std::move(type));
        }

        TypeBase *tupleType = new TupleType(std::move(types));
        return std::unique_ptr<TypeBase>(tupleType);
    }

    std::unique_ptr<TypeBase> Factor::Check(SymbolTable &table, bool &ok) {
        TypeBase *ret = new RValueType(GenType(VOID));
        return std::unique_ptr<TypeBase>(ret);
    }

    std::unique_ptr<TypeBase> ExpressionFactor::Check(SymbolTable &table, bool &ok) {
        return expression->Check(table, ok);
    }

    std::unique_ptr<TypeBase> NumFactor::Check(SymbolTable &table, bool &ok) {
        TypeBase *ret = new RValueType(GenTypeByStr(type));
        return std::unique_ptr<TypeBase>(ret);
    }

    std::unique_ptr<TypeBase> InvFactor::Check(SymbolTable &table, bool &ok) {
        TypeBase *fac = new RValueType(subFactor->Check(table, ok));
        std::string errMsg;
        auto ret = fac->CalcType(GenType(INTEGER), "-", ok, errMsg);
        if (!errMsg.empty())
            logErrMsg(Line, Column, errMsg);
        return ret;
    }

    std::unique_ptr<TypeBase> PosFactor::Check(SymbolTable &table, bool &ok) {
        TypeBase *fac = new RValueType(subFactor->Check(table, ok));
        std::string errMsg;
        auto ret = fac->CalcType(GenType(INTEGER), "+", ok, errMsg);
        if (!errMsg.empty())
            logErrMsg(Line, Column, errMsg);
        return ret;
    }

    std::unique_ptr<TypeBase> VariableFactor::Check(SymbolTable &table, bool &ok) {
        return variable->Check(table, ok);
    }

    std::unique_ptr<TypeBase> NotFactor::Check(SymbolTable &table, bool &ok) {
        TypeBase *fac = new RValueType(subFactor->Check(table, ok));
        std::string errMsg;
        auto ret = fac->CalcType(GenType(INTEGER), "not", ok, errMsg);
        if (!errMsg.empty())
            logErrMsg(Line, Column, errMsg);
        return ret;
    }

    std::unique_ptr<TypeBase> MulOpPart::Check(SymbolTable &table, bool &ok) {
        if (followPart == nullptr) {
            return secondFactor->Check(table, ok);
        }
        return CheckBinaryOpCompatible(followPart->Check(table, ok), secondFactor->Check(table, ok),
                                       followPart->mulOp, ok, followPart->Line, followPart->Column,
                                       "multiplication operation");
    }

    std::unique_ptr<TypeBase> Term::Check(SymbolTable &table, bool &ok) {
        if (mulOpPart == nullptr) {
            return firstFactor->Check(table, ok);
        }
        return CheckBinaryOpCompatible(mulOpPart->Check(table, ok), firstFactor->Check(table, ok),
                                       mulOpPart->mulOp, ok, mulOpPart->Line, mulOpPart->Column, "term");
    }

    std::unique_ptr<TypeBase> AddOpPart::Check(SymbolTable &table, bool &ok) {
        if (followPart == nullptr) {
            return secondTerm->Check(table, ok);
        }
        return CheckBinaryOpCompatible(followPart->Check(table, ok), secondTerm->Check(table, ok),
                                       followPart->addOp, ok, followPart->Line, followPart->Column,
                                       "addition operation");
    }

    std::unique_ptr<TypeBase> SimpleExpression::Check(SymbolTable &table, bool &ok) {
        if (addOpPart == nullptr) {
            return firstTerm->Check(table, ok);
        }
        return CheckBinaryOpCompatible(addOpPart->Check(table, ok), firstTerm->Check(table, ok),
                                       addOpPart->addOp, ok, addOpPart->Line, addOpPart->Column, "simple expression");
    }

    std::unique_ptr<TypeBase> RelPart::Check(SymbolTable &table, bool &ok) {
        return secondExpression->Check(table, ok);
    }

    std::unique_ptr<TypeBase> Expression::Check(SymbolTable &table, bool &ok) {
        if (relPart == nullptr) {
            return firstExpression->Check(table, ok);
        }
        return CheckBinaryOpCompatible(firstExpression->Check(table, ok), relPart->Check(table, ok),
                                       relPart->relop, ok, relPart->Line, relPart->Column, "expression");
    }

    std::unique_ptr<TypeBase> ExpressionList::Check(SymbolTable &table, bool &ok) {
        std::vector<std::unique_ptr<TypeBase>> types;
        if(expressions[0] != nullptr) {
            for (auto &expression: expressions) {
                types.push_back(expression->Check(table, ok));
            }
            TypeBase *tupleType = new TupleType(std::move(types));
            return std::unique_ptr<TypeBase>(tupleType);
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> VarPart::Check(SymbolTable &table, bool &ok) {
        return expressionList->Check(table, ok);
    }

    std::unique_ptr<TypeBase> Statement::Check(SymbolTable &table, bool &ok) {
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> VariableAssignStatement::Check(SymbolTable &table, bool &ok) {
        auto varType(variable->Check(table, ok));
        CheckVariableExpected(table, ok, variable->name, variable->Line, variable->Column, "assignment");
        auto expressionType = expression->Check(table, ok);
        CheckAssignCompatible(varType, expressionType, ok, variable->Line, variable->Column, "assignment");
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ProcedureCallStatement::Check(SymbolTable &table, bool &ok) {
        variable->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> SubCompoundStatement::Check(SymbolTable &table, bool &ok) {
        compoundStatement->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> IfElseStatement::Check(SymbolTable &table, bool &ok) {
        ifExpression->Check(table, ok);
        if (thenStatement != nullptr)
            thenStatement->Check(table, ok);
        if (elseStatement != nullptr)
            elseStatement->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ForLoopStatement::Check(SymbolTable &table, bool &ok) {
        CheckIdentifierExists(table, ok, counter, Line, Column, "for loop");
        if (ok) {
            bool has;
            int layer;
            const auto &item = table.FindSymbol(counter, has, layer);
            CheckVariableExpected(table, ok, counter, Line, Column, "for loop");

            auto &itemType = item->second.type;
            auto valType = initExpression->Check(table, ok);
            CheckAssignCompatible(itemType, valType, ok, Line, Column, "for loop initialization");
        }
        termiExpression->Check(table, ok);
        if (loopStatement != nullptr)
            loopStatement->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ReadStatement::Check(SymbolTable &table, bool &ok) {
        variableList->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> WriteStatement::Check(SymbolTable &table, bool &ok) {
        auto types = UniquePtrCast<TupleType>(expressionList->Check(table, ok))->GetSubTypes();
        typeStr = "\"";
        for (auto &type: types) {
            TypeID tp = UniquePtrCast<WrapperType>(type->Copy())->DeWrap()->GetTypeId();
            switch (tp) {
                case BOOLEAN:
                    typeStr += "\%d";
                    break;
                case INTEGER:
                    typeStr += "\%d";
                    break;
                case REAL:
                    typeStr += "\%f";
                    break;
                case CHAR:
                    typeStr += "\%c";
                    break;
                default:
                    typeStr += "";
            }
        }
        typeStr += "\"";
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> StatementList::Check(SymbolTable &table, bool &ok) {
        for (auto &statement: statements) {
            if (statement != nullptr)
                statement->Check(table, ok);
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> CompoundStatement::Check(SymbolTable &table, bool &ok) {
        statementList->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> SubProgramHead::Check(SymbolTable &table, bool &ok) {
        auto lVoid = std::unique_ptr<TypeBase>(new LValueType(GenType(VOID)));
        table.InsertSymbol(name, lVoid->Copy(), false, "");
        auto retType(lVoid->Copy());
        if (returnType != nullptr) {
            retType = returnType->Check(table, ok);
        }
        std::unique_ptr<TypeBase> funcType(
                new FuncType(
                        UniquePtrCast<TupleType>(parameters->Check(table, ok)),
                        std::move(retType)));

        TypeBase *lValueType = new LValueType(std::move(funcType));
        std::unique_ptr<TypeBase> ret(lValueType);
        table.InsertSymbol(name, ret->Copy(), false, "");
        return ret;
    }

    std::unique_ptr<TypeBase> SubProgramBody::Check(SymbolTable &table, bool &ok) {
        constantDeclarations->Check(table, ok);
        variableDeclarations->Check(table, ok);
        compoundStatement->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> SubProgram::Check(SymbolTable &table, bool &ok) {
        CheckDuplicateIdentifier(table, ok, head->name, head->Line, head->Column, "subprogram declaration");
        table.PushMap();
        table.Step();
        auto funcType(head->Check(table, ok));
        body->Check(table, ok);
        table.PopMap();
        table.InsertSymbol(head->name, std::move(funcType), false, "");
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> SubProgramDeclarations::Check(SymbolTable &table, bool &ok) {
        for (auto &subProgram: subPrograms)
            subProgram->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ProgramHead::Check(SymbolTable &table, bool &ok) {
        auto lVoid = std::unique_ptr<TypeBase>(new LValueType(GenType(VOID)));
        table.InsertSymbol(name, lVoid->Copy(), false, "");
        identifiers->Check(table, ok);
        for (int i = 0; i < identifiers->identifiers.size(); i++) {
            auto id = identifiers->identifiers[i];
            CheckDuplicateIdentifier(table, ok, id, identifiers->Lines[i], identifiers->Columns[i],
                                     "program head");
            if (ok) {
                table.InsertSymbol(id, lVoid->Copy(), false, "");
            }
        }
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ProgramBody::Check(SymbolTable &table, bool &ok) {
        constantDeclarations->Check(table, ok);
        variableDeclarations->Check(table, ok);
        subProgramDeclarations->Check(table, ok);
        compoundStatemnet->Check(table, ok);
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase> ProgramStruct::Check(SymbolTable &table, bool &ok) {
        table.PushMap();
        table.Step();
        programHead->Check(table, ok);
        programBody->Check(table, ok);
        table.PopMap();
        return GenType(VOID);
    }
}