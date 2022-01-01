#include <iostream>

#include <amd64.hpp>
#include <llir.hpp>

namespace LLIR {

Amd64Writer::Amd64Writer(Module *mod) {

}

void Amd64Writer::compile() {

}

void Amd64Writer::dump() {
    std::cout << assembly << std::endl;
}

void Amd64Writer::writeToFile() {

}

} // end namespace LLIR