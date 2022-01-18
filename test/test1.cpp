#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>

#include <llir.hpp>
#include <irbuilder.hpp>
#include <amd64.hpp>
using namespace LLIR;

int main(int argc, char **argv) {
    Module *mod = new Module("test1");
    IRBuilder *builder = new IRBuilder(mod);
    
    //
    // func main:
    //     ret 5
    Type *i32Type = Type::createI32Type();
    Function *mainFunc = Function::Create("main", Linkage::Global, i32Type);
    mod->addFunction(mainFunc);
    builder->setCurrentFunction(mainFunc);
    builder->createBlock("entry");
    
    Operand *i32 = builder->createI32(5);
    builder->createRet(i32Type, i32);
    
    mod->print();
    
    // Generate a binary
    mkdir("./test_bin", 0700);
    
    LLIR::Amd64Writer *writer = new LLIR::Amd64Writer(mod);
    writer->compile();
    writer->writeToFile("./test_bin/test1.s");
    system("gcc ./test_bin/test1.s -o ./test_bin/test1");
    
    return 0;
}
