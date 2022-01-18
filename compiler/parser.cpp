#include <iostream>

#include <parser.hpp>

Parser::Parser(std::string input, std::string name) {
    scanner = new Scanner(input);
    mod = new Module(name);
    builder = new IRBuilder(mod);
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
    Token token = scanner->getNext();
    Type *type = getType(token);
    if (type == nullptr) {
        std::cerr << "Error: Invalid token." << std::endl;
        return false;
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
    while (token.type != RParen && token.type != Eof) {
        token = scanner->getNext();
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
    
    if (token.type == SemiColon) {
        // TODO
    } else {
        buildBody();
    }
    
    return true;
}

// Builds a function body
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
            
            // Instructions
            case Ret: std::cout << "RET" << std::endl; break;
            
            default: {
                std::cerr << "Error: Invalid token in function body." << std::endl;
                return false;
            }
        }
    
        token = scanner->getNext();
    }
    
    return true;
}

void Parser::print() {
    if (mod) mod->print();
}
