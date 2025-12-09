#ifndef PYDFLOW_H
#define PYDFLOW_H

struct PyDFLowWrapper {};

int add_numbers(struct PyDFLowWrapper *wrapper, int a, int b);
void create_testing_pipe(struct PyDFLowWrapper *wrapper);


#endif