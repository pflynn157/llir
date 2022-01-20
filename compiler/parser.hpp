#pragma once

#include <memory>

#include <Lex.hpp>
#include <llir.hpp>
#include <irbuilder.hpp>

using namespace LLIR;

class Parser {
public:
    explicit Parser(std::string input, std::string name);
    ~Parser();
    
    void parse();
    void print();
    
    Module *getModule() {
        return mod;
    }
protected:
    Type *getType(Token token);
    bool buildFunction(Token linkToken);
    bool buildBody();
    bool buildDestInstruction();
    bool buildInstruction(Token instrType, std::shared_ptr<Operand> dest = nullptr);
private:
    Scanner *scanner;
    Module *mod;
    IRBuilder *builder;
};
