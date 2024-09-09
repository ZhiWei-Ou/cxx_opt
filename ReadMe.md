# A simple cxx opt
Only a header, easy to integrate and use.
Note: **use C++11**

# Integration
Copy [opt.h](opt.h) to your project.

# Example
- [example.cpp](example.cpp)
- build:
```bash
$ cmake -S. -Bbuild
```
- code:
```c
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
        .Int("i", "int", "int type option", opt_int, 0)
        .Bool("b", "bool", "bool type option", opt_bool, false)
        .String("s", "string", "string type option", opt_string, "")
        .Handle("f", "func", "func type option", func, (void*)"closure");
    /*parmsParser.Int("i", "int", "int type option", opt_int);*/
    /*parmsParser.Bool("b", "bool", "bool type option", opt_bool);*/
    /*parmsParser.String("s", "string", "string type option", opt_string);*/

    parmsParser.PrintAllOpt(std::cout);

    parmsParser.Parse(argc, argv);

    std::cout << "opt_int=" << opt_int
            << "\nopt_bool=" << opt_bool
            << "\nopt_string=" << opt_string << std::endl;
}
```
- output:
```bash
$ ./example --int=100 --string="Hello World" -b=TRUE
opt_int=100
opt_bool=1
opt_string=Hello World
$ ./example -h # ./example.cpp --help
Banner: example
        -i, --int, desc:int type option.
        -b, --bool, desc:bool type option.
        -s, --string, desc:string type option.
        -f, --func, desc:func type option.
        -h, --help, desc:show help.
        -v, --version, desc:show version.
$ ./example -v # ./example.cpp --version
cxx_opt V0.0.1
```
