#ifndef TYPES
#define TYPES

#include <memory>
#include <iostream>
#include <vector>
#include <functional>

namespace AbstractSyntaxTree {
    template<typename To, typename From>
    std::unique_ptr<To> UniquePtrCast(std::unique_ptr<From> &&ptr) {
        if (auto p = dynamic_cast<To *>(ptr.get())) {
            ptr.release();
            return std::unique_ptr<To>(p);
        }
        return nullptr;
    }

    template<typename To, typename From>
    inline std::unique_ptr<To> UniquePtrCast(std::unique_ptr<From> &ori) {
        if (auto p = dynamic_cast<To *>(ori.get())) {
            ori.release();
            return std::unique_ptr<To>(p);
        }
        return nullptr;
    }

    enum TypeID {
        VOID,
        BOOLEAN,
        INTEGER,
        REAL,
        CHAR,
        LVALUE,
        RVALUE,
        REF,
        FUNC,
        ARRAY,
        TUPLE
    };

    class TupleType;

    class TypeBase {
    public:
        virtual std::unique_ptr<TypeBase>
        CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &error_msg);

        virtual std::unique_ptr<TypeBase>
        CalcFuncType(std::unique_ptr<TupleType> &&argTypes, bool &ok, std::string &error_msg);

        virtual std::unique_ptr<TypeBase>
        CalcArrayType(std::unique_ptr<TupleType> &&elemTypes, bool &ok, std::string &error_msg);

