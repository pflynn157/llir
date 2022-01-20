#include <iostream>
#include <cstdlib>
#include <sys/stat.h>
#include <sys/types.h>
#include <memory>

#include <llir.hpp>
#include <irbuilder.hpp>
#include <amd64/amd64.hpp>
using namespace LLIR;

int main(int argc, char **argv) {
    Module *mod = new Module("test3");
    IRBuilder *builder = new IRBuilder(mod);
    
    //
    // func main:
    //     ret 0
    Type *i32Type = Type::createI32Type();
    Function *mainFunc = Function::Create("main", Linkage::Global, i32Type);
    mod->addFunction(mainFunc);
    builder->setCurrentFunction(mainFunc);
    builder->createBlock("entry");
    
    std::shared_ptr<Operand> i32 = builder->createI32(0);
    builder->createRet(i32Type, i32);
    
    mod->print();
    mod->transform();
    
    // Generate a binary
    mkdir("./test_bin", 0700);
    
    LLIR::Amd64Writer *writer = new LLIR::Amd64Writer(mod);
    writer->compile();
    writer->writeToFile("/tmp/test3.s");
    system("gcc /tmp/test3.s -o ./test_bin/test3");
    
    return 0;
}
