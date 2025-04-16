#ifndef SYMBOLTABLE
#define SYMBOLTABLE

#include <map>
#include <vector>
#include <memory>
#include "ast.h"

namespace AbstractSyntaxTree {
    struct SymbolTableItem {
        std::unique_ptr<TypeBase> type;
        bool isConstant;
        std::string oriVal;

        SymbolTableItem() {}

        SymbolTableItem(std::unique_ptr<TypeBase> &&type, bool isConstant, std::string oriVal) : type(std::move(type)),
                                                                                                 isConstant(isConstant),
                                                                                                 oriVal(oriVal) {}
    };

    typedef std::map<std::string, SymbolTableItem> SymbolMap;

    struct SymbolScope {
        SymbolMap symbolMap;
        std::shared_ptr<SymbolScope> upperScope;
        std::vector<std::shared_ptr<SymbolScope>> subScope;
        int layer;
        int curSubCnt;

        SymbolScope() : layer(0), curSubCnt(0) {}


        SymbolScope(std::shared_ptr<SymbolScope> &upperScope, int layer) : upperScope(upperScope), layer(layer),
                                                                           curSubCnt(0) {}
    };

    class SymbolTable {
    public:
        SymbolTable() {
            rootScope = curScope = std::make_shared<SymbolScope>();
        }

        void PushMap() {
            curScope->subScope.push_back(std::make_shared<SymbolScope>(curScope, curScope->layer + 1));
        }

        void Step() {
            if (curScope->curSubCnt < curScope->subScope.size())
                curScope = curScope->subScope[curScope->curSubCnt++];
        }

        void PopMap() {
            if (curScope->upperScope != nullptr) {
                curScope->curSubCnt = 0;
                curScope = curScope->upperScope;
            }
        }

        void Reset() {
            curScope->curSubCnt = 0;
        }

        SymbolMap::iterator FindSymbol(std::string id, bool &has, int &layer) {
            SymbolMap::iterator ret;
            has = false;
            for (auto scope = curScope; scope != nullptr; scope = scope->upperScope) {
                ret = scope->symbolMap.find(id);
                layer = scope->layer;
                if (ret != scope->symbolMap.end()) {
                    has = true;
                    break;
                }
            }
            return ret;
        }

        bool HasSymbol(std::string id, int &layer) {
            bool ret = false;
            for (auto scope = curScope; scope != nullptr; scope = scope->upperScope) {
                layer = scope->layer;
                if (scope->symbolMap.find(id) != scope->symbolMap.end()) {
                    ret = true;
                    break;
                }
            }
            return ret;
        }

        bool SymbolAtCurScope(std::string id) {
            auto &cur = curScope->symbolMap;
            if (cur.find(id) != cur.end())
                return true;
            return false;
        }

        void InsertSymbol(std::string id, std::unique_ptr<TypeBase> &&type, bool isConstant, std::string oriVal) {
            curScope->symbolMap[id] = SymbolTableItem(std::move(type), isConstant, oriVal);
        }

        void Print(std::ostream& out = std::cout) const {
            out << "===== Symbol Table =====\n";
            PrintScope(rootScope, 0);
            out << "=======================\n";
        }
    private:
        std::shared_ptr<SymbolScope> rootScope;
        std::shared_ptr<SymbolScope> curScope;

        void PrintScope(std::shared_ptr<SymbolScope> scope, int indent) const {
            std::string indentStr(indent * 2, ' ');

            std::cout << indentStr << "┌─ Scope (layer " << scope->layer << ")\n";

            if (scope->symbolMap.empty()) {
                std::cout << indentStr << "│  (no symbols)\n";
            } else {
                for (const auto& [name, item] : scope->symbolMap) {
                    std::cout << indentStr << "├─ " << name << ": "
                              << (item.isConstant ? "[const] " : "[var]   ")
                              << (item.type ? item.type->ToString() : "[null type]");

                    if (!item.oriVal.empty()) {
                        std::cout << " (value: " << item.oriVal << ")";
                    }
                    std::cout << "\n";
                }
            }

            std::cout << indentStr << "│\n";
            for (size_t i = 0; i < scope->subScope.size(); ++i) {
                const bool last = (i == scope->subScope.size() - 1);
                std::cout << indentStr << (last ? "└─▶ " : "├─▶ ");
                PrintScope(scope->subScope[i], indent + (last ? 1 : 2));
            }

            if (scope->subScope.empty()) {
                std::cout << indentStr << "└─ (no subscopes)\n";
            }
        }

    };
}

#endif