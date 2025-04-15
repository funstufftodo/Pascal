#include "ast.h"

#include <string>
#include <iostream>

namespace AST {
    void logErrMsg(int line, int column, std::string msg) {
        std::ostringstream errMsg;
        errMsg << "line: " << line << " column: " << column << " " << msg;
        std::cout << errMsg.str() << std::endl;
    }

    std::unique_ptr<ASTNode> GenOriAstNode(CompilerFront::Token &token) {
        std::string tokenType = token.type;
        if (tokenType == "relop") {
            ASTNode *oriASTNode = new OriASTNode(token.lexeme, token.lexeme, token.line, token.column);
            return std::unique_ptr<ASTNode>(oriASTNode);
        }
        if (tokenType == "digits") {
            ASTNode *oriASTNode = new OriASTNode(token.lexeme, "int", token.line, token.column);
            return std::unique_ptr<ASTNode>(oriASTNode);
        }
        if (tokenType == "float") {
            ASTNode *oriASTNode = new OriASTNode(token.lexeme, "float", token.line, token.column);
            return std::unique_ptr<ASTNode>(oriASTNode);
        }
        if (tokenType == "char") {
            ASTNode *oriASTNode = new OriASTNode(std::string(1, '\'') + token.lexeme + "\'", "char", token.line,
                                                 token.column);
            return std::unique_ptr<ASTNode>(oriASTNode);
        }
        if (tokenType == "mulop") {
            ASTNode *oriASTNode = new OriASTNode(token.type, token.lexeme, token.line, token.column);
            return std::unique_ptr<ASTNode>(oriASTNode);
        }
        if (tokenType == "id") {
            ASTNode *oriASTNode = new OriASTNode(token.lexeme, token.lexeme, token.line, token.column);
            return std::unique_ptr<ASTNode>(oriASTNode);
        }
        ASTNode *oriASTNode = new OriASTNode(token.type, token.type, token.line, token.column);
        return std::unique_ptr<ASTNode>(oriASTNode);
    }

    std::unique_ptr<ASTNode> GenAstNode(std::string expressionLeft, std::string expressionFirst,
                                        std::vector<std::unique_ptr<ASTNode>> &subNodes) {
        if (expressionLeft == "programstruct") {
            if (expressionFirst == "program_head") {
                //programstruct-->program_head ; program_body .
                ASTNode *program = new Program(Unpack<ProgramHead>(subNodes[0]), Unpack<ProgramBody>(subNodes[2]));
                return std::unique_ptr<ASTNode>(program);
            }
        }
    }
}