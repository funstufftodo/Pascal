#include <map>
#include <set>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>
#include <algorithm>
#include "utils.h"

const std::string sync = R"(programstruct
const var array procedure function begin if for read write $
program_head
; const var array procedure function begin if for read write $
program_body
. array if for read write $
idlist
) const var : array procedure function begin if for read write $
const_declarations
array if for read write $
var_declarations
const array if for read write $
subprogram_declarations
const var array if for read write $
compound_statement
; . const var array procedure function end if for read write else $
const_declaration
; const var array procedure function begin if for read write $
const_value
; const var array procedure function begin if for read write $
num
; ) , const = + - var array ] procedure function begin end if then for to do read write else relop or mulop $
var_declaration
; const var array procedure function begin if for read write $
type
; const var procedure function begin if for read write $
basic_type
; ) const var array procedure function begin if for read write $
period
const var array ] procedure function begin if for read write $
subprogram
; const var array begin if for read write $
subprogram_head
; const var array begin if for read write $
subprogram_body
; array procedure function if for read write $
formal_parameter
const var array procedure function begin if for read write $
parameter_list
const array procedure function begin if for read write $
parameter
const array procedure function begin if for read write $
var_parameter
; ) const array procedure function begin if for read write $
value_parameter
; ) const var array procedure function begin if for read write $
statement_list
const var array procedure function $
statement
const var array procedure function $
variable
; ) , const = + - var array ] procedure function begin end assignop if then for to do read write else relop or mulop $
expression
const var array procedure function begin if for read write $
else_part
const var array procedure function begin if for read write $
variable_list
) const var array procedure function begin if for read write $
expression_list
const var array procedure function begin if for read write $
id_varpart
const var array procedure function begin if for read write $
simple_expression
; ) , const = var array ] procedure function begin end if then for to do read write else relop $
term
; ) , const = var array ] procedure function begin end if then for to do read write else relop or $
factor
; ) , const = var array ] procedure function begin end if then for to do read write else relop or mulop $
idlist_78
const var array procedure function begin if for read write $
subprogram_declarations_79
const var array if for read write $
const_declaration_80
const var array procedure function begin if for read write $
var_declaration_81
const var array procedure function begin if for read write $
period_82
const var array procedure function begin if for read write $
parameter_list_83
const var array procedure function begin if for read write $
statement_list_84
const var array procedure function begin if for read write $
variable_list_85
const var array procedure function begin if for read write $
expression_list_86
const var array procedure function begin if for read write $
simple_expression_87
const var array procedure function begin if for read write $
term_88
const var array procedure function begin if for read write $
program_head_89
const var array procedure function begin if for read write $
statement_90
const var array procedure function begin if for read write $
expression_91
const var array procedure function begin if for read write $
)";

