#include <iostream>

#include "opt.h"

void func(void *p) {
    std::cout << "func(" << (char *)p << ")" << std::endl;
    std::exit(0);
}

int main(int argc, char** argv){

    int opt_int = 0;
    bool opt_bool = false;
    std::string opt_string;

    Opt parmsParser;

    parmsParser
        .Banner("Banner: example")
        .Int("i", "int", "int type option.", opt_int, 0)
        .Bool("b", "bool", "bool type option.", opt_bool, false)
        .String("s", "string", "string type option.", opt_string, "")
        .Handle("f", "func", "func type option.", func, (void*)"closure")
        .Helper()
        .Version("cxx_opt V0.0.1");

    parmsParser.Parse(argc, argv);

    std::cout << "opt_int=" << opt_int
            << "\nopt_bool=" << opt_bool
            << "\nopt_string=" << opt_string << std::endl;
}
