#include "types.h"
#include <iostream>

namespace AbstractSyntaxTree {
    inline bool isRelop(const std::string &op) {
        if (op == "=" || op == "<" || op == ">" || op == "<=" || op == ">=" || op == "<>")
            return true;
        return false;
    }

    std::unique_ptr<TypeBase>
    TypeBase::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        ok = false;
        errMsg = std::string("operation ") + op + " is not compatible with type " + ToString();
        if (isRelop(op)) {
            return GenType(BOOLEAN);
        }

        if (anotherType->IsBasicType()) {
            return std::move(anotherType);
        }
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

    std::unique_ptr<TypeBase>
    WrapperType::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        if (anotherType->IsWrapperType()) {
            anotherType = UniquePtrCast<WrapperType>(anotherType)->DeWrap();
        }
        TypeBase *result = new RValueType(targetType->CalcType(std::move(anotherType), op, ok, errMsg));
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    WrapperType::CalcFuncType(std::unique_ptr<TupleType> &&argTypes, bool &ok, std::string &errMsg) {
        TypeBase *result = new RValueType(targetType->CalcFuncType(std::move(argTypes), ok, errMsg));
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    WrapperType::CalcArrayType(std::unique_ptr<TupleType> &&elemTypes, bool &ok, std::string &errMsg) {
        TypeBase *result = new LValueType(targetType->CalcArrayType(std::move(elemTypes), ok, errMsg));
        return std::unique_ptr<TypeBase>(result);
    }

    std::string LValueType::ToString() {
        return "LVal(" + targetType->ToString() + ")";
    }

    std::unique_ptr<TypeBase> LValueType::Copy() {
        TypeBase *result = new LValueType(targetType->Copy());
        return std::unique_ptr<TypeBase>(result);
    }

    bool LValueType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (!AssignCompatible(std::move(anotherType), errMsg)) {
            errMsg = std::string("type ") + ToString() + " cannot be initialized by type " + anotherType->ToString();
            return false;
        }
        return true;
    }