const std::string ll1 = R"(programstruct
program program_head
program_head
program program
program_body
const const_declarations
var const_declarations
procedure const_declarations
function const_declarations
begin const_declarations
idlist
id id
const_declarations
const const
var EPS
procedure EPS
function EPS
begin EPS
var_declarations
var var
procedure EPS
function EPS
begin EPS
subprogram_declarations
procedure EPS
function EPS
begin EPS
compound_statement
begin begin
const_declaration
id id
const_value
+ +
- -
' '
digits num
float num
num
digits digits
float float
var_declaration
id idlist
type
array array
integer basic_type
real basic_type
boolean basic_type
char basic_type
basic_type
integer integer
real real
boolean boolean
char char
period
digits digits
subprogram
procedure subprogram_head
function subprogram_head
subprogram_head
procedure procedure
function function
subprogram_body
const const_declarations
var const_declarations
begin const_declarations
formal_parameter
; EPS
( (
: EPS
parameter_list
; parameter
id parameter
) parameter
var parameter
parameter
; EPS
id value_parameter
) EPS
var var_parameter
var_parameter
var var
value_parameter
id idlist
statement_list
; statement
id statement
begin statement
end statement
if statement
for statement
read statement
write statement
Break statement
while statement
statement
; EPS
id variable
begin compound_statement
end EPS
if if
for for
read read
write write
else EPS
Break Break
while while
variable
id id
expression
; EPS
id simple_expression
( simple_expression
) EPS
, EPS
+ simple_expression
- simple_expression
] EPS
digits simple_expression
end EPS
then EPS
to EPS
do EPS
else EPS
not simple_expression
float simple_expression
else_part
; EPS
end EPS
else EPS else
variable_list
id variable
expression_list
id expression
( expression
) expression
, expression
+ expression
- expression
] expression
digits expression
not expression
float expression
id_varpart
; EPS
( (
) EPS
, EPS
= EPS
+ EPS
- EPS
[ [
] EPS
end EPS
assignop EPS
then EPS
to EPS
do EPS
else EPS
relop EPS
or EPS
mulop EPS
simple_expression
id term
( term
+ term
- term
digits term
not term
float term
term
id factor
( factor
+ factor
- factor
digits factor
not factor
float factor
factor
id variable
( (
+ +
- -
digits num
not not
float num
idlist_78
) EPS
, ,
: EPS
subprogram_declarations_79
procedure subprogram
function subprogram
begin EPS
const_declaration_80
; EPS ;
var_declaration_81
; EPS ;
period_82
, ,
] EPS
parameter_list_83
; ;
) EPS
statement_list_84
; ;
end EPS
variable_list_85
) EPS
, ,
expression_list_86
) EPS
, ,
] EPS
simple_expression_87
; EPS
) EPS
, EPS
= EPS
+ +
- -
] EPS
end EPS
then EPS
to EPS
do EPS
else EPS
relop EPS
or or
term_88
; EPS
) EPS
, EPS
= EPS
+ EPS
- EPS
] EPS
end EPS
then EPS
to EPS
do EPS
else EPS
relop EPS
or EPS
mulop mulop
program_head_89
; EPS
( (
statement_90
; EPS
end EPS
assignop assignop
else EPS
expression_91
; EPS
) EPS
, EPS
= =
] EPS
end EPS
then EPS
to EPS
do EPS
else EPS
relop relop
)";

namespace Utils {

    std::map<int, std::set<int>> FirstSet;
    std::map<int, std::set<int>> FollowSet;

    void calculateFirstSet(int);

    bool processProduction(const Expression &, std::set<int> &);

    bool processSymbol(int symbolId, std::set<int> &);

    bool processTerminal(int, std::set<int> &);

    bool processNonTerminal(int, std::set<int> &);

    void GenFirst() {
        for (auto symbol: Symbols) {
            int id = symbol.first;
            if (symbol.second.type == TERMI) {
                FirstSet.insert(std::pair<int, std::set<int>>(id, std::set<int>()));
                FirstSet.find(id)->second.insert(id);
            } else {
                if (FirstSet.find(id) == FirstSet.end()) {
                    calculateFirstSet(id);
                }
            }
        }
    }

    void calculateFirstSet(int symbolId) {
        Symbol &symbol = Symbols[symbolId];
        std::set<int> firstSet;

        for (const auto &[first, productions]: symbol.subExpressions) {
            // 构造完整产生式
            Expression production = productions[0];
            production.insert(production.begin(), first);

            // 如果产生式可以推导出ε，将ε加入FIRST集
            if (processProduction(production, firstSet)) {
                firstSet.insert(EPS);
            }
        }

        FirstSet[symbolId] = firstSet;
    }

    bool processProduction(const Expression &production, std::set<int> &firstSet) {
        bool canDeriveEps = true;

        for (int symbolId: production) {
            if (!processSymbol(symbolId, firstSet)) {
                canDeriveEps = false;
                break;
            }
        }

        return canDeriveEps;
    }

    bool processSymbol(int symbolId, std::set<int> &firstSet) {
        auto &symbol = Symbols[symbolId];
        return symbol.type == TERMI ?
               processTerminal(symbolId, firstSet) :
               processNonTerminal(symbolId, firstSet);
    }

    bool processTerminal(int symbolId, std::set<int> &firstSet) {
        if (symbolId != EPS) {
            firstSet.insert(symbolId);
            return false;
        }
        return true;
    }

    bool processNonTerminal(int symbolId, std::set<int> &firstSet) {
        if (FirstSet.find(symbolId) == FirstSet.end()) {
            calculateFirstSet(symbolId);
        }

        bool hasEps = false;
        for (int symbol: FirstSet[symbolId]) {
            if (symbol == EPS) {
                hasEps = true;
            } else {
                firstSet.insert(symbol);
            }
        }
        return hasEps;
    }

    void ShowFirst() {
        for (auto fs: FirstSet) {
            std::cout << InvSymbolNameMap.find(fs.first)->second << " :" << std::endl;
            for (int i: fs.second) {
                std::cout << InvSymbolNameMap.find(i)->second << " ";
            }
            std::cout << std::endl << "------------------------" << std::endl;
        }
    }

