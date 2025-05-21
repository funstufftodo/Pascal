#include "parser.h"

namespace CompilerFront {

    namespace {
        bool handleConstDeclaration(const std::string& topSymbolName, const Utils::Symbol& topSymbol,
                                    const Token& nextToken, Utils::Expression& expression, const Utils::LL1Item& item) {
            if (topSymbolName != "const_declaration_80") return false;

            for (int sbitem : item) {
                if (nextToken.type == "id" && sbitem != Utils::EPS) {
                    expression = topSymbol.subExpressions.find(sbitem)->second[0];
                    expression.insert(expression.begin(), sbitem);
                    return true;
                } else if (nextToken.type != "id" && sbitem == Utils::EPS) {
                    expression = topSymbol.subExpressions.find(sbitem)->second[0];
                    expression.insert(expression.begin(), sbitem);
                    return true;
                }
            }
            return false;
        }

        bool handleVarDeclaration(const std::string& topSymbolName, const Utils::Symbol& topSymbol,
                                  const Token& nextToken, Utils::Expression& expression, const Utils::LL1Item& item) {
            if (topSymbolName != "var_declaration_81") return false;

            for (int sbitem : item) {
                if (nextToken.type == "id" && sbitem != Utils::EPS) {
                    expression = topSymbol.subExpressions.find(sbitem)->second[0];
                    expression.insert(expression.begin(), sbitem);
                    return true;
                } else if (nextToken.type != "id" && sbitem == Utils::EPS) {
                    expression = topSymbol.subExpressions.find(sbitem)->second[0];
                    expression.insert(expression.begin(), sbitem);
                    return true;
                }
            }
            return false;
        }

        bool handleElsePart(const std::string& topSymbolName, const Utils::Symbol& topSymbol,
                            const Token& /*nextToken*/, Utils::Expression& expression,
                            const Utils::LL1Item& item, int& ifCnt) {
            if (topSymbolName != "else_part") return false;

            for (int sbitem : item) {
                if (ifCnt == 0 && sbitem == Utils::EPS) {
                    expression = topSymbol.subExpressions.find(sbitem)->second[0];
                    expression.insert(expression.begin(), sbitem);
                    return true;
                } else if (ifCnt > 0 && sbitem != Utils::EPS) {
                    expression = topSymbol.subExpressions.find(sbitem)->second[0];
                    expression.insert(expression.begin(), sbitem);
                    ifCnt--;
                    return true;
                }
            }
            return false;
        }
    }

    void Parser::handleTerminalSymbol(int topSymbolId, int oriSymbolId, bool& ok, int& ifCnt) {
        if (topSymbolId == oriSymbolId) {
            if (curToken.type == "if")
                ifCnt++;
            symbolStack.pop();

            if (ok)
                astStack.push_back(AbstractSyntaxTree::GenOriAstNode(curToken));

            curToken = nxtToken;
            nxtToken = lexer.getToken();
        } else {
            ok = false;
            logErrMsg(curToken.line, curToken.column, topSymbolId, oriSymbolId, true);
            symbolStack.pop();
        }
    }

