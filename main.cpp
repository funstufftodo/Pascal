#include <iostream>
#include <fstream>
#include <string>
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
#endif

using namespace Utils;

std::string GetOutputFilename(const std::string& inputPath) {
    size_t lastDot = inputPath.find_last_of(".");
    if (lastDot != std::string::npos) {
        return inputPath.substr(0, lastDot) + ".c";
    }
    return inputPath + ".c";
}

int main(int argc, char* argv[]) {
    std::string inputPath = "../test.txt";  // 默认值
    std::string outputPath;

    // 解析命令行参数
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-i" && i+1 < argc) {
            inputPath = argv[++i];
            outputPath = GetOutputFilename(inputPath);
        }
    }

    if (outputPath.empty()) {
        outputPath = GetOutputFilename(inputPath);
    }

#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    EnableConsoleColors();
#endif

    AddConstantSymbols();
    LoadSymbols("../utils/grammar.txt");
    ElimLeftRecur();
    CombineLeftCommon();
    GenFirst();
    GenFollow(SymbolNameMap.find("programstruct")->second);
    GenLL1();
    SaveLL1Table("../utils/ll1_table", "../utils/sync.txt");

    CompilerFront::Parser parser(
            inputPath.c_str(),    // 使用命令行输入的路径
            "./reduced.txt",
            "./ll1_table.txt",
            "./sync.txt",
            "programstruct");


    bool ok;
    AbstractSyntaxTree::AbstractSyntaxTree ast(parser.Parse(ok));
    if (!ok)
        return 0;
    //ast.astRoot->FormatShow(0);
    bool result = ast.Check();
    //ast.symTable.Print();

    if (result) {
        std::ofstream outFile(outputPath);
        if (outFile.is_open()) {
            outFile << ast.GenCCode();
            std::cout << "Output generated to: " << outputPath << std::endl;
        } else {
            std::cerr << "Failed to create output file: " << outputPath << std::endl;
        }
    }
    return 0;
}