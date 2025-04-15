cmake_minimum_required(VERSION 3.28)
project(compiler)

set(CMAKE_CXX_STANDARD 17)

add_executable(compiler main.cpp
        utils/utils.h
        utils/left_recursion.cpp
        utils/ll1_constructor.cpp
        types.h
        types.cpp
        lexer.h
        lexer.cpp
        symboltable.h
        ast.h
        ast.cpp
)
