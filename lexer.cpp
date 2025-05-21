#include "lexer.h"
#include <set>

namespace CompilerFront {

    std::set<std::string> keywords = {
            "while",
            "Break",
            "not",
            "program",
            "const",
            "var",
            "procedure",
            "function",
            "begin",
            "end",
            "array",
            "of",
            "integer",
            "real",
            "boolean",
            "char",
            "if",
            "then",
            "else",
            "for",
            "while",
            "to",
            "do",
            "read",
            "write",
            "or"
    };

    std::set<std::string> mulop = {
            "div",
            "mod",
            "and"
    };

    inline bool isDigit(char c) {
        return c >= '0' && c <= '9';
    }

    inline bool isAlpha(char c) {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    Token Lexer::getToken() {

        if(!charST) {
            skipWhitespace();
        }

        if (pos >= contentLength) {
            return Token("$", "$", line, column);
        }

        char curChar = content[pos];

        if (charST) {
            pos++;
            if (curChar == '\'') {
                charST = false;
                return Token(std::string(1, curChar), std::string(1, curChar), line, column++);
            }
            return Token("letter", std::string(1, curChar), line, column++);
        }

        if (isDigit(curChar)) {
            return scanNumber();
        }

        if (isAlpha(curChar)) {
            return scanIdentifier();
        }

        switch (curChar) {
            case '\'':
                pos++;
                charST = true;
                return Token(std::string(1, curChar), std::string(1, curChar), line, column++);
            case '+':
                pos++;
                return Token("+", "+", line, column++);
            case '-':
                pos++;
                return Token("-", "-", line, column++);
            case '*':
                pos++;
                return Token("mulop", "*", line, column++);
            case '/':
                pos++;
                return Token("mulop", "/", line, column++);
            case '(':
                pos++;
                return Token("(", "(", line, column++);
            case ')':
                pos++;
                return Token(")", ")", line, column++);
            case '[':
                pos++;
                return Token("[", "[", line, column++);
            case ']':
                pos++;
                return Token("]", "]", line, column++);
            case ',':
                pos++;
                return Token(",", ",", line, column++);
            case ';':
                pos++;
                column++;
                return Token(";", ";", line, column++);
            case '=':
                pos++;
                return Token("=", "=", line, column++);
            case ':':
                return scanColon();
            case '<':
                return scanLess();
            case '>':
                return scanGreater();
            case '.':
                return scanPeriod();
            default:
                pos++;
                reportError("Unexpected character");
                return Token("error", std::string(1, curChar), line, column++);
        }
    }

    void Lexer::skipWhitespace() {
        while (pos < contentLength) {
            char c = content[pos];

            if (c == ' ' || c == '\t' || c == '\r') {
                pos++;
                column++;
            } else if (c == '\n') {
                pos++;
                line++;
                column = 1;
            } else if (c == '{') {
                skipComment();
            } else if (c == '/' && pos + 1 < contentLength && content[pos + 1] == '/') {
                // 处理单行注释
                pos += 2;
                column += 2;
                while (pos < contentLength && content[pos] != '\n') {
                    pos++;
                    column++;
                }
                // 换行符也要跳过
                if (pos < contentLength && content[pos] == '\n') {
                    pos++;
                    line++;
                    column = 1;
                }
            } else {
                break;
            }
        }
    }


    Token Lexer::scanNumber() {
        std::string num;
        int startColumn = column;
        bool isFloat = false;

        while (pos < contentLength && isDigit(content[pos])) {
            num += content[pos++];
            column++;
        }

        if (pos < contentLength && content[pos] == '.' &&
            pos + 1 < contentLength && isDigit(content[pos + 1])) {
            isFloat = true;
            num += content[pos++];
            column++;

            while (pos < contentLength && isDigit(content[pos])) {
                num += content[pos++];
                column++;
            }
        }

        if (pos < contentLength && isAlpha(content[pos])) {
            reportError("Invalid character in number");
            return Token("error", num + content[pos], line, column);
        }

        return Token(isFloat ? "float" : "digits", num, line, startColumn);
    }

    Token Lexer::scanIdentifier() {
        std::string identifier;
        int startColumn = column;

        while (pos < contentLength && (isAlpha(content[pos]) || isDigit(content[pos]) || content[pos] == '_')) {
            identifier += content[pos++];
            column++;
        }

        auto it = keywords.find(identifier);
        if (it != keywords.end()) {
            return Token(identifier, identifier, line, startColumn);
        }
        it = mulop.find(identifier);
        if (it != mulop.end()) {
            return Token("mulop", identifier, line, startColumn);
        }

        return Token("id", identifier, line, startColumn);
    }

    Token Lexer::scanColon() {
        int startColumn = column;
        pos++;
        column++;

        if (pos < contentLength && content[pos] == '=') {
            pos++;
            column++;
            return Token("assignop", ":=", line, startColumn);
        }

        return Token(":", ":", line, startColumn);
    }

    Token Lexer::scanLess() {
        int startColumn = column;
        pos++;
        column++;

        if (pos < contentLength) {
            if (content[pos] == '=') {
                pos++;
                column++;
                return Token("relop", "<=", line, startColumn);
            } else if (content[pos] == '>') {
                pos++;
                column++;
                return Token("relop", "<>", line, startColumn);
            }
        }

        return Token("relop", "<", line, startColumn);
    }

    Token Lexer::scanGreater() {
        int startColumn = column;
        pos++;
        column++;

        if (pos < contentLength && content[pos] == '=') {
            pos++;
            column++;
            return Token("relop", ">=", line, startColumn);
        }

        return Token("relop", ">", line, startColumn);
    }

    Token Lexer::scanPeriod() {
        int startColumn = column;
        pos++;
        column++;

        if (pos < contentLength && content[pos] == '.') {
            pos++;
            column++;
            return Token("..", "..", line, startColumn);
        }

        return Token(".", ".", line, startColumn);
    }

    void Lexer::skipComment() {
        pos++;
        column++;

        while (pos < contentLength && content[pos] != '}') {
            if (content[pos] == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            pos++;
        }

        if (pos < contentLength) {
            pos++;
            column++;
        }
    }


    void Lexer::reportError(const std::string &message) {
        std::cerr << "Error at line " << line << ", column " << column << ": " << message << std::endl;
    }

};
