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
        
        // Instructions
        
        // Clean up the stack and leave
        assembly += "  leave\n";
        assembly += "  ret\n";
    }
}

void Amd64Writer::dump() {
    std::cout << assembly << std::endl;
}

void Amd64Writer::writeToFile() {

}

} // end namespace LLIR