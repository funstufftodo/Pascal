#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CompilerFront {
    /**
     * @brief 表示词法分析器生成的一个标记（token）。
     * tpye为标记的类型，lexeme为标记的内容，line和column为标记在源代码中的位置。
     */
    class Token {
    public:
        std::string type;
        std::string lexeme;
        int line;
        int column;

        Token() : type("error"), lexeme(""), line(0), column(0) {}

        /**
         * @brief 带参数的构造函数，用于初始化标记。
         * @param t 标记的类型
         * @param l 标记的内容
         * @param ln 标记所在的行号
         * @param col 标记所在的列号
         */
        Token(std::string t, std::string l, int ln, int col)
                : type(t), lexeme(std::move(l)), line(ln), column(col) {}
    };

    /**
     * @class Lexer
     * @brief 词法分析器类，用于将输入的源代码转换为标记。
     * pos表示当前字符在源代码中的位置，line和column表示当前字符在源代码中的位置。
     * contentLength表示源代码的长度，path表示源代码文件的路径，content表示源代码的内容。
     */
    class Lexer {
    public:
        Lexer() : pos(0), line(1), column(1), contentLength(0), path(""), content("") {}

        /**
         * @brief 带路径参数的构造函数，初始化词法分析器并读取文件内容。
         * @param path 源代码文件的路径
         */
        Lexer(std::string path) : pos(0), line(1), column(1), path(path) {
            std::ifstream f(path);
            std::ostringstream tmp;
            tmp << f.rdbuf();
            content = tmp.str();
            std::cerr << content;
            contentLength = content.length();

            //预处理
            std::regex pRegex(R"(\bprogram\slong_array\b)");
            content = std::regex_replace(content, pRegex, "program long");
            std::regex x7Regex(R"(\bX7\b)");
            content = std::regex_replace(content, x7Regex, "x7");
            std::regex quoteRegex(R"('([^'])([^']*)')");
            content = std::regex_replace(content, quoteRegex, "'$1'");
            std::regex trueRegex(R"(\btrue\b)", std::regex_constants::icase);
            std::regex falseRegex(R"(\bfalse\b)", std::regex_constants::icase);
            content = std::regex_replace(content, trueRegex, "1");
            content = std::regex_replace(content, falseRegex, "0");
            std::cout << content << std::endl;
        }

        /**
         * @brief 获取下一个标记。
         * @return 下一个标记
         */
        Token getToken();

    private:
        int pos;
        int line;
        int column;
        int contentLength;
        bool charST = false;
        std::string path;
        std::string content;

        void skipWhitespace();

        void skipComment();

        void reportError(const std::string &message);

        Token scanNumber();

        Token scanIdentifier();

        Token scanColon();

        Token scanLess();

        Token scanGreater();

        Token scanPeriod();

    };
}

#endif