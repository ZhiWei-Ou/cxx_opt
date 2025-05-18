# CXX Options
    A CXX options parsing tool similar to Golang's flag package.

# Dependencies
    Uses C++11 features and STL.

# Integration
    Copy `cxx_opt.h` and `cxx_opt.cpp` to project.

# Usage
```c++
#include "cxx_opt.h"
using namespace CXX_OPT_NAMESPACE;

int main(int argc, char **argv) {
    Flag flag;

    bool debug = false;
    int port = 80;
    std::string config_file = "/path/to/default/config";

    flag.registerBool("debug", &debug, "debug mode");
    flag.registerInt("port", &port, "server port");
    flag.registerString("config", &config_file, "config file path");

    flag.parse(argc, argv);

    if (debug) {
        // enable debug mode
    }

    // load config_file

    // start server on port

    return 0;
}
```
```bash
g++ -std=c++11 main.cpp cxx_opt.cpp -I. -o flag
$ ./flag -debug -port 443 -config config.yml
debug: 1
port: 443
config: config.yaml

```
