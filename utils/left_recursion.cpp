#include <map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>
#include "utils.h"

const std::string grammar = R"(programstruct program_head ; program_body .
program_head program id ( idlist )
program_head program id
program_body const_declarations var_declarations subprogram_declarations compound_statement
idlist idlist , id
idlist id
const_declarations const const_declaration ;
const_declarations EPS
const_declaration const_declaration ; id = const_value
const_declaration id = const_value
const_value + num
const_value - num
const_value num
const_value ' letter '
var_declarations var var_declaration ;
var_declarations EPS
var_declaration var_declaration ; idlist : type
var_declaration idlist : type
type basic_type
type array [ period ] of basic_type
basic_type integer
basic_type real
basic_type boolean
basic_type char
period period , digits .. digits
period digits .. digits
subprogram_declarations subprogram_declarations subprogram ;
subprogram_declarations EPS
subprogram subprogram_head ; subprogram_body
subprogram_head procedure id formal_parameter
subprogram_head function id formal_parameter : basic_type
formal_parameter ( parameter_list )
formal_parameter EPS
parameter_list parameter_list ; parameter
parameter_list parameter
parameter var_parameter
parameter value_parameter

parameter EPS

var_parameter var value_parameter
value_parameter idlist : basic_type
subprogram_body const_declarations var_declarations compound_statement
compound_statement begin statement_list end
statement_list statement_list ; statement
statement_list statement
statement variable assignop expression
statement variable
statement compound_statement
statement if expression then statement else_part
statement for id assignop expression to expression do statement
statement read ( variable_list )
statement write ( expression_list )
statement Break

statement while ( expression ) do statement

statement EPS
variable_list variable_list , variable
variable_list variable
variable id id_varpart
id_varpart [ expression_list ]
id_varpart ( expression_list )
id_varpart EPS
else_part else statement
else_part EPS
expression_list expression_list , expression
expression_list expression
expression simple_expression relop simple_expression
expression simple_expression = simple_expression
expression simple_expression
expression EPS
simple_expression simple_expression + term
simple_expression simple_expression - term
simple_expression simple_expression or term
simple_expression term
term term mulop factor
term factor
factor num
factor variable
factor ( expression )
factor not factor
factor - factor
factor + factor
num digits
num float)";

const std::string reduced = R"(programstruct program_head ; program_body .
program_head program id program_head_89
program_body const_declarations var_declarations subprogram_declarations compound_statement
idlist id idlist_78
const_declarations EPS
const_declarations const const_declaration ;
var_declarations EPS
var_declarations var var_declaration ;
subprogram_declarations EPS subprogram_declarations_79
compound_statement begin statement_list end
const_declaration id = const_value const_declaration_80
const_value + num
const_value num
const_value - num
const_value ' letter '
num digits
num float
var_declaration idlist : type var_declaration_81
type basic_type
type array [ period ] of basic_type
basic_type integer
basic_type real
basic_type boolean
basic_type char
period digits .. digits period_82
subprogram subprogram_head ; subprogram_body
subprogram_head procedure id formal_parameter
subprogram_head function id formal_parameter : basic_type
subprogram_body const_declarations var_declarations compound_statement
formal_parameter EPS
formal_parameter ( parameter_list )
parameter_list parameter parameter_list_83
parameter var_parameter
parameter value_parameter
var_parameter var value_parameter
value_parameter idlist : basic_type
statement_list statement statement_list_84
statement EPS
statement compound_statement
statement variable statement_90
statement if expression then statement else_part
statement for id assignop expression to expression do statement
statement while expression do statement
statement read ( variable_list )
statement write ( expression_list )
variable id id_varpart
expression simple_expression expression_91
else_part EPS
else_part else statement
variable_list variable variable_list_85
expression_list expression expression_list_86
id_varpart EPS
id_varpart ( expression_list )
id_varpart [ expression_list ]
simple_expression term simple_expression_87
term factor term_88
factor ( expression )
factor num
factor ' letter '
factor - factor
factor variable
factor not factor
idlist_78 EPS
idlist_78 , id idlist_78
subprogram_declarations_79 EPS
subprogram_declarations_79 subprogram ; subprogram_declarations_79
const_declaration_80 EPS
const_declaration_80 ; id = const_value const_declaration_80
var_declaration_81 EPS
var_declaration_81 ; idlist : type var_declaration_81
period_82 EPS
period_82 , digits .. digits period_82
parameter_list_83 EPS
parameter_list_83 ; parameter parameter_list_83
statement_list_84 EPS
statement_list_84 ; statement statement_list_84
variable_list_85 EPS
variable_list_85 , variable variable_list_85
expression_list_86 EPS
expression_list_86 , expression expression_list_86
simple_expression_87 EPS
simple_expression_87 + term simple_expression_87
simple_expression_87 - term simple_expression_87
simple_expression_87 or term simple_expression_87
term_88 EPS
term_88 mulop factor term_88
program_head_89 EPS
program_head_89 ( idlist )
statement_90 EPS
statement_90 assignop expression
expression_91 EPS
expression_91 = simple_expression
expression_91 relop simple_expression)";

namespace Utils {

    int SymbolID;
    std::map<std::string, int> SymbolNameMap;
    std::map<int, std::string> InvSymbolNameMap;
    std::map<int, Symbol> Symbols;

    void LoadSymbols(const std::string &pth) {
        if(pth == "grammar") {
            std::stringstream f(grammar);
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
        }
        if(pth == "reduced") {
            std::stringstream f(reduced);
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
        }
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