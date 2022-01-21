#pragma once

#include <string>
#include <map>
#include <memory>

#include <llir.hpp>
#include <amd64/x86ir.hpp>

namespace LLIR {

class Amd64Writer {
public:
    explicit Amd64Writer(Module *mod);
    ~Amd64Writer();
    void compile();
    void compileInstruction(std::shared_ptr<Instruction> instr, std::string prefix);
    std::shared_ptr<X86Operand> compileOperand(std::shared_ptr<Operand> src, Type *type, std::string prefix);
    void dump();
    void writeToFile();
    void writeToFile(std::string path);
    void build();
protected:
    std::string getSizeForType(Type *type);
    int getIntSizeForType(Type *type);
private:
    Module *mod = nullptr;
    X86File *file;
    
    int stackPos = 0;
    std::map<std::string, int> memMap;
    std::map<int, X86Reg> regMap;
    std::map<int, X86Reg> argRegMap;
};

} // end namespace LLIR
