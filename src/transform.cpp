#include <map>
#include <vector>
#include <string>
#include <algorithm>

#include <llir.hpp>

namespace LLIR {

// The maps
std::vector<std::string> memList;
std::map<std::string, int> regMap;
std::map<std::string, int> argMap;
std::map<std::string, int> ptrMap;
int regCount = 0;
int argCount = 0;
int ptrCount = 0;

std::shared_ptr<Operand> checkOperand(std::shared_ptr<Operand> input) {
    if (input->getType() != OpType::Reg) {
        return input;
    }
    
    std::shared_ptr<Reg> reg = std::dynamic_pointer_cast<Reg>(input);
    if (std::find(memList.begin(), memList.end(), reg->getName()) != memList.end()) {
        std::shared_ptr<Mem> mem = std::make_shared<Mem>(reg->getName());
        return mem;
    }
    
    if (argMap.find(reg->getName()) != argMap.end()) {
        std::shared_ptr<AReg> reg2 = std::make_shared<AReg>(argMap[reg->getName()]);
        return reg2;
    }
    
    if (ptrMap.find(reg->getName()) != ptrMap.end()) {
        std::shared_ptr<PReg> reg2 = std::make_shared<PReg>(ptrMap[reg->getName()]);
        return reg2;
    }
    
    if (regMap.find(reg->getName()) != regMap.end()) {
        std::shared_ptr<HReg> reg2 = std::make_shared<HReg>(regMap[reg->getName()]);
        return reg2;
    }
    
    return input;
}

// By default, all operands in LLIR are virtual registers, which are naturally not
// suitable to hardware transformation
//
// This pass translates all virtual registers to hardware registers and memory operands
// as appropriate.
//
// The algorithm is very simple. Basically, we look for "allocas", and assign memory operands to
// the destination, and replace everywhere else.
//
void Module::transform() {
    for (Function *func : functions) {
        memList.clear();
        regMap.clear();
        regCount = 0;
        argMap.clear();
        argCount = 0;
        ptrMap.clear();
        ptrCount = 0;
        
        for (int i = 0; i<func->getBlockCount(); i++) {
            Block *block = func->getBlock(i);
            
            // Assign argument registers
            for (int j = 0; j<func->getArgCount(); j++) {
                Reg *reg = func->getArg(j);
                argMap[reg->getName()] = argCount;
                ++argCount;
            }
            
            // Now, take care of the rest of the instructions
            for (int j = 0; j<block->getInstrCount(); j++) {
                std::shared_ptr<Instruction> instr = block->getInstruction(j);
                switch (instr->getType()) {
                    case InstrType::Alloca: {
                        std::shared_ptr<Reg> reg = std::dynamic_pointer_cast<Reg>(instr->getDest());
                        memList.push_back(reg->getName());
                        
                        std::shared_ptr<Mem> mem = std::make_shared<Mem>(reg->getName());
                        instr->setDest(mem);
                        continue;
                    }
                    
                    // In order to keep from too many registers being used, we should always
                    // go back to earlier registers once we hit a store instruction
                    case InstrType::StructStore:
                    case InstrType::Store: {
                        regCount = 0;
                    } break;
                    
                    case InstrType::GEP: {
                        std::shared_ptr<Reg> reg = std::dynamic_pointer_cast<Reg>(instr->getDest());
                        ptrMap[reg->getName()] = ptrCount;
                        ++ptrCount;
                        
                        std::shared_ptr<PReg> reg2 = std::make_shared<PReg>(ptrCount - 1);
                        instr->setDest(reg2);
                    } break;
                    
                    case InstrType::Load:
                    case InstrType::StructLoad:
                    case InstrType::Add:
                    case InstrType::Sub:
                    case InstrType::SMul:
                    case InstrType::UMul:
                    case InstrType::SDiv:
                    case InstrType::UDiv:
                    case InstrType::And:
                    case InstrType::Or:
                    case InstrType::Xor: {
                        std::shared_ptr<Reg> reg = std::dynamic_pointer_cast<Reg>(instr->getDest());
                        regMap[reg->getName()] = regCount;
                        ++regCount;
                        
                        std::shared_ptr<HReg> reg2 = std::make_shared<HReg>(regCount - 1);
                        instr->setDest(reg2);
                    } break;
                    
                    case InstrType::Call: {
                        regCount = 0;
                    
                        std::shared_ptr<FunctionCall> fc = std::dynamic_pointer_cast<FunctionCall>(instr);
                        std::vector<std::shared_ptr<Operand>> args;
                        for (std::shared_ptr<Operand> arg : fc->getArgs()) {
                            std::shared_ptr<Operand> op = checkOperand(arg);
                            args.push_back(op);
                        }
                        fc->setArgs(args);
                        
                        if (instr->getDest()) {
                            std::shared_ptr<Reg> reg = std::dynamic_pointer_cast<Reg>(instr->getDest());
                            regMap[reg->getName()] = regCount;
                            ++regCount;
                            
                            std::shared_ptr<HReg> reg2 = std::make_shared<HReg>(regCount - 1);
                            instr->setDest(reg2);
                        }
                    } break;
                }
                
                // Otherwise, check and switch the operands
                if (instr->getOperand1()) {
                    std::shared_ptr<Operand> op = checkOperand(instr->getOperand1());
                    instr->setOperand1(op);
                }
                
                if (instr->getOperand2()) {
                    std::shared_ptr<Operand> op = checkOperand(instr->getOperand2());
                    instr->setOperand2(op);
                }
                
                if (instr->getOperand3()) {
                    std::shared_ptr<Operand> op = checkOperand(instr->getOperand3());
                    instr->setOperand3(op);
                }
            }
        }
    }
}

} // end LLIR
