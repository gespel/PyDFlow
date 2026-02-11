#pragma once
#include <string>
#include "pipe.h"
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

class Pipe {
public:
    Pipe(struct doca_flow_port **ports);
    void create_entry();

private:
    struct doca_flow_pipe *pipe;  // Nicht **pipe
    struct doca_flow_port *ports[2];
};