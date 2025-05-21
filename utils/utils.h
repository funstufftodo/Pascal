#ifndef COMPILER_UTILS_H
#define COMPILER_UTILS_H

#include <string>
#include <map>
#include <vector>
#include <set>
#include <climits>

namespace Utils {

    const int EPS = 0;
    const int END = INT_MAX;

    typedef std::vector<int> Expression;
    typedef std::vector<int> LL1Item;

    enum SymbolType {
        NON_TERMI,
        TERMI
    };

    /**
     * @brief 表示文法中的符号。
     * id：符号的唯一标识。
     * type：符号的类型，非终极符或是终结符
     * subExpressions：符号的产生式，key为产生式首符号的id，value为剩余产生式的序列。如：
     * A -> B + C 的 subExpressions 为 { (id)B: { (id)+, (id)C } }
     */
    struct Symbol {
        int id;
        SymbolType type;
        std::map<int, std::vector<Expression>> subExpressions;

        Symbol() {}

        Symbol(int sid, SymbolType tp) : id(sid), type(tp) {}
    };

    void LoadSymbols(const std::string &pth);

    void ElimLeftRecur();

    void CombineLeftCommon();

    void Show();

    void GenFirst();

    void ShowFirst();

    void GenFollow(int);

    void ShowFollow();

    void GenLL1();

    void ShowLL1Table();

    void SaveLL1Table(std::string, std::string);

    void LoadLL1Table(std::string, std::string);

    extern std::map<std::string, int> SymbolNameMap;
    extern std::map<int, std::string> InvSymbolNameMap;
    extern std::map<int, Symbol> Symbols;
    extern std::map<int, std::set<int>> FirstSet;
    extern std::map<int, std::set<int>> FollowSet;
    extern std::map<int, std::map<int, LL1Item>> LL1Table;
    extern std::map<int, std::set<int>> SyncTable;

    inline void InsertSymbolId(int id, std::string name) {
        SymbolNameMap.insert(std::pair<std::string, int>(name, id));
        InvSymbolNameMap.insert(std::pair<int, std::string>(id, name));
    }

    inline void AddConstantSymbols() {
        InsertSymbolId(100, "Break");
        Symbol breakSymbol(EPS, TERMI);
        Symbols.insert(std::pair<int, Symbol>(100, breakSymbol));

        InsertSymbolId(101, "while");
        Symbol whileSymbol(EPS, TERMI);
        Symbols.insert(std::pair<int, Symbol>(101, whileSymbol));

        InsertSymbolId(EPS, "EPS");
        Symbol epsSymbol(EPS, TERMI);
        Symbols.insert(std::pair<int, Symbol>(EPS, epsSymbol));

        InsertSymbolId(END, "$");
        Symbol endSymbol(END, TERMI);
        Symbols.insert(std::pair<int, Symbol>(END, endSymbol));
    }
}

#endif //COMPILER_UTILS_H
