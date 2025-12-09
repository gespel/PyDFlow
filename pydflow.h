#ifndef PYDFLOW_H
#define PYDFLOW_H
#include <string>

class PyDFlowWrapper {
public:
    PyDFlowWrapper(std::string name) : name(name) {

    }
    std::string name;
};

int add_numbers(PyDFlowWrapper *wrapper, int a, int b);
void create_testing_pipe(PyDFlowWrapper *wrapper);


#endif