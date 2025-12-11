#ifndef PYDFLOW_H
#define PYDFLOW_H
#include <string>
extern "C" {
#include <stdlib.h>
#include <stdio.h>
#include <doca_flow.h>
#include <doca_dev.h>
#include <doca_log.h>
#include "flow_common.h" 
#include "doca_error.h"
#include "dpdk_utils.h"
#include "doca_argp.h"
#include "doca_error.h"
#include <cstdio>
}

class PyDFlow {
public:
    PyDFlow(std::string name);
    std::string name;
    int add_numbers(int a, int b);
    void create_testing_pipe();
    void create_entry();
    struct doca_flow_port *getPort(int portNr);
    void dumpPipeInformationForPort(int portNr, std::string fileName);
private:
    struct doca_flow_pipe *pipe;  // Nicht **pipe
    struct doca_flow_port *ports[2];
};




#endif