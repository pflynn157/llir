#include <iostream>
#include <vector>
#include <memory>

#include <parser.hpp>

Parser::Parser(std::string input, std::string name) {
    scanner = new Scanner(input);
    mod = new Module(name);
    builder = new IRBuilder(mod);
}

// Clean up the mess that we undoubtly made
Parser::~Parser() {
    delete scanner;
    delete mod;
    delete builder;
}

void Parser::parse() {
    Token token = scanner->getNext();
    
    while (token.type != Eof) {
        switch (token.type) {
            case Global:
            case Local:
            case Extern: {
                // TODO: Handle errors
                buildFunction(token);
            } break;
            
            default: {
                std::cerr << "Error: Invalid token in function scope." << std::endl;
            }
        }
        
        token = scanner->getNext();
    }
}

Type *Parser::getType(Token token) {
    switch (token.type) {
        case Void: return Type::createVoidType();
        case I8: return Type::createI8Type();
        case I16: return Type::createI16Type();
        case I32: return Type::createI32Type();
        case I64: return Type::createI64Type();
        
        default: {}
    }
    
    return nullptr;
}

//
// Builds a function
//
bool Parser::buildFunction(Token linkToken) {
    // First, build the type
    Token token = scanner->getNext();
    int ptrLevel = 0;
    while (token.type != Eof && token.type == Pointer) {
        ++ptrLevel;
        token = scanner->getNext();
    }
    Type *type = getType(token);
    if (type == nullptr) {
        std::cerr << "Error: Invalid type for function." << std::endl;
        return false;
    }
    
    for (int i = 0; i<ptrLevel; i++) {
        type = new PointerType(type);
    }
    
    // The next token should be an ID
    // --> The function name
    token = scanner->getNext();
    std::string name = token.id_val;
    if (token.type != Id) {
        std::cerr << "Error: Expected function name." << std::endl;
        return false;
    }
    
    // Now, parse arguments
    token = scanner->getNext();
    if (token.type != LParen) {
        std::cerr << "Error: Expected \'(\'." << std::endl;
        return false;
    }
    
    // The syntax for arguments is: %<reg>:<type>
    std::vector<Reg *> argRegs;
    std::vector<Type *> argTypes;
    
    token = scanner->getNext();
    while (token.type != RParen && token.type != Eof) {
        Token regToken = scanner->getNext();
        Token colon = scanner->getNext();
        if (token.type != Mod || colon.type != Colon) {
            std::cerr << "Error: Invalid register syntax." << std::endl;
            return false;
        }
        
        Reg *reg;
        switch (regToken.type) {
            case Id: reg = new Reg(regToken.id_val); break;
            case Int32: reg = new Reg(std::to_string(regToken.i32_val)); break;
            
            default: {
                std::cerr << "Error: Invalid register syntax in function argument." << std::endl;
                return false;
            }
        }
        
        // Now, the data type
        int ptrCount = 0;
        token = scanner->getNext();
        while (token.type != Eof && token.type == Pointer) {
            ++ptrCount;
            token = scanner->getNext();
        }
        
        Type *type = getType(token);
        if (type == nullptr) {
            std::cerr << "Error: Invalid type for function." << std::endl;
            return false;
        }
        
        for (int i = 0; i<ptrLevel; i++) {
            type = new PointerType(type);
        }
        
        // Add to the function
        argRegs.push_back(reg);
        argTypes.push_back(type);
        
        // Move on, but make sure we have proper syntax
        token = scanner->getNext();
        if (token.type == Comma) {
            token = scanner->getNext();
        }
    }
    
    // Finally, determine if we have a function body
    token = scanner->getNext();
    if (token.type != LCBrace && token.type != SemiColon) {
        std::cerr << "Error: Invalid token at end of function signature." << std::endl;
        return false;
    }
    
    // Now, we can create the function
    Linkage link;
    switch (linkToken.type) {
        case Global: link = Linkage::Global; break;
        case Local: link = Linkage::Local; break;
        case Extern: link = Linkage::Extern; break;
        
        default: {}
    }
    
    Function *func = Function::Create(name, link, type);
    mod->addFunction(func);
    builder->setCurrentFunction(func);
    
    for (int i = 0; i<argTypes.size(); i++) {
        if (argTypes.at(i) == nullptr || argRegs.at(i) == nullptr) {
            std::cerr << "Error: Invalid function arguments." << std::endl;
            return false;
        }
        func->addArgPair(argTypes.at(i), argRegs.at(i));
    }
    
    if (token.type == SemiColon) {
        // TODO
    } else {
        buildBody();
    }
    
    return true;
}

