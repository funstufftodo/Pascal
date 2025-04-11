#include "types.h"
#include <iostream>

namespace AST {
    std::unique_ptr<TypeBase> TypeBase::CalcType(std::unique_ptr<TypeBase>&& anotherType, std::string op, bool& ok, std::string& errMsg)
    {
        ok = false;
        errMsg = std::string("operation ") + op + " is not compatiable with type " + ToString();
        if (isRelop(op))
            return GenType(BOOLEAN);

        if (anotherType->IsBasicType())
            return std::move(anotherType);

        return GenType(VOID);
    }
}
