#pragma once

#include <string>
#include <map>

#include <llir.hpp>

namespace LLIR {

class Amd64Writer {
public:
    explicit Amd64Writer(Module *mod);
    void compile();
    void compileInstruction(Instruction *instr);
    void dump();
    void writeToFile();
protected:
    std::string getSizeForType(Type *type);
private:
    Module *mod = nullptr;
    std::string assembly = "";
    
    int stackPos = 0;
    std::map<std::string, int> memMap;
    std::map<std::string, std::string> regMap;
};

} // end namespace LLIR