//
// Builds a function body
//
bool Parser::buildBody() {
    Token token = scanner->getNext();
    
    while (token.type != Eof && token.type != RCBrace) {
        switch (token.type) {
            // Basic blocks
            case Id: {
                std::string name = token.id_val;
                
                token = scanner->getNext();
                if (token.type != Colon) {
                    std::cerr << "Error: Expected \':\' at end of block declaration." << std::endl;
                    return false;
                }
                
                builder->createBlock(name);
            } break;
            
            // Destination operands
            case Mod: buildDestInstruction(); break;
            
            // Otherwise, all other instructions
            default: buildInstruction(token);
        }
    
        token = scanner->getNext();
    }
    
    return true;
}

//
// Builds a destination operand (an instruction with an assignment)
//
bool Parser::buildDestInstruction() {
    // First, build register name
    Token token = scanner->getNext();
    std::string name = "";
    
    switch (token.type) {
        case Id: name = token.id_val; break;
        case Int32: name = std::to_string(token.i32_val); break;
        
        default: {
            std::cerr << "Error: Invalid token in register naming." << std::endl;
            return false;
        }
    }
    
    // Make sure the next token is an assignment
    token = scanner->getNext();
    if (token.type != Assign) {
        std::cerr << "Error: Expected \'=\' in assignment." << std::endl;
        return false;
    }
    
    // Now, we can build the rest of the instruction
    auto reg = std::make_shared<Reg>(name);
    token = scanner->getNext();
    if (!buildInstruction(token, reg)) {
        return false;
    }
    
    return true;
}