    std::map<int, std::set<int>> FollowDependency;   // 存储Follow集合之间的依赖关系
    static int depth;                                // Tarjan算法中的深度计数器
    static std::set<int> visit;                      // 记录已访问的节点
    static std::map<int, int> dfn;                   // 节点的发现时间
    static std::map<int, int> low;                   // 节点能够回溯到的最早的节点
    static std::set<int> instk;                      // 当前在栈中的节点集合
    static std::stack<int> stk;                      // Tarjan算法使用的栈

    void BuildInitialFollowSets();

    void ProcessFollowDependencies(int);

    void UpdateFollowSetAndDependency(int, int, const std::set<int> &, bool);

    void UpdateTempFollow(const std::set<int> &, std::set<int> &, bool &);

    void ProcessSubExpression(const Expression &, int);

    void ProcessDependencies(int);

    void ProcessSCC(int, const std::vector<int> &);

    void GenFollow(int startId) {
        // 初始化 FollowSet
        for (auto symbolIt: Symbols) {
            if (symbolIt.second.type == TERMI) continue;
            FollowSet.insert(std::pair<int, std::set<int>>(symbolIt.first, std::set<int>()));
        }

        FollowSet.find(startId)->second.insert(END);
        BuildInitialFollowSets();

        for (auto symbolIt: Symbols) {
            if (symbolIt.second.type == NON_TERMI && visit.find(symbolIt.first) == visit.end()) {
                depth = 0;
                dfn.clear();
                low.clear();
                instk.clear();
                while (!stk.empty()) stk.pop();
                ProcessFollowDependencies(symbolIt.first);
            }
        }
    }

    void BuildInitialFollowSets() {
        for (auto symbolIt: Symbols) {
            for (auto subExpressions: symbolIt.second.subExpressions) {
                Expression subExpression = subExpressions.second[0];
                subExpression.insert(subExpression.begin(), subExpressions.first);
                ProcessSubExpression(subExpression, symbolIt.first);
            }
        }
    }

    /**
     * 从右向左遍历每个符号,对于非终结符，更新其Follow集和依赖关系,
     * 维护一个临时Follow集合，用于记录当前位置后面的First集
     */
    void ProcessSubExpression(const Expression &subExpression, int symbolId) {
        bool toEnd = true;
        std::set<int> tempFollow;

        for (int i = subExpression.size() - 1; i >= 0; --i) {
            int subSymbolId = subExpression[i];
            std::set<int> &subFirstSet = FirstSet.find(subSymbolId)->second;
            Symbol subSymbol = Symbols.find(subSymbolId)->second;

            if (subSymbol.type == NON_TERMI) {
                UpdateFollowSetAndDependency(subSymbolId, symbolId, tempFollow, toEnd);
            }

            UpdateTempFollow(subFirstSet, tempFollow, toEnd);
        }
    }

    /**
     * 将临时Follow集加入当前非终结符的Follow集中
     * 如果可以到达产生式末尾，建立Follow依赖关系
     */
    void UpdateFollowSetAndDependency(int subSymbolId, int symbolId, const std::set<int> &tempFollow, bool toEnd) {
        std::set<int> &subFollowSet = FollowSet.find(subSymbolId)->second;
        for (int follow: tempFollow) {
            subFollowSet.insert(follow);
        }

        if (toEnd && symbolId != subSymbolId) {
            auto dependencyIt = FollowDependency.find(subSymbolId);
            if (dependencyIt == FollowDependency.end()) {
                FollowDependency.insert(std::pair<int, std::set<int>>(subSymbolId, std::set<int>()));
                dependencyIt = FollowDependency.find(subSymbolId);
            }
            dependencyIt->second.insert(symbolId);
        }
    }

    /**
     * 如果First集不含空串，则清空tempFollow并设置toEnd为false
     * 将First集中非空串的元素添加到tempFollow中
     */
    void UpdateTempFollow(const std::set<int> &firstSet, std::set<int> &tempFollow, bool &toEnd) {
        if (firstSet.find(EPS) == firstSet.end()) {
            toEnd = false;
            tempFollow.clear();
        }

        for (int first: firstSet) {
            if (first != EPS) {
                tempFollow.insert(first);
            }
        }
    }

