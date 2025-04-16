#include "parser.h"

namespace CompilerFront {
    std::unique_ptr<AbstractSyntaxTree::ASTNode> Parser::Parse(bool &ok) {
        ok = true;
        std::map<int, std::map<int, Utils::LL1Item>> &ll1Table = Utils::LL1Table;
        curToken = lexer.getToken();
        nxtToken = lexer.getToken();
        int ifCnt = 0;

        while (!symbolStack.empty()) {
            int topSymbolId = symbolStack.top();
            const Utils::Symbol &topSymbol = Utils::Symbols.find(topSymbolId)->second;

            int oriSymbolId = Utils::SymbolNameMap.find(curToken.type)->second;
            const Utils::Symbol &oriSymbol = Utils::Symbols.find(oriSymbolId)->second;

            if (topSymbol.type == Utils::TERMI) {
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
                    continue;
                }
            } else {
                std::map<int, Utils::LL1Item> &items = ll1Table.find(topSymbolId)->second;
                auto itemIt = items.find(oriSymbolId);
                if (itemIt == items.end()) {
                    ok = false;
                    logErrMsg(curToken.line, curToken.column, topSymbolId, oriSymbolId, false);
                    auto &syncSet = Utils::SyncTable.find(topSymbolId)->second;
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
                    continue;
                }
                symbolStack.pop();

                Utils::LL1Item &item = itemIt->second;
                Utils::Expression expression;
                if (item.size() > 1) {
                    if (Utils::InvSymbolNameMap.find(topSymbolId)->second == "const_declaration_80") {
                        for (int sbitem: item) {
                            if (nxtToken.type == "id" && sbitem != Utils::EPS) {
                                expression = topSymbol.subExpressions.find(sbitem)->second[0];
                                expression.insert(expression.begin(), sbitem);
                                break;
                            } else if (nxtToken.type != "id" && sbitem == Utils::EPS) {
                                expression = topSymbol.subExpressions.find(sbitem)->second[0];
                                expression.insert(expression.begin(), sbitem);
                                break;
                            }
                        }
                    } else if (Utils::InvSymbolNameMap.find(topSymbolId)->second == "var_declaration_81") {
                        for (int sbitem: item) {
                            if (nxtToken.type == "id" && sbitem != Utils::EPS) {
                                expression = topSymbol.subExpressions.find(sbitem)->second[0];
                                expression.insert(expression.begin(), sbitem);
                                break;
                            } else if (nxtToken.type != "id" && sbitem == Utils::EPS) {
                                expression = topSymbol.subExpressions.find(sbitem)->second[0];
                                expression.insert(expression.begin(), sbitem);
                                break;
                            }
                        }
                    } else if (Utils::InvSymbolNameMap.find(topSymbolId)->second == "else_part") {
                        for (int sbitem: item) {
                            if (ifCnt == 0 && sbitem == Utils::EPS) {
                                expression = topSymbol.subExpressions.find(sbitem)->second[0];
                                expression.insert(expression.begin(), sbitem);
                                break;
                            } else if (ifCnt > 0 && sbitem != Utils::EPS) {
                                expression = topSymbol.subExpressions.find(sbitem)->second[0];
                                expression.insert(expression.begin(), sbitem);
                                ifCnt--;
                                break;
                            }
                        }
                    }
                } else {
                    if (Utils::InvSymbolNameMap.find(topSymbolId)->second == "else_part")
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
                    if (allEps) {
                        std::vector<std::unique_ptr<AbstractSyntaxTree::ASTNode>> empty;
                        astStack.push_back(AbstractSyntaxTree::GenAstNode(
                                Utils::InvSymbolNameMap.find(topSymbolId)->second,
                                Utils::InvSymbolNameMap.find(expression[0])->second, empty));
                    } else {
                        reduceStack.push(ReduceInfo(
                                reduceStackSize,
                                reduceCnt,
                                Utils::InvSymbolNameMap.find(topSymbolId)->second,
                                Utils::InvSymbolNameMap.find(expression[0])->second));
                    }
                }
            }
            for (ReduceInfo info = reduceStack.top();
                 ok && (symbolStack.size() == info.reduceStackSize); info = reduceStack.top()) {
                reduceStack.pop();
                std::vector<std::unique_ptr<AbstractSyntaxTree::ASTNode>> subNodes;
                // std::cout << "POP!!!!!!! " << info.expressionLeft << " " << info.expressionFirst << std::endl;
                for (int i = 0; i < info.reduceCnt; i++) {
                    subNodes.insert(subNodes.begin(), std::move(astStack[astStack.size() - 1]));
                    astStack.pop_back();
                }
                astStack.push_back(AbstractSyntaxTree::GenAstNode(info.expressionLeft, info.expressionFirst, subNodes));
                if (reduceStack.empty())
                    break;
            }
        }
        return ok ? std::move(astStack[astStack.size() - 1]) : std::unique_ptr<AbstractSyntaxTree::ASTNode>();
    }
}