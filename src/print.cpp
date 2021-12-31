#include <iostream>

#include <llir.hpp>

namespace LLIR {

void Module::print() {
    std::cout << ";module " << name << std::endl;
    std::cout << std::endl;
    for (Function *func : functions) func->print();
}

void Function::print() {
    if (blocks.size() == 0) {
        std::cout << "def ";
    } else {
        std::cout << "func ";
    }
    switch (linkage) {
        case Linkage::Local: std::cout << "(lcl) "; break;
        case Linkage::Global: std::cout << "(gbl) "; break;
        case Linkage::Extern: std::cout << "(ext) "; break;
    }
    std::cout << name;
    
    // TODO: args
    std::cout << "()";
    
    if (blocks.size() == 0) {
        std::cout << ";";
    } else {
        std::cout << " {" << std::endl;
        for (Block *block : blocks) block->print();
        std::cout << "}";
    }
    std::cout << std::endl;
}

void Block::print() {
    std::cout << name << ":" << std::endl;
    for (Instruction *i : instrs) {
        std::cout << "  ";
        i->print();
    }
}

void Instruction::print() {
    switch (type) {
        case InstrType::None: std::cout << "?? "; break;
        
        case InstrType::Ret: std::cout << "ret "; break;
        case InstrType::RetVoid: std::cout << "retvoid "; break;
        
        case InstrType::Add: std::cout << "add "; break;
        case InstrType::Sub: std::cout << "sub "; break;
        case InstrType::UMul: std::cout << "umul "; break;
        case InstrType::SMul: std::cout << "smul "; break;
        case InstrType::UDiv: std::cout << "udiv "; break;
        case InstrType::SDiv: std::cout << "sdiv "; break;
        case InstrType::URem: std::cout << "urem "; break;
        case InstrType::SRem: std::cout << "srem "; break;
        
        case InstrType::Br: std::cout << "br "; break;
        case InstrType::Beq: std::cout << "beq "; break;
        case InstrType::Bne: std::cout << "bne "; break;
        case InstrType::Bgt: std::cout << "bgt "; break;
        case InstrType::Bge: std::cout << "bge "; break;
        case InstrType::Blt: std::cout << "blt "; break;
        case InstrType::Ble: std::cout << "ble "; break;
        
        case InstrType::Load: std::cout << "load "; break;
        case InstrType::Store: std::cout << "store "; break;
    }
    
    if (dest) dest->print();
    if (src1) {
        if (dest) std::cout << ", ";
        src1->print();
    }
    if (src2) {
        std::cout << ", ";
        src2->print();
    }
    if (src3) {
        std::cout << ", ";
        src3->print();
    }
    std::cout << ";" << std::endl;
}

void Imm::print() {
    std::cout << imm;
}

void Reg::print() {
    std::cout << "%" << name;
}

void Label::print() {
    std::cout << name;
}

} // end namespace LLIR