    /**
     * 使用Tarjan算法寻找Follow集之间的循环依赖
     * 将查找到的含有循环依赖的非终结符组成一个SCC进行处理
     */
    void ProcessFollowDependencies(int id) {
        visit.insert(id);
        dfn.insert(std::pair<int, int>(id, ++depth));
        low.insert(std::pair<int, int>(id, depth));
        instk.insert(id);
        stk.push(id);

        ProcessDependencies(id);

        if (dfn.find(id)->second == low.find(id)->second) {
            std::vector<int> components;
            while (true) {
                int component = stk.top();
                stk.pop();
                components.push_back(component);
                instk.erase(instk.find(component));
                if (component == id) break;
            }
            ProcessSCC(id, components);
        }
    }

    /**
     * 遍历当前节点的所有依赖节点，更新low值和Follow集合
     */
    void ProcessDependencies(int id) {
        auto it = FollowDependency.find(id);
        std::set<int> &curFollowSet = FollowSet.find(id)->second;

        if (it != FollowDependency.end()) {
            for (int nxt: it->second) {
                int &curlow = low.find(id)->second;
                if (dfn.find(nxt) == dfn.end()) {
                    ProcessFollowDependencies(nxt);
                    curlow = std::min(curlow, low.find(nxt)->second);
                } else if (instk.find(nxt) != instk.end()) {
                    curlow = std::min(curlow, dfn.find(nxt)->second);
                }
                for (int follow: FollowSet.find(nxt)->second) {
                    curFollowSet.insert(follow);
                }
            }
        }
    }

    /**
     * 处理强连通分量(SCC)，将强连通分量的所有节点的Follow集合并，
     * 将合并后的集合更新给每个节点
     */
    void ProcessSCC(int id, const std::vector<int> &components) {
        std::set<int> tmpFollow;
        // 收集所有组件的 Follow 集
        for (int component: components) {
            const std::set<int> &componentFollow = FollowSet.find(component)->second;
            tmpFollow.insert(componentFollow.begin(), componentFollow.end());
        }

        // 更新所有组件的 Follow 集
        for (int component: components) {
            std::set<int> &componentFollowSet = FollowSet.find(component)->second;
            componentFollowSet.insert(tmpFollow.begin(), tmpFollow.end());
        }
    }

    void ShowFollow() {
        for (auto fs: FollowSet) {
            std::cout << InvSymbolNameMap.find(fs.first)->second << " :" << std::endl;
            for (int i: fs.second) {
                std::cout << InvSymbolNameMap.find(i)->second << " ";
            }
            std::cout << std::endl << "------------------------" << std::endl;
        }
    }

    std::set<int> GetFirst(Expression &expression) {
        std::set<int> firstSet;
        bool canDeriveEmpty = true;

        // 从左到右扫描表达式中的每个符号
        for (int symbolId: expression) {
            const std::set<int> &symbolFirstSet = FirstSet.find(symbolId)->second;
            bool containsEmpty = false;

            // 处理当前符号的First集
            for (int terminal: symbolFirstSet) {
                if (terminal == EPS) {
                    containsEmpty = true;
                } else {
                    firstSet.insert(terminal);
                }
            }

            // 如果当前符号不能推导出空串，后续符号不需要继续处理
            if (!containsEmpty) {
                canDeriveEmpty = false;
                break;
            }
        }

        // 如果表达式中所有符号都可以推导出空串，将空串加入结果集
        if (canDeriveEmpty) {
            firstSet.insert(EPS);
        }

        return firstSet;
    }

    std::map<int, std::map<int, LL1Item>> LL1Table;
    std::map<int, std::set<int>> SyncTable;

    static const std::vector<std::string> syncSymbol = {
            "$", "begin", "const", "var", "procedure",
            "function", "array", "for", "if", "read", "write"
    };

    void ProcessEpsilonCase(int, int, const std::set<int> &, std::map<int, LL1Item> &);

    void ProcessNonEpsilonCase(int, int, int, std::map<int, LL1Item> &);

    std::vector<int> CollectNonTerminals() {
        std::vector<int> nonTermiIds;
        for (const auto &symbolIt: Symbols) {
            if (symbolIt.first != EPS && symbolIt.second.type == NON_TERMI) {
                nonTermiIds.push_back(symbolIt.first);
            }
        }
        return nonTermiIds;
    }

