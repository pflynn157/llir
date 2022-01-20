#include <llir.hpp>
#include <irbuilder.hpp>

namespace LLIR {

IRBuilder::IRBuilder(Module *mod) {
    this->mod = mod;
}

Block *IRBuilder::createBlock(std::string name) {
    currentBlock = new Block(name);
    currentFunc->addBlock(currentBlock);
    return currentBlock;
}

void IRBuilder::addBlock(Block *block) {
    currentFunc->addBlock(block);
}

void IRBuilder::addBlockAfter(Block *block, Block *newBlock) {
    currentFunc->addBlockAfter(block, newBlock);
}

void IRBuilder::setInsertPoint(Block *block) {
    currentBlock = block;
}

Block *IRBuilder::getInsertPoint() {
    return currentBlock;
}

std::shared_ptr<Operand> IRBuilder::createI8(int8_t val) {
    return std::make_shared<Imm>(val);
}

std::shared_ptr<Operand> IRBuilder::createI16(int16_t val) {
    return std::make_shared<Imm>(val);
}

std::shared_ptr<Operand> IRBuilder::createI32(int val) {
    return std::make_shared<Imm>(val);
}

std::shared_ptr<Operand> IRBuilder::createI64(int64_t val) {
    return std::make_shared<Imm>(val);
}

std::shared_ptr<Operand> IRBuilder::createString(std::string val) {
    std::string name = "STR" + std::to_string(lblCounter);
    ++lblCounter;
    
    std::string val2 = "";
    for (char c : val) {
        if (c == '\n') val2 += "\\n";
        else val2 += c;
    }
    
    std::shared_ptr<StringPtr> ptr = std::make_shared<StringPtr>(name, val2);
    mod->addStringPtr(ptr.get());
    
    return ptr;
}

std::shared_ptr<Reg> IRBuilder::createAlloca(Type *type) {
    Instruction *alloc = new Instruction(InstrType::Alloca);
    alloc->setDataType(type);
    
    auto dest = std::make_shared<Reg>(std::to_string(regCounter));
    ++regCounter;
    alloc->setDest(dest);
    
    currentBlock->addInstruction(alloc);
    return dest;
}

Instruction *IRBuilder::createStore(Type *type, std::shared_ptr<Operand> op, std::shared_ptr<Operand> dest) {
    Instruction *store = new Instruction(InstrType::Store);
    store->setDataType(type);
    store->setOperand1(op);
    store->setOperand2(dest);
    currentBlock->addInstruction(store);
    return store;
}

Instruction *IRBuilder::createStructStore(Type *type, std::shared_ptr<Operand> ptr, int index, std::shared_ptr<Operand> val) {
    Instruction *op = new Instruction(InstrType::StructStore);
    op->setDataType(type);
    op->setOperand1(ptr);
    op->setOperand2(std::make_shared<Imm>(index));
    op->setOperand3(val);
    
    currentBlock->addInstruction(op);
    return op;
}

std::shared_ptr<Reg> IRBuilder::createLoad(Type *type, std::shared_ptr<Operand> src) {
    Instruction *load = new Instruction(InstrType::Load);
    load->setDataType(type);
    load->setOperand1(src);
    
    auto dest = std::make_shared<Reg>(std::to_string(regCounter));
    ++regCounter;
    load->setDest(dest);
    
    currentBlock->addInstruction(load);
    return dest;
}

std::shared_ptr<Reg> IRBuilder::createStructLoad(Type *type, std::shared_ptr<Operand> src, int index) {
    Instruction *load = new Instruction(InstrType::StructLoad);
    load->setDataType(type);
    load->setOperand1(src);
    load->setOperand2(std::make_shared<Imm>(index));
    
    auto dest = std::make_shared<Reg>(std::to_string(regCounter));
    ++regCounter;
    load->setDest(dest);
    
    currentBlock->addInstruction(load);
    return dest;
}

std::shared_ptr<Operand> IRBuilder::createBinaryOp(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, InstrType iType, Block *destBlock) {
    if (op1->getType() == OpType::Imm && op2->getType() == OpType::Imm) {
        std::shared_ptr<Imm> imm1 = std::dynamic_pointer_cast<Imm>(op1);
        std::shared_ptr<Imm> imm2 = std::dynamic_pointer_cast<Imm>(op2);
        
        switch (iType) {
            case InstrType::Add: return std::make_shared<Imm>(imm1->getValue() + imm2->getValue());
            case InstrType::Sub: return std::make_shared<Imm>(imm1->getValue() - imm2->getValue());
            case InstrType::SMul: return std::make_shared<Imm>(imm1->getValue() * imm2->getValue());
            case InstrType::SDiv: return std::make_shared<Imm>(imm1->getValue() / imm2->getValue());
            case InstrType::And: return std::make_shared<Imm>(imm1->getValue() & imm2->getValue());
            case InstrType::Or: return std::make_shared<Imm>(imm1->getValue() | imm2->getValue());
            case InstrType::Xor: return std::make_shared<Imm>(imm1->getValue() ^ imm2->getValue());
            
            default: {}
        }
    }
    
    Instruction *op = new Instruction(iType);
    op->setDataType(type);
    op->setOperand1(op1);
    op->setOperand2(op2);
    
    if (destBlock != nullptr) op->setOperand3(std::make_shared<Label>(destBlock->getName()));
    
    std::shared_ptr<Reg> dest = std::make_shared<Reg>(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

std::shared_ptr<Operand> IRBuilder::createGEP(Type *type, std::shared_ptr<Operand> ptr, std::shared_ptr<Operand> index) {
    return createBinaryOp(type, ptr, index, InstrType::GEP);
}

std::shared_ptr<Operand> IRBuilder::createAdd(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::Add);
}

std::shared_ptr<Operand> IRBuilder::createSub(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::Sub);
}

std::shared_ptr<Operand> IRBuilder::createSMul(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::SMul);
}

