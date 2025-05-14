#ifndef PARSER
#define PARSER

#include "lexer.h"
#include "utils/utils.h"
#include "ast.h"
#include <string>
#include <sstream>
#include <stack>

namespace CompilerFront {
    struct ReduceInfo {
        int reduceStackSize;
        int reduceCnt;
        std::string expressionLeft;
        std::string expressionFirst;

        ReduceInfo() {}

        ReduceInfo(int reduceStackSize,
                   int reduceCnt,
                   std::string expressionLeft,
                   std::string expressionFirst)
                : reduceStackSize(reduceStackSize),
                  reduceCnt(reduceCnt),
                  expressionLeft(expressionLeft),
                  expressionFirst(expressionFirst) {}
    };

    class Parser {
    public:
        Parser();

        Parser(std::string path, std::string symbolPath, std::string tablePath, std::string syncPath, std::string st)
                : path(path), lexer(path) {
            Utils::AddConstantSymbols();
            Utils::LoadSymbols(symbolPath);
            Utils::LoadLL1Table(tablePath, syncPath);
            symbolStack.push(Utils::SymbolNameMap.find(st)->second);
        }

        std::unique_ptr<AbstractSyntaxTree::ASTNode> Parse(bool &ok);

    private:
        std::string path;
        Lexer lexer;
        std::stack<int> symbolStack;
        std::stack<ReduceInfo> reduceStack;
        std::vector<std::unique_ptr<AbstractSyntaxTree::ASTNode>> astStack;
        Token curToken;
        Token nxtToken;

        void logErrMsg(int line, int column, int topSymbolId, int curSymbolId, bool isTermi) {
            static int preLine = 0;
            static int preColumn = 0;

            if (line == preLine && column == preColumn)
                return;

            preLine = line;
            preColumn = column;

            std::ostringstream errMsg;
            errMsg << "line: " << line << " column: " << column << " met token: ";

            errMsg << Utils::InvSymbolNameMap.find(curSymbolId)->second << ",";
            errMsg << " expecting:";
            if (isTermi) {
                errMsg << Utils::InvSymbolNameMap.find(topSymbolId)->second;
            } else {
                auto &item = Utils::LL1Table.find(topSymbolId)->second;
                for (auto &it: item)
                    errMsg << " " << Utils::InvSymbolNameMap.find(it.first)->second;
            }
            std::cout << errMsg.str() << std::endl;
        }

        void handleTerminalSymbol(int topSymbolId, int oriSymbolId, bool &ok, int &ifCnt);

        void
        handleNonTerminalSymbol(int topSymbolId, const Utils::Symbol &topSymbol, int oriSymbolId, bool &ok, int &ifCnt);

        void processReduceStack(bool ok);
    };
}

#endif