    /**
     * 假设有产生式 E' → + T E' | ε:
     * 对于第一个产生式 + T E': 调用 ProcessNonEpsilonCase
     * 对于第二个产生式 ε: 调用 ProcessEpsilonCase
     */
    void ProcessProductionFirst(int nonTermiId, const Expression &subExpression, int productionId,
                                const std::set<int> &firstSet, const std::set<int> &followSet,
                                std::map<int, LL1Item> &ll1Map) {
        for (int first: firstSet) {
            if (first == EPS) {
                ProcessEpsilonCase(nonTermiId, productionId, followSet, ll1Map);
            } else {
                ProcessNonEpsilonCase(nonTermiId, productionId, first, ll1Map);
            }
        }
    }

    /**
     * 假设我们有产生式 E' → ε，非终结符 E' 的Follow集为 {$, )}
     * 因为产生式可以推导出空串，需要将产生式加入到Follow集中每个终结符对应的表项
     * 对于 $ ∈ Follow(E')：如果LL1Table[E', $]已存在：输出错误信息（不是LL(1)文法），将产生式加入已有表项
     * 否则：创建新表项：LL1Table[E', $] = "ε"
     */
    void ProcessEpsilonCase(int nonTermiId, int productionId, const std::set<int> &followSet,
                            std::map<int, LL1Item> &ll1Map) {
        for (int follow: followSet) {
            auto itemIt = ll1Map.find(follow);
            if (itemIt != ll1Map.end()) {
                std::cout << "\033[31mLL1 ERROR FOLLOW \033[0m"
                          << InvSymbolNameMap.find(nonTermiId)->second
                          << " " << InvSymbolNameMap.find(follow)->second << std::endl;
                itemIt->second.push_back(productionId);
            } else {
                std::cout << "LL1 INSERT FOLLOW "
                          << InvSymbolNameMap.find(nonTermiId)->second << " "
                          << InvSymbolNameMap.find(follow)->second << std::endl;
                LL1Item item;
                item.push_back(productionId);
                ll1Map.insert({follow, item});
            }
        }
    }

    /**
     * 假设我们有产生式 E → T E'：产生式的First集包含 {(, id}，需要将产生式加入到这些终结符对应的表项
     * 检查LL1Table[E, (]是否已有项：如果已存在：输出错误信息（不是LL(1)文法）将产生式加入已有表项
     * 否则：创建新表项：LL1Table[E, (] = "T E'"
     */
    void ProcessNonEpsilonCase(int nonTermiId, int productionId, int first, std::map<int, LL1Item> &ll1Map) {
        auto itemIt = ll1Map.find(first);
        if (itemIt != ll1Map.end()) {
            std::cout << "\033[31mLL1 ERROR FIRST \033[0m"
                      << InvSymbolNameMap.find(nonTermiId)->second << " "
                      << InvSymbolNameMap.find(first)->second << std::endl;
            itemIt->second.push_back(productionId);
        } else {
            std::cout << "LL1 INSERT FIRST "
                      << InvSymbolNameMap.find(nonTermiId)->second << " "
                      << InvSymbolNameMap.find(first)->second << std::endl;
            LL1Item item;
            item.push_back(productionId);
            ll1Map.insert({first, item});
        }
    }

    /**
     * 将Follow(E)中未在LL1表中使用的终结符加入syncSet，
     * 将预定义同步符号加入syncSet
     */
    void BuildSyncSet(int nonTermiId, const std::map<int, LL1Item> &ll1Map) {
        std::set<int> syncSet;

        auto followSet = FollowSet.find(nonTermiId)->second;
        for (auto followId: followSet) {
            if (ll1Map.find(followId) == ll1Map.end()) {
                syncSet.insert(followId);
            }
        }

        for (const auto &syncStr: syncSymbol) {
            int syncId = SymbolNameMap.find(syncStr)->second;
            if (ll1Map.find(syncId) == ll1Map.end()) {
                syncSet.insert(syncId);
            }
        }

        SyncTable.insert({nonTermiId, syncSet});
    }