    void Parser::handleNonTerminalSymbol(int topSymbolId, const Utils::Symbol& topSymbol,
                                         int oriSymbolId, bool& ok, int& ifCnt) {
        std::map<int, Utils::LL1Item>& items = Utils::LL1Table.find(topSymbolId)->second;
        auto itemIt = items.find(oriSymbolId);

        if (itemIt == items.end()) {
            ok = false;
            logErrMsg(curToken.line, curToken.column, topSymbolId, oriSymbolId, false);
            auto& syncSet = Utils::SyncTable.find(topSymbolId)->second;

            if (syncSet.find(oriSymbolId) != syncSet.end()) {
                symbolStack.pop();
            } else {
                while ((items.find(oriSymbolId) == items.end()) &&
                       (syncSet.find(oriSymbolId) == syncSet.end())) {
                    curToken = nxtToken;
                    nxtToken = lexer.getToken();
                    oriSymbolId = Utils::SymbolNameMap.find(curToken.type)->second;
                    if (oriSymbolId == Utils::END)
                        break;
                }
            }
            return;
        }

        symbolStack.pop();
        Utils::LL1Item& item = itemIt->second;
        Utils::Expression expression;
        std::string topSymbolName;
        auto topSymbolIt = Utils::InvSymbolNameMap.find(topSymbolId);
        if (topSymbolIt != Utils::InvSymbolNameMap.end()) {
            topSymbolName = topSymbolIt->second;
        } else {
            // 严重错误：topSymbolId 不在 InvSymbolNameMap 中
            std::cerr << "Error: topSymbolId " << topSymbolId << " not found in InvSymbolNameMap." << std::endl;
            ok = false;
            return; // 或者抛出异常
        }

        if (item.size() > 1) {
            if (handleConstDeclaration(topSymbolName, topSymbol, nxtToken, expression, item)) {}
            else if (handleVarDeclaration(topSymbolName, topSymbol, nxtToken, expression, item)) {}
            else if (handleElsePart(topSymbolName, topSymbol, nxtToken, expression, item, ifCnt)) {}
        } else {
            if (topSymbolName == "else_part")
                ifCnt--;
            expression = topSymbol.subExpressions.find(item[0])->second[0];
            expression.insert(expression.begin(), item[0]);
        }

        bool allEps = true;
        int reduceStackSize = symbolStack.size();
        int reduceCnt = 0;

        for (int i = expression.size() - 1; i >= 0; --i) {
            if (expression[i] != Utils::EPS) {
                symbolStack.push(expression[i]);
                reduceCnt++;
                allEps = false;
            }
        }

        if (ok) {
            std::string exprFirstName;
            auto exprFirstIt = Utils::InvSymbolNameMap.find(expression[0]);
            if (exprFirstIt != Utils::InvSymbolNameMap.end()) {
                exprFirstName = exprFirstIt->second;
            } else {
                std::cerr << "Error: expression[0] (" << expression[0] << ") not found in InvSymbolNameMap." << std::endl;
                ok = false;
            }
            if (allEps) {
                std::vector<std::unique_ptr<AbstractSyntaxTree::ASTNode>> empty;
                astStack.push_back(AbstractSyntaxTree::GenAstNode(
                        topSymbolName,
                        exprFirstName, empty));
            } else {
                reduceStack.push(ReduceInfo(
                        reduceStackSize,
                        reduceCnt,
                        topSymbolName,
                        exprFirstName));
            }
        }
    }

    void Parser::processReduceStack(bool ok) {
        for (ReduceInfo info = reduceStack.top();
             ok && (symbolStack.size() == info.reduceStackSize); info = reduceStack.top()) {
            reduceStack.pop();
            std::vector<std::unique_ptr<AbstractSyntaxTree::ASTNode>> subNodes;

            for (int i = 0; i < info.reduceCnt; i++) {
                subNodes.insert(subNodes.begin(), std::move(astStack[astStack.size() - 1]));
                astStack.pop_back();
            }

            astStack.push_back(AbstractSyntaxTree::GenAstNode(
                    info.expressionLeft, info.expressionFirst, subNodes));

            if (reduceStack.empty())
                break;
        }
    }

    std::unique_ptr<AbstractSyntaxTree::ASTNode> Parser::Parse(bool& ok) {
        ok = true;
        curToken = lexer.getToken();
        nxtToken = lexer.getToken();
        int ifCnt = 0;

        while (!symbolStack.empty()) {
            int topSymbolId = symbolStack.top();
            const Utils::Symbol& topSymbol = Utils::Symbols.find(topSymbolId)->second;

            int oriSymbolId = Utils::SymbolNameMap.find(curToken.type)->second;
            const Utils::Symbol& oriSymbol = Utils::Symbols.find(oriSymbolId)->second;

            if (topSymbol.type == Utils::TERMI) {
                handleTerminalSymbol(topSymbolId, oriSymbolId, ok, ifCnt);
            } else {
                handleNonTerminalSymbol(topSymbolId, topSymbol, oriSymbolId, ok, ifCnt);
            }

            processReduceStack(ok);
        }

        return ok ? std::move(astStack[astStack.size() - 1]) : std::unique_ptr<AbstractSyntaxTree::ASTNode>();
    }

}