        virtual bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) = 0;

        virtual bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) = 0;

        virtual std::unique_ptr<TypeBase> Copy() = 0;

        virtual std::string ToString() = 0;

        TypeBase() = default;

        virtual ~TypeBase() = default;

        explicit TypeBase(TypeID id) : id(id) {}

        TypeID GetTypeId() {
            return id;
        }

        bool IsBasicType() {
            if (id == BOOLEAN || id == INTEGER || id == REAL || id == CHAR) {
                return true;
            }
            return false;
        }

        bool IsWrapperType() {
            if (id == LVALUE || id == RVALUE || id == REF) {
                return true;
            }
            return false;
        }

        bool IsVoidType() {
            if (id == VOID) {
                return true;
            }
            return false;
        }

    private:
        TypeID id = VOID;
    };

    class WrapperType : public TypeBase {
    public:
        WrapperType() : TypeBase(VOID) {}

        virtual ~WrapperType() = default;

        WrapperType(TypeID id) : TypeBase(id) {}

        WrapperType(std::unique_ptr<TypeBase> &&target, TypeID id) : targetType(std::move(target)), TypeBase(id) {
            if (targetType->IsWrapperType())
                targetType = UniquePtrCast<WrapperType>(targetType)->DeWrap();
        }

        std::unique_ptr<TypeBase> DeWrap() {
            //std::cout << "DEWRAP " << targetType->GetTypeId() << std::endl;
            return targetType->Copy();
        }

        virtual std::unique_ptr<TypeBase>
        CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) override;

        virtual std::unique_ptr<TypeBase>
        CalcFuncType(std::unique_ptr<TupleType> &&argTypes, bool &ok, std::string &errMsg) override;

        virtual std::unique_ptr<TypeBase>
        CalcArrayType(std::unique_ptr<TupleType> &&elemTypes, bool &ok, std::string &errMsg) override;

    protected:
        std::unique_ptr<TypeBase> targetType;
    };

    class LValueType : public WrapperType {
    public:
        LValueType() : WrapperType(LVALUE) {}

        LValueType(std::unique_ptr<TypeBase> &&targetType) : WrapperType(std::move(targetType), LVALUE) {
        }

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class RValueType : public WrapperType {
    public:
        RValueType() : WrapperType(RVALUE) {}

        RValueType(std::unique_ptr<TypeBase> &&targetType) : WrapperType(std::move(targetType), RVALUE) {
        }

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class RefType : public WrapperType {
    public:
        RefType() : WrapperType(REF) {}

        RefType(std::unique_ptr<TypeBase> &&targetType) : WrapperType(std::move(targetType), REF) {
        }

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class VOIDType : public TypeBase {
    public:
        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class BooleanType : public TypeBase {
    public:
        virtual std::unique_ptr<TypeBase>
        CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) override;

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        BooleanType() : TypeBase(BOOLEAN) {}

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class IntegerType : public TypeBase {
    public:
        virtual std::unique_ptr<TypeBase>
        CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) override;

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        IntegerType() : TypeBase(INTEGER) {}

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class RealType : public TypeBase {
    public:
        std::unique_ptr<TypeBase>
        CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) override;

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        RealType() : TypeBase(REAL) {}

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class CharType : public TypeBase {
    public:
        std::unique_ptr<TypeBase>
        CalcType(std::unique_ptr<TypeBase> &&anotherType, std::string op, bool &ok, std::string &errMsg) override;

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        CharType() : TypeBase(CHAR) {}

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;
    };

    class TupleType : public TypeBase {
    public:
        TupleType() : TypeBase(TUPLE) {}

        TupleType(std::vector<std::unique_ptr<TypeBase>> &types) : TypeBase(TUPLE) {
            for (const auto &type: types) {
                subTypes.push_back(type->Copy());
            }
        }

        TupleType(std::vector<std::unique_ptr<TypeBase>> &&types) : TypeBase(TUPLE) {
            for (auto &type: types) {
                subTypes.push_back(std::move(type));
            }
        }

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        std::vector<std::unique_ptr<TypeBase>> GetSubTypes() {
            std::vector<std::unique_ptr<TypeBase>> result;
            for (auto &type: subTypes)
                result.push_back(type->Copy());
            return std::move(result);
        }

    private:
        std::vector<std::unique_ptr<TypeBase>> subTypes;
    };

    class FuncType : public TypeBase {
    public:
        std::unique_ptr<TypeBase>
        CalcFuncType(std::unique_ptr<TupleType> &&argTypes, bool &ok, std::string &errMsg) override;

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        FuncType() : TypeBase(FUNC) {}

        FuncType(std::unique_ptr<TupleType> &&argTypes, std::unique_ptr<TypeBase> &&retType)
                : argTypes(std::move(argTypes)), retType(std::move(retType)), TypeBase(FUNC) {}

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        std::vector<std::unique_ptr<TypeBase>> GetArgTypes() {
            return argTypes->GetSubTypes();
        }

        std::unique_ptr<TypeBase> RetAsLValue() {
            TypeBase *lval = new LValueType(retType->Copy());
            return std::unique_ptr<TypeBase>(lval);
        }

    private:
        std::unique_ptr<TupleType> argTypes;
        std::unique_ptr<TypeBase> retType;
    };

    class ArrayType : public TypeBase {
    public:
        std::unique_ptr<TypeBase>
        CalcArrayType(std::unique_ptr<TupleType> &&elemTypes, bool &ok, std::string &errMsg) override;

        std::unique_ptr<TypeBase> Copy() override;

        std::string ToString() override;

        ArrayType() : TypeBase(ARRAY) {}

        ArrayType(std::vector<std::pair<int, int>> &dimensions, std::unique_ptr<TypeBase> &&contentType)
                : dimensions(dimensions), contentType(std::move(contentType)), TypeBase(ARRAY) {}

        bool InitCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        bool AssignCompatible(std::unique_ptr<TypeBase> &&anotherType, std::string &errMsg) override;

        std::vector<int> GetOffset() {
            std::vector<int> result;
            for (auto &dim: dimensions)
                result.push_back(dim.first);
            return result;
        }

        int GetContentType() {
            return contentType->GetTypeId();
        }

    private:
        std::vector<std::pair<int, int>> dimensions;
        std::unique_ptr<TypeBase> contentType;
    };

    inline std::unique_ptr<TypeBase> GenType(TypeID id) {
        static const std::unordered_map<TypeID, std::function<std::unique_ptr<TypeBase>()>> factoryMap = {
                {TypeID::INTEGER, [] { return std::make_unique<IntegerType>(); }},
                {TypeID::REAL,    [] { return std::make_unique<RealType>(); }},
                {TypeID::CHAR,    [] { return std::make_unique<CharType>(); }},
                {TypeID::BOOLEAN, [] { return std::make_unique<BooleanType>(); }},
                {TypeID::VOID,    [] { return std::make_unique<VOIDType>(); }}
        };

        auto it = factoryMap.find(id);
        return it != factoryMap.end() ? it->second() : std::make_unique<VOIDType>();
    }

    inline std::unique_ptr<TypeBase> GenTypeByStr(const std::string &id) {
        static const std::unordered_map<std::string, TypeID> typeMap = {
                {"integer", TypeID::INTEGER},
                {"int",     TypeID::INTEGER},
                {"real",    TypeID::REAL},
                {"float",   TypeID::REAL},
                {"char",    TypeID::CHAR},
                {"boolean", TypeID::BOOLEAN}
        };

        auto it = typeMap.find(id);
        return it != typeMap.end() ? GenType(it->second) : GenType(TypeID::VOID);
    }

}

#endif