    void GenLL1() {
        std::vector<int> nonTermiIds = CollectNonTerminals();

        // 为每个非终结符构建LL1表项
        for (int nonTermiId: nonTermiIds) {
            std::map<int, LL1Item> ll1Map;
            Symbol &symbol = Symbols.find(nonTermiId)->second;
            std::set<int> &followSet = FollowSet.find(nonTermiId)->second;

            std::cout << "CONSTRUCT FOR "
                      << InvSymbolNameMap.find(nonTermiId)->second
                      << "------------" << std::endl;

            // 处理每个产生式
            for (const auto &subExpressionsIt: symbol.subExpressions) {
                Expression subExpression = subExpressionsIt.second[0];
                subExpression.insert(subExpression.begin(), subExpressionsIt.first);

                std::set<int> firstSet = GetFirst(subExpression);

                // 输出当前处理的表达式
                std::cout << "FOR SUB EXPRESSION ";
                for (int sb: subExpression) {
                    std::cout << InvSymbolNameMap.find(sb)->second << " ";
                }
                std::cout << std::endl;

                ProcessProductionFirst(nonTermiId, subExpression,
                                       subExpressionsIt.first, firstSet,
                                       followSet, ll1Map);
            }

            LL1Table.insert({nonTermiId, ll1Map});
        }

        // 构建同步集
        for (int nonTermiId: nonTermiIds) {
            auto &ll1Map = LL1Table.find(nonTermiId)->second;
            BuildSyncSet(nonTermiId, ll1Map);
        }
    }

    void ShowLL1Table() {
        for (auto symbolIt: Symbols) {
            if (symbolIt.second.type == TERMI)
                continue;
            int symbolId = symbolIt.first;
            Symbol &symbol = symbolIt.second;
            std::cout << InvSymbolNameMap.find(symbolId)->second << "----------" << std::endl;
            for (auto items: LL1Table.find(symbolId)->second) {
                for (auto item: items.second) {
                    std::cout << InvSymbolNameMap.find(items.first)->second << " -> "
                              << InvSymbolNameMap.find(item)->second;
                    Expression expression = symbol.subExpressions.find(item)->second[0];
                    for (int subSymbolId: expression)
                        std::cout << " " << InvSymbolNameMap.find(subSymbolId)->second;
                    std::cout << std::endl;
                }
            }
        }
    }


    void SaveLL1Table(std::string tablePath, std::string syncPath) {
        std::string s;
        for (auto symbolIt: Symbols) {
            if (symbolIt.second.type == TERMI)
                continue;
            int symbolId = symbolIt.first;
            s += InvSymbolNameMap.find(symbolId)->second + "\n";
            for (auto items: LL1Table.find(symbolId)->second) {
                if (!items.second.size())
                    continue;
                std::string itemStr = InvSymbolNameMap.find(items.first)->second;
                for (auto item: items.second)
                    itemStr += " " + InvSymbolNameMap.find(item)->second;
                s += itemStr + "\n";
            }
        }
        std::ofstream ofile(tablePath);
        ofile << s;
        ofile.close();

        s = "";
        for (auto symbolIt: Symbols) {
            if (symbolIt.second.type == TERMI)
                continue;
            int symbolId = symbolIt.first;
            s += InvSymbolNameMap.find(symbolId)->second + "\n";
            for (auto item: SyncTable.find(symbolId)->second)
                s += InvSymbolNameMap.find(item)->second + " ";
            s += "\n";
        }
        ofile = std::ofstream(syncPath);
        ofile << s;
        ofile.close();
    }

    void LoadLL1Table(std::string tablePath, std::string syncPath) {
        std::stringstream f(ll1);
        std::string line = "";
        int curSymbol;
        while (std::getline(f, line)) {
            std::istringstream itemStream(line);
            std::vector<std::string> items;
            std::string temp;
            while (std::getline(itemStream, temp, ' ')) {
                if (temp.length())
                    items.push_back(temp);
            }
            if (items.size() == 1) {
                curSymbol = SymbolNameMap.find(items[0])->second;
                LL1Table.insert(std::pair<int, std::map<int, LL1Item>>(curSymbol, std::map<int, LL1Item>()));
            } else if (items.size()) {
                int sb = SymbolNameMap.find(items[0])->second;
                LL1Item item;
                for (int i = 1; i < items.size(); i++)
                    item.push_back(SymbolNameMap.find(items[i])->second);
                LL1Table.find(curSymbol)->second.insert(std::pair<int, LL1Item>(sb, item));
            }
        }

        f = std::stringstream(sync);
        line = "";
        while (std::getline(f, line)) {
            curSymbol = SymbolNameMap.find(line)->second;
            std::set<int> syncSet;
            std::getline(f, line);
            std::istringstream itemStream(line);
            std::string temp;
            while (std::getline(itemStream, temp, ' ')) {
                if (temp.length())
                    syncSet.insert(SymbolNameMap.find(temp)->second);
            }
            SyncTable.insert(std::pair<int, std::set<int>>(curSymbol, syncSet));
        }
    }
}