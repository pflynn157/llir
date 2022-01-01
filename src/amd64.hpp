#pragma once

#include <string>

#include <llir.hpp>

namespace LLIR {

class Amd64Writer {
public:
    explicit Amd64Writer(Module *mod);
    void compile();
    void dump();
    void writeToFile();
private:
    Module *mod = nullptr;
    std::string assembly = "";
};

} // end namespace LLIR