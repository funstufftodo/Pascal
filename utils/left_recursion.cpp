#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "utils.h"

namespace Utils {

    int SymbolID;
    std::map<std::string, int> SymbolNameMap;
    std::map<int, std::string> InvSymbolNameMap;
    std::map<int, Symbol> Symbols;

    void LoadSymbols(const std::string &pth) {
        std::stringstream f(pth);
        std::string line;
        while (std::getline(f, line)) {
            std::istringstream lstream(line);
            std::string symbolName;
            bool isLeftPart = true;
            int leftPartId = 0;
            Expression expression;
            while (lstream >> symbolName) {
                if (symbolName.empty())
                    continue;
                auto it = SymbolNameMap.find(symbolName);
                int id;
                if (it == SymbolNameMap.end()) {
                    id = ++SymbolID;
                    SymbolNameMap[symbolName] = id;
                    InvSymbolNameMap[id] = symbolName;
                    Symbols[id] = Symbol(id, TERMI);
                } else {
                    id = it->second;
                }
                if (isLeftPart) {
                    isLeftPart = false;
                    leftPartId = id;
                } else {
                    expression.push_back(id);
                }
            }
            if (!expression.empty()) {
                int st = expression[0];
                expression.erase(expression.begin());
                Symbol &lsymbol = Symbols[leftPartId];
                lsymbol.type = NON_TERMI;
                lsymbol.subExpressions[st].push_back(expression);
            }
        }
        f.close();
    }

    void Show() {
        for (auto it1: Symbols) {
            std::cout << "Symbol: " << InvSymbolNameMap.find(it1.first)->second << " (Type: "
                      << (it1.second.type == TERMI ? "TERMI" : "NON_TERMI") << ")" << std::endl;
            std::cout << "Productions:" << std::endl;
            for (auto subs: it1.second.subExpressions) {
                std::cout << "  " << InvSymbolNameMap.find(subs.first)->second << " ";
                for (auto subExpression: subs.second) {
                    for (auto symbol: subExpression) {
                        std::cout << InvSymbolNameMap.find(symbol)->second << " ";
                    }
                    std::cout << "| ";
                }
                std::cout << std::endl;
            }
            std::cout << "------------------------" << std::endl;
        }
    }

    void ElimSingleLeftRecur(std::vector<int> &symbolIds) {
        for (int curId: symbolIds) {
            Symbol &symbol = Symbols[curId];

            //记录形如A -> Aα的产生式为badExpressionsIt
            auto badExpressionsIt = symbol.subExpressions.find(curId);
            if (badExpressionsIt != symbol.subExpressions.end()) {
                //badExpressions为badExpressionsIt的右部集合
                auto badExpressions = badExpressionsIt->second;
                //新建非终结符A'
                int newSymbolId = ++SymbolID;
                std::string newSymbolName = InvSymbolNameMap[curId] + "_" + std::to_string(newSymbolId);
                InsertSymbolId(newSymbolId, newSymbolName);
                Symbol newSymbol(newSymbolId, NON_TERMI);

                //删除形如A -> Aα的产生式
                symbol.subExpressions.erase(curId);

                //将 A -> Aα 转换为 A' -> αA'：
                //获取产生式右部第一个符号（st）
                //删除右部第一个符号（A）
                //在右部末尾添加新符号（A'）
                //将转换后的产生式加入新符号的产生式集
                for (auto &badExpression: badExpressions) {
                    if (badExpression.empty())
                        continue;
                    int st = badExpression[0];
                    badExpression.erase(badExpression.begin());
                    badExpression.push_back(newSymbolId);
                    newSymbol.subExpressions[st].push_back(badExpression);
                }

                //如果没有非递归产生式：
                //添加 A → A' 产生式
                //如果有非递归产生式：
                //为A'添加 ε 产生式
                //将所有 A -> β 转换为 A -> βA'
                if (symbol.subExpressions.empty()) {
                    symbol.subExpressions[newSymbolId].push_back(Expression());
                } else {
                    newSymbol.subExpressions[EPS].push_back(Expression());

                    for (auto &goodExpressions: symbol.subExpressions) {
                        for (auto &goodExpression: goodExpressions.second) {
                            goodExpression.push_back(newSymbolId);
                        }
                    }
                }
                Symbols[newSymbolId] = newSymbol;
            }
        }
    }

