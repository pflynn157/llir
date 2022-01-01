#include <iostream>

#include <amd64.hpp>
#include <llir.hpp>

namespace LLIR {

Amd64Writer::Amd64Writer(Module *mod) {
    this->mod = mod;
    
    // GAS specific
    assembly += ".intel_syntax noprefix\n";
}

void Amd64Writer::compile() {
    // Data section
    assembly += ".data\n";
    
    // Text section
    assembly += ".text\n";
    for (int i = 0; i<mod->getFunctionCount(); i++) {
        assembly += "\n";
        
        Function *func = mod->getFunction(i);
        switch (func->getLinkage()) {
            case Linkage::Global: {
                assembly += ".globl " + func->getName() + "\n";
                assembly += ".type " + func->getName() + ",@function\n";
            } break;
            
            case Linkage::Local: {} break;
            
            case Linkage::Extern: {
                assembly += ".extern " + func->getName() + "\n";
                continue;
            } break;
        }
        
        assembly += func->getName() + ":\n";
        assembly += "  push rbp\n";
        assembly += "  mov rbp, rsp\n";
        assembly += std::string("  sub rsp, ") + std::to_string(func->getStackSize()) + "\n";
        
        // Blocks
        for (int j = 0; j<func->getBlockCount(); j++) {
            Block *block = func->getBlock(j);
            assembly += block->getName() + ":\n";
            
            // Instructions
            for (int k = 0; k<block->getInstrCount(); k++) {
                compileInstruction(block->getInstruction(k));
            }
        }
        
        // Clean up the stack and leave
        assembly += "  leave\n";
        assembly += "  ret\n";
    }
}

void Amd64Writer::compileInstruction(Instruction *instr) {
    switch (instr->getType()) {
        case InstrType::None: assembly += "nop\n"; break;
        
        case InstrType::Ret: {
            Operand *src = instr->getOperand1();
            switch (src->getType()) {
                case OpType::Imm: break;
                
                case OpType::Reg: {
                    Reg *reg = static_cast<Reg *>(src);
                    if (regMap[reg->getName()] == "eax" || regMap[reg->getName()] == "rax") break;
                    // TODO: Either load memory or move to the proper register
                    
                } break;
            }
        } break;
        
        case InstrType::RetVoid: break;    // Nothing on x86-64
        
        case InstrType::Add: break;
        case InstrType::Sub: break;
        case InstrType::UMul: break;
        case InstrType::SMul: break;
        case InstrType::UDiv: break;
        case InstrType::SDiv: break;
        case InstrType::URem: break;
        case InstrType::SRem: break;
        
        case InstrType::Br: break;
        case InstrType::Beq: break;
        case InstrType::Bne: break;
        case InstrType::Bgt: break;
        case InstrType::Bge: break;
        case InstrType::Blt: break;
        case InstrType::Ble: break;
        
        case InstrType::Alloca: {
            switch (instr->getDataType()->getType()) {
                case DataType::Void: break;
                case DataType::I8: stackPos += 1; break;
                case DataType::I16: stackPos += 2; break;
                case DataType::F32:
                case DataType::I32: stackPos += 4; break;
                case DataType::I64:
                case DataType::F64:
                case DataType::Ptr: stackPos += 8; break;
            }
            
            Reg *reg = static_cast<Reg *>(instr->getDest());
            memMap[reg->getName()] = stackPos;
        } break;
        
        case InstrType::Load: {
            Reg *destReg = static_cast<Reg *>(instr->getDest());
            regMap[destReg->getName()] = "eax";
            
            Reg *srcReg = static_cast<Reg *>(instr->getOperand1());
            int pos = memMap[srcReg->getName()];
            
            assembly += "  mov eax, ";
            assembly += getSizeForType(instr->getDataType());
            assembly += " [rbp-" + std::to_string(pos) + "]\n";
        } break;
        
        case InstrType::Store: {
            Reg *dest = static_cast<Reg *>(instr->getOperand2());
            int pos = memMap[dest->getName()];
            
            Operand *src = instr->getOperand1();
            switch (src->getType()) {
                case OpType::Imm: {
                    Imm *imm = static_cast<Imm *>(src);
                    
                    assembly += "  mov " + getSizeForType(instr->getDataType());
                    assembly += " [rbp-" + std::to_string(pos) + "]";
                    assembly += ", " + std::to_string(imm->getValue()) + "\n";
                } break;
                
                case OpType::Reg: break;
            }
        } break;
    }
}

void Amd64Writer::dump() {
    std::cout << assembly << std::endl;
}

void Amd64Writer::writeToFile() {

}

std::string Amd64Writer::getSizeForType(Type *type) {
    switch (type->getType()) {
        case DataType::Void: break;
        case DataType::I8: return "BYTE PTR";
        case DataType::I16: return "WORD PTR";
        case DataType::F32:
        case DataType::I32: return "DWORD PTR";
        case DataType::I64:
        case DataType::F64:
        case DataType::Ptr: return "QWORD PTR";
    }
    return "";
}

} // end namespace LLIR