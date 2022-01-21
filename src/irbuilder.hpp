#pragma once

#include <memory>

#include <llir.hpp>

namespace LLIR {

class IRBuilder {
public:
    explicit IRBuilder(Module *mod);

    void setCurrentFunction(Function *func) {
        currentFunc = func;
        regCounter = currentFunc->getArgCount();
    };
    
    // Creates a new block and sets the insert point
    Block *createBlock(std::string name);
    void addBlock(Block *block);
    void addBlockAfter(Block *block, Block *newBlock);
    void setInsertPoint(Block *block);
    Block *getInsertPoint();
    
    //
    // Operand builders
    //
    std::shared_ptr<Operand> createI8(int8_t val);
    std::shared_ptr<Operand> createI16(int16_t val);
    std::shared_ptr<Operand> createI32(int val);
    std::shared_ptr<Operand> createI64(int64_t val);
    std::shared_ptr<Operand> createString(std::string val);
    
    //
    // Instruction builders
    //
    std::shared_ptr<Reg> createAlloca(Type *type);
    std::shared_ptr<Instruction> createStore(Type *type, std::shared_ptr<Operand> op, std::shared_ptr<Operand> dest);
    std::shared_ptr<Instruction> createStructStore(Type *type, std::shared_ptr<Operand> ptr, int index, std::shared_ptr<Operand> val);
    std::shared_ptr<Reg> createLoad(Type *type, std::shared_ptr<Operand> src);
    std::shared_ptr<Reg> createStructLoad(Type *type, std::shared_ptr<Operand> src, int index);
    std::shared_ptr<Operand> createGEP(Type *type, std::shared_ptr<Operand> ptr, std::shared_ptr<Operand> index);
    std::shared_ptr<Operand> createAdd(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createSub(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createSMul(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createSDiv(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createAnd(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createOr(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createXor(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2);
    std::shared_ptr<Operand> createNeg(Type *type, std::shared_ptr<Operand> op1);
    std::shared_ptr<Operand> createBeq(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock);
    std::shared_ptr<Operand> createBne(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock);
    std::shared_ptr<Operand> createBgt(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock);
    std::shared_ptr<Operand> createBlt(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock);
    std::shared_ptr<Operand> createBge(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock);
    std::shared_ptr<Operand> createBle(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock);
    std::shared_ptr<Instruction> createBr(Block *block);
    std::shared_ptr<Instruction> createVoidCall(std::string name, std::vector<std::shared_ptr<Operand> > args);
    std::shared_ptr<Reg> createCall(Type *type, std::string name, std::vector<std::shared_ptr<Operand> > args);
    std::shared_ptr<Instruction> createRetVoid();
    std::shared_ptr<Instruction> createRet(Type *type, std::shared_ptr<Operand> op);
    
    void addInstruction(std::shared_ptr<Instruction> instr) {
        currentBlock->addInstruction(instr);
    }
protected:
    std::shared_ptr<Operand> createBinaryOp(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, InstrType iType, Block *destBlock = nullptr);
private:
    Module *mod;
    Function *currentFunc;
    Block *currentBlock;
    int regCounter = 0;
    int lblCounter = 0;
};

}