    bool LValueType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        bool compatible;
        std::string str = anotherType->ToString();
        if (anotherType->IsWrapperType()) {
            auto another(UniquePtrCast<WrapperType>(anotherType)->DeWrap());
            compatible = targetType->AssignCompatible(std::move(another), errMsg);
        } else {
            compatible = targetType->AssignCompatible(std::move(anotherType), errMsg);
        }
        if (!compatible)
            errMsg = std::string("type ") + ToString() + " cannot be assigned by type " + str;
        return compatible;
    }

    std::unique_ptr<TypeBase> RValueType::Copy() {
        TypeBase *result = new RValueType(targetType->Copy());
        return std::unique_ptr<TypeBase>(result);
    }

    std::string RValueType::ToString() {
        return "RVal(" + targetType->ToString() + ")";
    }

    bool RValueType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = std::string("type ") + ToString() + " cannot be initialized by type " + anotherType->ToString();
        return false;
    }

    bool RValueType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = std::string("type ") + ToString() + " cannot be assigned by type " + anotherType->ToString();
        return false;
    }

    std::unique_ptr<TypeBase> RefType::Copy() {
        TypeBase *result = new RefType(targetType->Copy());
        return std::unique_ptr<TypeBase>(result);
    }

    std::string RefType::ToString() {
        return "Ref(" + targetType->ToString() + ")";
    }

    bool RefType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        bool compatible;
        std::string str = anotherType->ToString();
        if (anotherType->GetTypeId() == REF || anotherType->GetTypeId() == LVALUE) {
            auto another(UniquePtrCast<WrapperType>(anotherType)->DeWrap());
            compatible = targetType->InitCompatible(std::move(another), errMsg);
        } else {
            compatible = false;
        }
        if (!compatible)
            errMsg = "type " + ToString() + " cannot be initialized by type " + str;
        return compatible;
    }

    bool RefType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        bool compatiable;
        std::string str = anotherType->ToString();
        if (anotherType->IsWrapperType()) {
            auto ano(UniquePtrCast<WrapperType>(anotherType)->DeWrap());
            compatiable = targetType->AssignCompatible(std::move(ano), errMsg);
        } else {
            compatiable = targetType->AssignCompatible(std::move(anotherType), errMsg);
        }
        if (!compatiable)
            errMsg = "type " + ToString() + " cannot be assigned by type " + str;
        return compatiable;
    }

    std::string VOIDType::ToString() {
        return "VOID";
    }

    std::unique_ptr<TypeBase> VOIDType::Copy() {
        TypeBase *result = new VOIDType();
        return std::unique_ptr<TypeBase>(result);
    }

    bool VOIDType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }

    bool VOIDType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
        return false;
    }

    std::string BooleanType::ToString() {
        return "BOOLEAN";
    }

    std::unique_ptr<TypeBase> BooleanType::Copy() {
        TypeBase *result = new BooleanType();
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    BooleanType::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            if (op == "/") {
                return GenType(REAL);
            }
            if (isRelop(op)) {
                return GenType(BOOLEAN);
            }
            if (anotherType->GetTypeId() == REAL) {
                if (op == "mod" || op == "div") {
                    ok = false;
                    errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
                    return GenType(VOID);
                }
                return std::move(anotherType);
            }
            return GenType(INTEGER);
        }
        ok = false;
        errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
        return GenType(VOID);
    }

    bool BooleanType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (!AssignCompatible(std::move(anotherType), errMsg)) {
            errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
            return false;
        }
        return true;
    }

    bool BooleanType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            return true;
        }
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }

    std::string IntegerType::ToString() {
        return "INTEGER";
    }

    std::unique_ptr<TypeBase> IntegerType::Copy() {
        TypeBase *result = new IntegerType();
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    IntegerType::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            if (op == "/") {
                return GenType(REAL);
            }
            if (isRelop(op)) {
                return GenType(BOOLEAN);
            }
            if (anotherType->GetTypeId() == REAL) {
                if (op == "mod" || op == "div") {
                    ok = false;
                    errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
                    return GenType(VOID);
                }
                return std::move(anotherType);
            }
            return GenType(INTEGER);
        }
        ok = false;
        errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
        return GenType(VOID);
    }

    bool IntegerType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (!AssignCompatible(std::move(anotherType), errMsg)) {
            errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
            return false;
        }
        return true;
    }

    bool IntegerType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            if(anotherType->GetTypeId() == REAL) {
                errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
                return false;
            }
            return true;
        }
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }

    std::string RealType::ToString() {
        return "REAL";
    }

    std::unique_ptr<TypeBase> RealType::Copy() {
        TypeBase *result = new RealType();
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    RealType::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        if (op == "/" && anotherType->IsBasicType()) {
            return GenType(REAL);
        }

        if (op == "mod" || op == "div") {
            ok = false;
            errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
            return GenType(VOID);
        }

        if (anotherType->IsBasicType()) {
            return GenType(REAL);
        }

        ok = false;
        errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
        return GenType(VOID);
    }

    bool RealType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (!AssignCompatible(std::move(anotherType), errMsg)) {
            errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
            return false;
        }
        return true;
    }

    bool RealType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            return true;
        }
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }

    std::string CharType::ToString() {
        return "CHAR";
    }

    std::unique_ptr<TypeBase> CharType::Copy() {
        TypeBase *result = new CharType();
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    CharType::CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            if (op == "/") {
                return GenType(REAL);
            }
            if (isRelop(op)) {
                return GenType(BOOLEAN);
            }
            if (anotherType->GetTypeId() == REAL) {
                if (op == "mod" || op == "div") {
                    ok = false;
                    errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
                    return GenType(VOID);
                }
                return std::move(anotherType);
            }
            if (anotherType->GetTypeId() == BOOLEAN) {
                return GenType(BOOLEAN);
            }
            return GenType(INTEGER);
        }
        ok = false;
        errMsg = "operation " + op + "is not compatible with type " + anotherType->ToString();
        return GenType(VOID);
    }

    bool CharType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (!AssignCompatible(std::move(anotherType), errMsg)) {
            errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
            return false;
        }
        return true;
    }

    bool CharType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (anotherType->IsBasicType()) {
            return true;
        }
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }

    std::string TupleType::ToString() {
        std::string result = "TUPLE(";
        for (auto &subType: subTypes) {
            result += subType->ToString();
        }
        return result + ")";
    }

    std::unique_ptr<TypeBase> TupleType::Copy() {
        TypeBase *result = new TupleType(subTypes);
        return std::unique_ptr<TypeBase>(result);
    }

    bool TupleType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if(anotherType == nullptr) {
            return true;
        }
        if (anotherType->GetTypeId() != TUPLE) {
            errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
            return false;
        }

        auto another(UniquePtrCast<TupleType>(anotherType));
        if (another->subTypes.size() != subTypes.size()) {
            if (anotherType == nullptr) {
                errMsg = "Type " + ToString() + "cannot be initialized by null";
                return false;
            }
            errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
            return false;
        }

        for (int i = 0; i < subTypes.size(); i++) {
            if (!subTypes[i]->AssignCompatible(std::move(another->subTypes[i]), errMsg)) {
                errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
                return false;
            }
        }
        return true;
    }

    bool TupleType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        if (anotherType->GetTypeId() != TUPLE) {
            errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
            return false;
        }

        auto another(UniquePtrCast<TupleType>(anotherType));
        if (another->subTypes.size() != subTypes.size()) {
            errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
            return false;
        }

        for (int i = 0; i < subTypes.size(); i++) {
            if (!subTypes[i]->AssignCompatible(std::move(another->subTypes[i]), errMsg)) {
                errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
                return false;
            }
        }
        return true;
    }

    std::string FuncType::ToString() {
        if(argTypes == nullptr || retType == nullptr) {
            return "FUNCTION";
        }
        return "FUNCTION " + argTypes->ToString() + retType->ToString();
    }

    std::unique_ptr<TypeBase> FuncType::Copy() {
        TypeBase *result;
        if(argTypes == nullptr) {
            result = new FuncType(std::make_unique<TupleType>(), retType->Copy());
        } else {
            result = new FuncType(UniquePtrCast<TupleType>(argTypes->Copy()), retType->Copy());
        }
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    FuncType::CalcFuncType(std::unique_ptr<TupleType> &&argTypes, bool &ok, std::string &errMsg) {
        if(argTypes.get() == nullptr || this->argTypes == nullptr) {
            return retType->Copy();
        }
        if (!this->argTypes->InitCompatible(std::move(argTypes), errMsg)) {
            ok = false;
            errMsg = "Function args not compatible " + errMsg;
        }
        return retType->Copy();
    }

    bool FuncType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
        return false;
    }

    bool FuncType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }

    std::string ArrayType::ToString() {
        std::string result = contentType->ToString() + "[";
        for (auto &dim: dimensions) {
            result += std::to_string(dim.first) + ".." + std::to_string(dim.second) + ",";
        }
        return result + "]";
    }

    std::unique_ptr<TypeBase> ArrayType::Copy() {
        TypeBase *result = new ArrayType(dimensions, contentType->Copy());
        return std::unique_ptr<TypeBase>(result);
    }

    std::unique_ptr<TypeBase>
    ArrayType::CalcArrayType(std::unique_ptr<TupleType> &&elemTypes, bool &ok, std::string &errMsg) {
        auto subTypes(elemTypes->GetSubTypes());
        if (subTypes.size() != dimensions.size()) {
            ok = false;
            errMsg = "Subscript dimension mismatch, meet: " + std::to_string(subTypes.size()) + "expecting: " +
                     std::to_string(dimensions.size());
        } else {
            for (auto &subType: subTypes) {
                auto targetType = ((WrapperType *) subType.get())->DeWrap();
                if (!targetType->IsBasicType() || (targetType->GetTypeId() == REAL)) {
                    ok = false;
                    errMsg = "Subscript dimension must be integer";
                    break;
                }
            }
        }
        return contentType->Copy();
    }

    bool ArrayType::InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = "Type " + ToString() + "cannot be initialized by " + anotherType->ToString();
        return false;
    }

    bool ArrayType::AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) {
        errMsg = "Type " + ToString() + "cannot be assigned by " + anotherType->ToString();
        return false;
    }
}