//
// Builds an instruction
//
bool Parser::buildInstruction(Token instrType, std::shared_ptr<Operand> dest) {
    // The type
    Token token = scanner->getNext();
    int ptrLevel = 0;
    while (token.type != Eof && token.type == Pointer) {
        ++ptrLevel;
        token = scanner->getNext();
    }
    Type *type = getType(token);
    if (type == nullptr) {
        std::cerr << "Error: Invalid type for function." << std::endl;
        return false;
    }
    
    for (int i = 0; i<ptrLevel; i++) {
        type = new PointerType(type);
    }
    
    // Operands
    std::vector<std::shared_ptr<Operand>> operands;
    std::string funcName = "";
    bool inFunc = false;
    
    token = scanner->getNext();
    while (token.type != Eof && token.type != SemiColon) {
        switch (token.type) {
            case Int32: operands.push_back(std::make_shared<Imm>(token.i32_val)); break;
            
            case Mod: {
                token = scanner->getNext();
                std::shared_ptr<Reg> reg;
                if (token.type == Id) {
                    reg = std::make_shared<Reg>(token.id_val);
                } else if (token.type == Int32) {
                    reg = std::make_shared<Reg>(std::to_string(token.i32_val));
                } else {
                    std::cerr << "Error: Invalid register." << std::endl;
                    return false;
                }
                
                operands.push_back(reg);
            } break;
            
            case Id: {
                std::string name = token.id_val;
                token = scanner->getNext();
                if (token.type == LParen) {
                    funcName = name;
                    inFunc = true;
                } else {
                    scanner->rewind(token);
                    auto lbl = std::make_shared<Label>(name);
                    operands.push_back(lbl);
                }
            } break;
            
            case StrSym: {
                Token nameToken = scanner->getNext();
                Token lp = scanner->getNext();
                Token valToken = scanner->getNext();
                Token rp = scanner->getNext();
                
                // Do all the syntax checks
                if (nameToken.type != Id) {
                    std::cerr << "Error: Expected string constant name." << std::endl;
                    return false;
                }
                if (lp.type != LParen || rp.type != RParen) {
                    std::cerr << "Error: String values should be surrounded with \'(\' and \')\'." << std::endl;
                    return false;
                }
                if (valToken.type != String) {
                    std::cerr << "Error: Expected string literal." << std::endl;
                    return false;
                }
                
                // If all passes, we can build
                StringPtr *ptr = new StringPtr(nameToken.id_val, valToken.id_val);
                mod->addStringPtr(ptr);
                
                std::shared_ptr<StringPtr> ptr2 = std::make_shared<StringPtr>(nameToken.id_val, valToken.id_val);
                operands.push_back(ptr2);
            } break;
            
            case RParen: {
                if (inFunc) {
                    inFunc = false;
                } else {
                    std::cerr << "Error: Invalid argument token." << std::endl;
                    return false;
                }
            } break;
            
            default: {
                std::cerr << "Error: Invalid argument token." << std::endl;
                return false;
            }
        }
        
        token = scanner->getNext();
        if (token.type == Comma) {
            token = scanner->getNext();
        }
    }
    
    // Now, build the instruction
    std::shared_ptr<Instruction> instr;
    switch (instrType.type) {
        case Ret: instr = std::make_shared<Instruction>(InstrType::Ret); break;
        case Alloca: instr = std::make_shared<Instruction>(InstrType::Alloca); break;
        case Load: instr = std::make_shared<Instruction>(InstrType::Load); break;
        case Store: instr = std::make_shared<Instruction>(InstrType::Store); break;
        case LoadStruct: instr = std::make_shared<Instruction>(InstrType::StructLoad); break;
        case StoreStruct: instr = std::make_shared<Instruction>(InstrType::StructStore); break;
        case GetElementPtr: instr = std::make_shared<Instruction>(InstrType::GEP); break;
        
        case Add: instr = std::make_shared<Instruction>(InstrType::Add); break;
        case Sub: instr = std::make_shared<Instruction>(InstrType::Sub); break;
        case SMul: instr = std::make_shared<Instruction>(InstrType::SMul); break;
        case SDiv: instr = std::make_shared<Instruction>(InstrType::SDiv); break;
        case Call: instr = std::make_shared<FunctionCall>(funcName, operands); break;
        
        case Br: instr = std::make_shared<Instruction>(InstrType::Br); break;
        case Beq: instr = std::make_shared<Instruction>(InstrType::Beq); break;
        case Bne: instr = std::make_shared<Instruction>(InstrType::Bne); break;
        case Bgt: instr = std::make_shared<Instruction>(InstrType::Bgt); break;
        case Blt: instr = std::make_shared<Instruction>(InstrType::Blt); break;
        case Bge: instr = std::make_shared<Instruction>(InstrType::Bge); break;
        case Ble: instr = std::make_shared<Instruction>(InstrType::Ble); break;
        
        case And: instr = std::make_shared<Instruction>(InstrType::And); break;
        case Or: instr = std::make_shared<Instruction>(InstrType::Or); break;
        case Xor: instr = std::make_shared<Instruction>(InstrType::Xor); break;
        case Not: instr = std::make_shared<Instruction>(InstrType::Not); break;
        
        default: {
            std::cerr << "Error: Unknown instruction." << std::endl;
            return false;
        }
    }
    
    instr->setDataType(type);
    if (dest) instr->setDest(dest);
    
    if (instrType.type != Call) {
        if (operands.size() >= 1) instr->setOperand1(operands.at(0));
        if (operands.size() >= 2) instr->setOperand2(operands.at(1));
        if (operands.size() >= 3) instr->setOperand3(operands.at(2));
    }
    
    builder->addInstruction(instr);
    
    return true;
}

void Parser::print() {
    if (mod) mod->print();
}

