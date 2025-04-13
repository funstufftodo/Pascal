#include "types.h"
#include <iostream>

namespace AST {
    inline bool isRelop(const std::string &op) {
        if (op == "=" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "<>")
            return true;
        return false;
    }

    std::unique_ptr<TypeBase>
    TypeBase::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        ok = false;
        errMsg = std::string("operation ") + op + " is not compatiable with type " + ToString();
        if (isRelop(op))
            return GenType(BOOLEAN);

        if (anotherType->IsBasicType())
            return std::move(anotherType);

        return GenType(VOID);
    }

    std::unique_ptr<TypeBase>
    TypeBase::CalcFuncType(std::unique_ptr<TupleType> &&argTypes, bool &ok, std::string &errMsg) {
        ok = false;
        errMsg = std::string("type ") + ToString() + " is not callable";
        return GenType(VOID);
    }

    std::unique_ptr<TypeBase>
    TypeBase::CalcArrayType(std::unique_ptr<TupleType> &&elemTypes, bool &ok, std::string &errMsg) {
        ok = false;
        errMsg = std::string("type ") + ToString() + " is not subscriptable";
        return GenType(VOID);
    }
}
