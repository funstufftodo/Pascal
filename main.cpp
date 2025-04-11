#include <iostream>
#include "./utils/utils.h"
#include <windows.h>

using namespace Utils;

void EnableConsoleColors() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
}

int main() {

    EnableConsoleColors();
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

    return 0;
}