std::shared_ptr<Operand> IRBuilder::createSDiv(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::SDiv);
}

std::shared_ptr<Operand> IRBuilder::createAnd(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::And);
}

std::shared_ptr<Operand> IRBuilder::createOr(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::Or);
}

std::shared_ptr<Operand> IRBuilder::createXor(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2) {
    return createBinaryOp(type, op1, op2, InstrType::Xor);
}

std::shared_ptr<Operand> IRBuilder::createNeg(Type *type, std::shared_ptr<Operand> op1) {
    if (op1->getType() == OpType::Imm) {
        std::shared_ptr<Imm> imm = std::dynamic_pointer_cast<Imm>(op1);
        int val = imm->getValue() * -1;
        imm->setValue(val);
        return imm;    
    }
    
    Instruction *op = new Instruction(InstrType::Not);
    op->setDataType(type);
    op->setOperand1(op1);
    
    std::shared_ptr<Reg> dest = std::make_shared<Reg>(std::to_string(regCounter));
    ++regCounter;
    op->setDest(dest);
    
    currentBlock->addInstruction(op);
    return dest;
}

std::shared_ptr<Operand> IRBuilder::createBeq(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock) {
    if (op1->getType() == OpType::Imm && op2->getType() == OpType::Imm) {
        std::shared_ptr<Imm> imm1 = std::dynamic_pointer_cast<Imm>(op1);
        std::shared_ptr<Imm> imm2 = std::dynamic_pointer_cast<Imm>(op2);
        if (imm1->getValue() == imm2->getValue()) {
            auto lbl = std::make_shared<Label>(destBlock->getName());
            Instruction *op = new Instruction(InstrType::Br);
            op->setOperand1(lbl);
            
            std::shared_ptr<Reg> dest = std::make_shared<Reg>(std::to_string(regCounter));
            ++regCounter;
            op->setDest(dest);
            
            currentBlock->addInstruction(op);
            return dest;
        }
    }
    return createBinaryOp(type, op1, op2, InstrType::Beq, destBlock);
}

std::shared_ptr<Operand> IRBuilder::createBne(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock) {
    return createBinaryOp(type, op1, op2, InstrType::Bne, destBlock);
}

std::shared_ptr<Operand> IRBuilder::createBgt(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock) {
    return createBinaryOp(type, op1, op2, InstrType::Bgt, destBlock);
}

std::shared_ptr<Operand> IRBuilder::createBlt(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock) {
    return createBinaryOp(type, op1, op2, InstrType::Blt, destBlock);
}

std::shared_ptr<Operand> IRBuilder::createBge(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock) {
    return createBinaryOp(type, op1, op2, InstrType::Bge, destBlock);
}

std::shared_ptr<Operand> IRBuilder::createBle(Type *type, std::shared_ptr<Operand> op1, std::shared_ptr<Operand> op2, Block *destBlock) {
    return createBinaryOp(type, op1, op2, InstrType::Ble, destBlock);
}

Instruction *IRBuilder::createBr(Block *block) {
    auto lbl = std::make_shared<Label>(block->getName());
    Instruction *op = new Instruction(InstrType::Br);
    op->setOperand1(lbl);
    currentBlock->addInstruction(op);
    return op;
}

Instruction *IRBuilder::createVoidCall(std::string name, std::vector<std::shared_ptr<Operand> > args) {
    FunctionCall *fc = new FunctionCall(name, args);
    currentBlock->addInstruction(fc);
    return fc;
}

std::shared_ptr<Reg> IRBuilder::createCall(Type *type, std::string name, std::vector<std::shared_ptr<Operand> > args) {
    FunctionCall *fc = new FunctionCall(name, args);
    fc->setDataType(type);
    
    auto dest = std::make_shared<Reg>(std::to_string(regCounter));
    ++regCounter;
    fc->setDest(dest);
    
    currentBlock->addInstruction(fc);
    return dest;
}

Instruction *IRBuilder::createRetVoid() {
    Instruction *ret = new Instruction(InstrType::Ret);
    ret->setDataType(Type::createVoidType());
    currentBlock->addInstruction(ret);
    return ret;
}

Instruction *IRBuilder::createRet(Type *type, std::shared_ptr<Operand> op) {
    Instruction *ret = new Instruction(InstrType::Ret);
    ret->setDataType(type);
    ret->setOperand1(op);
    currentBlock->addInstruction(ret);
    return ret;
}

} // end namespace LLIR