    void ElimLeftRecur() {
        std::vector<int> symbolIds;

        for (auto it: Symbols) {
            if (it.second.type == NON_TERMI) {
                symbolIds.push_back(it.first);

            }
        }
        for (int id: symbolIds) {
            Symbol &symbol = Symbols[id];
            for (auto subExpressionIt: symbol.subExpressions) {
                if (subExpressionIt.first == id) {
                    std::vector<int> badIds = {id};
                    ElimSingleLeftRecur(badIds);
                    break;
                }
            }
        }
    }

    void CombineSingleLeftCommon(int symbolId) {
        Symbol &symbol = Symbols.find(symbolId)->second;
        //筛选形如A -> abc | abd | abef等有公共部分的表达式
        for (auto subExpressionsIt = symbol.subExpressions.begin();
             subExpressionsIt != symbol.subExpressions.end(); subExpressionsIt++) {
            if (subExpressionsIt->second.size() > 1) {
                int newSymbolId = ++SymbolID;
                std::string newSymbolName = InvSymbolNameMap.find(symbolId)->second + "_" + std::to_string(newSymbolId);
                InsertSymbolId(newSymbolId, newSymbolName);
                Symbol newSymbol(newSymbolId, NON_TERMI);
                int maxCommonLen = -1;
                Expression preExpressionRight;
                for (auto subExpression: subExpressionsIt->second) {
                    if (maxCommonLen == -1) {
                        maxCommonLen = subExpression.size();
                    } else {
                        int i;
                        for (i = 0; i < std::min(maxCommonLen, int(subExpression.size())); i++) {
                            if (subExpression[i] != preExpressionRight[i]) {
                                break;
                            }
                        }
                        maxCommonLen = i;
                        if (maxCommonLen == 0)
                            break;
                    }
                    preExpressionRight = subExpression;
                }
                for (auto subExpression: subExpressionsIt->second) {
                    Expression newSubExpression;
                    int st = EPS;
                    if (subExpression.size() > maxCommonLen) {
                        st = subExpression[maxCommonLen];
                        for (int i = maxCommonLen + 1; i < subExpression.size(); i++) {
                            //newSubExpression是每个右部除去公共部分之外的部分的value
                            newSubExpression.push_back(subExpression[i]);
                        }
                    }
                    //构造A' -> c | d | ef
                    auto it = newSymbol.subExpressions.find(st);
                    if (it == newSymbol.subExpressions.end()) {
                        std::vector<Expression> newSubExpressions;
                        newSymbol.subExpressions[st] = newSubExpressions;
                        it = newSymbol.subExpressions.find(st);
                    }
                    it->second.push_back(newSubExpression);
                }
                Symbols[newSymbolId] = newSymbol;
                //构造A -> abA'
                Expression reducedExpression;
                if (preExpressionRight.size() > 0) {
                    for (int i = 0; i < maxCommonLen; i++)
                        reducedExpression.push_back(preExpressionRight[i]);
                }
                reducedExpression.push_back(newSymbolId);
                subExpressionsIt->second.clear();
                subExpressionsIt->second.push_back(reducedExpression);

                CombineSingleLeftCommon(newSymbolId);
            }
        }
    }

    void CombineLeftCommon() {
        std::vector<int> symbolIds;
        for (auto symbol: Symbols) {
            if (symbol.second.type == TERMI)
                continue;
            symbolIds.push_back(symbol.first);
        }
        for (int id: symbolIds) {
            CombineSingleLeftCommon(id);
        }
    }


}