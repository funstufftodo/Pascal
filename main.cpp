#include <iostream>
#include "./utils/utils.h"
#include "parser.h"
#ifdef _WIN32
#include <windows.h>
void EnableConsoleColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}
SetConsoleOutputCP(CP_UTF8);
EnableConsoleColors();
#endif

using namespace Utils;

int main() {
    AddConstantSymbols();
    LoadSymbols("../utils/grammar.txt");
    ElimLeftRecur();
    CombineLeftCommon();
    Show();
    GenFirst();
    GenFollow(SymbolNameMap.find("programstruct")->second);
    ShowFollow();
    GenLL1();
    ShowLL1Table();
    SaveLL1Table("../utils/ll1_table", "../utils/sync.txt");

    CompilerFront::Parser parser(
            "../test.txt",
            "./reduced.txt",
            "./ll1_table.txt",
            "./sync.txt",
            "programstruct");
    std::cout << "---------------------------" << std::endl;
    std::cout << "PARSE START" << std::endl;
    bool ok;
    AbstractSyntaxTree::AbstractSyntaxTree ast(parser.Parse(ok));
    if (!ok)
        return 0;
    ast.astRoot->FormatShow(0);
    std::cout << "PARSE OVER" << std::endl;
    bool result = ast.Check();
    ast.symTable.Print();
    if (result) {
        std::cout << ast.GenCCode();
    }
    return 0;
}
