#include <iostream>

#include <parser.hpp>

int main(int argc, char **argv) {
    if (argc == 1) {
        std::cout << "Error: No input file specified." << std::endl;
        return 0;
    }
    
    std::string input = argv[1];
    
    Parser *parser = new Parser(input, "mod1");
    parser->parse();
    parser->print();
    
    return 0;
}
