#pragma once

#include <string>
#include <vector>

#include <llir_operand.hpp>

namespace LLIR {

// Enumerations
enum class DataType {
    Void,
    I8,
    I16,
    I32,
    I64,
    F32,
    F64,
    Ptr,
    Struct
};

enum class Linkage {
    Global,
    Local,
    Extern
};

enum class InstrType {
    None,
    
    // Returns
    Ret,
    RetVoid,
    
    // Integer math
    Add,
    Sub,
    SMul,
    UMul,
    SDiv,
    UDiv,
    SRem,
    URem,
    
    // Bitwise operations
    And,
    Or,
    Xor,
    Not,
    
    // Jumps
    // We're going to use RISC-V style because these will be the easiest
    // to translate on different architectures
    Br,
    Beq,
    Bne,
    Bgt,
    Blt,
    Bge,
    Ble,
    
    // Function calls
    Call,
    
    // Memory
    Alloca,
    StructLoad,
    Load,
    GEP,
    StructStore,
    Store
};

// Forward declarations
class Function;
class Block;
class Instruction;
class Operand;
class Reg;
class StringPtr;

//
// Represents a data type
//
class Type {
public:
    explicit Type(DataType type) {
        this->type = type;
    }
    
    // Convience creators for types
    static Type *createVoidType() { return new Type(DataType::Void); }
    static Type *createI8Type() { return new Type(DataType::I8); }
    static Type *createI16Type() { return new Type(DataType::I16); }
    static Type *createI32Type() { return new Type(DataType::I32); }
    static Type *createI64Type() { return new Type(DataType::I64); }
    
    DataType getType() { return type; }
    
    virtual void print();
protected:
    explicit Type() {}
    DataType type = DataType::Void;
};

class PointerType : public Type {
public:
    explicit PointerType(Type *baseType);
    explicit PointerType(DataType type);
    ~PointerType();
    
    static PointerType *createVoidPtrType() { return new PointerType(DataType::Void); }
    static PointerType *createI8PtrType() { return new PointerType(DataType::I8); }
    static PointerType *createI16PtrType() { return new PointerType(DataType::I16); }
    static PointerType *createI32PtrType() { return new PointerType(DataType::I32); }
    static PointerType *createI64PtrType() { return new PointerType(DataType::I64); }
    
    Type *getBaseType() { return baseType; }
    
    void print();
private:
    Type *baseType = nullptr;
};

class StructType : public Type {
public:
    explicit StructType(std::string name, std::vector<Type *> elementTypes);
    ~StructType();
    
    std::vector<Type *> getElementTypes();
    
    void print();
private:
    std::string name = "";
    std::vector<Type *> elementTypes;
};

//
// The base class for instructions
//
class Instruction {
public:
    explicit Instruction(InstrType type);
    ~Instruction();
    
    void setDataType(Type *d);
    void setDest(Operand *d);
    void setOperand1(Operand *o);
    void setOperand2(Operand *o);
    void setOperand3(Operand *o);
    
    InstrType getType();
    Type *getDataType();
    Operand *getDest();
    Operand *getOperand1();
    Operand *getOperand2();
    Operand *getOperand3();
    
    virtual void print();
protected:
    Type *dataType;
    InstrType type = InstrType::None;
    Operand *dest = nullptr;
    Operand *src1 = nullptr;
    Operand *src2 = nullptr;
    Operand *src3 = nullptr;
};

// Represents function calls
class FunctionCall : public Instruction {
public:
    explicit FunctionCall(std::string name, std::vector<Operand *> args);
    
    void setArgs(std::vector<Operand *> args);
    std::string getName();
    std::vector<Operand *> getArgs();
    
    void print();
private:
    std::string name = "";
    std::vector<Operand *> args;
};

//
// Represents a basic block
//
class Block {
public:
    explicit Block(std::string name);
    ~Block();
    
    void addInstruction(Instruction *i);
    void setID(int id);
    
    std::string getName();
    int getID();
    
    int getInstrCount();
    Instruction *getInstruction(int pos);
    
    void print();
private:
    std::string name = "";
    std::vector<Instruction *> instrs;
    int id = 0;
};

//
// Represents a function
//
// TODO: We can probably get rid of stack
class Function {
public:
    explicit Function(std::string name, Linkage linkage);
    ~Function();
    
    static Function *Create(std::string name, Linkage linkage, Type *dataType) {
        Function *func = new Function(name, linkage);
        func->setDataType(dataType);
        return func;
    }
    
    void setDataType(Type *d);
    void setArgs(std::vector<Type *> args);
    void addArgPair(Type *type, Reg *reg);
    void addBlock(Block *block);
    
    void addBlockAfter(Block *block, Block *newBlock);
    
    std::string getName();
    Linkage getLinkage();
    
    int getBlockCount();
    Block *getBlock(int pos);
    
    int getArgCount();
    Type *getArgType(int pos);
    Reg *getArg(int pos);
    
    void print();
private:
    Type *dataType;
    std::string name = "";
    Linkage linkage = Linkage::Local;
    std::vector<Block *> blocks;
    std::vector<Type *> args;
    std::vector<Reg *> varRegs;
    int blockID = 1;
};

//
// The base of all LLIR projects
//
class Module {
public:
    explicit Module(std::string name);
    ~Module();
    
    void addFunction(Function *func);
    void addStringPtr(StringPtr *ptr);
    
    std::string getName();
    int getFunctionCount();
    Function *getFunction(int pos);
    int getStringCount();
    StringPtr *getString(int pos);
    
    Function *getFunctionByName(std::string fname);
    
    void transform();
    
    void print();
private:
    std::string name = "";
    std::vector<Function *> functions;
    std::vector<StringPtr *> strings;
};

} // end namespace LLIR

