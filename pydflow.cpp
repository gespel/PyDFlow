#include "pydflow.h"
#include "doca_dev.h"
#include "doca_error.h"
#include "doca_flow.h"
#include "doca_log.h"
#include "flow_common.h"

DOCA_LOG_REGISTER(0);

PyDFlowPipe::PyDFlowPipe(std::string name) : name(name) {
    //doca_error_t result;
	struct doca_log_backend *sdk_log;
    struct flow_resources resource = {1};
    uint32_t nr_shared_resources[SHARED_RESOURCE_NUM_VALUES] = {0};
    struct doca_dev *dev_arr[1];
    application_dpdk_config dpdk_config = {
        { 2, 4, 2},
    };
    struct doca_flow_shared_resource_cfg cfg = { DOCA_FLOW_PIPE_DOMAIN_DEFAULT};
    doca_error_t result;

	result = doca_log_backend_create_standard();
	result = doca_log_backend_create_with_file_sdk(stderr, &sdk_log);
	result = doca_log_backend_set_sdk_level(sdk_log, DOCA_LOG_LEVEL_WARNING);

    printf("Starting PyDFlowPipe Wrapper: %s\n", name.c_str());

	result = doca_argp_init(name.c_str(), NULL);
	if (result != DOCA_SUCCESS) {
		printf("Failed to init ARGP resources: %s\n", doca_error_get_descr(result));
	}
	doca_argp_set_dpdk_program(dpdk_init);
    char *n = (char*)name.c_str();
	char **args = (char**)malloc(sizeof(char*)*16);
    args[0] = n;
    args[1] = (char*)"-a";
    args[2] = (char*)"0d:00.0,dv_flow_en=2";
    args[3] = (char*)"-a";
    args[4] = (char*)"0d:00.1,dv_flow_en=2";
    args[5] = (char*)"-c";
    args[6] = (char*)"0xff";
    args[7] = (char*)"--";
    args[8] = (char*)"-l";
    args[9] = (char*)"70";
    result = doca_argp_start(10, args);
	if (result != DOCA_SUCCESS) {
		printf("Failed to parse sample input: %s\n", doca_error_get_descr(result));
	}
    
	result = dpdk_queues_and_ports_init(&dpdk_config);
	if (result != DOCA_SUCCESS) {
		printf("Failed to update ports and queues: %s\n", doca_error_get_descr(result));
        exit(-1);
	}

    result = init_doca_flow(4, "vnf,hws", &resource, nr_shared_resources);
    if (result != DOCA_SUCCESS) {
        printf("Failed to initialize DOCA Flow\n");
    }

    memset(dev_arr, 0, sizeof(struct doca_dev *));

    result = init_doca_flow_ports(2, ports, true, dev_arr);
    if (result != DOCA_SUCCESS) {
        printf("Failed to init DOCA Flow ports: %s", doca_error_get_descr(result));
        exit(-1);
    }
}

struct doca_flow_port *PyDFlowPipe::getPort(int portNr) {
    return ports[portNr];
}

int PyDFlowPipe::add_numbers(int a, int b) {
    return a + b;
}

void PyDFlowPipe::create_pipe() {
    
}

void PyDFlowPipe::create_testing_pipe() {
    struct doca_flow_match match;
    struct doca_flow_match match_mask;
    struct doca_flow_monitor monitor;
    struct doca_flow_actions actions0, actions1, actions2, *actions_arr[2];
    struct doca_flow_fwd fwd, fwd_miss;
    struct doca_flow_pipe_cfg *pipe_cfg;
    doca_error_t result;

    memset(&match, 0, sizeof(match));
    memset(&match_mask, 0, sizeof(match_mask));
    memset(&monitor, 0, sizeof(monitor));
    memset(&actions0, 0, sizeof(actions0));
    memset(&actions1, 0, sizeof(actions1));
    memset(&actions2, 0, sizeof(actions2));
    memset(&fwd, 0, sizeof(fwd));
    memset(&fwd_miss, 0, sizeof(fwd_miss));

    //match.outer.l3_type = DOCA_FLOW_L3_TYPE_IP4;
    //match.outer.ip4.src_ip = BE_IPV4_ADDR(255, 255, 255, 255);
    //match_mask.outer.ip4.src_ip = BE_IPV4_ADDR(0, 0, 0, 1);
    //DOCA_LOG_INFO("%d", match_mask.outer.ip4.src_ip);

    SET_MAC_ADDR(actions0.outer.eth.dst_mac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    SET_MAC_ADDR(actions0.outer.eth.src_mac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

    actions_arr[0] = &actions0;

    result = doca_flow_pipe_cfg_create(&pipe_cfg, ports[0]);
    if (result != DOCA_SUCCESS) {
            printf("Failed to create doca_flow_pipe_cfg: %s\n", doca_error_get_descr(result));
        goto destroy_pipe_cfg;    
    }

    result = set_flow_pipe_cfg(pipe_cfg, "SHARED_COUNTER_PIPE", DOCA_FLOW_PIPE_BASIC, true);
    if (result != DOCA_SUCCESS) {
            printf("Failed to set doca_flow_pipe_cfg: %s\n", doca_error_get_descr(result));
            goto destroy_pipe_cfg;
    }
    result = doca_flow_pipe_cfg_set_match(pipe_cfg, &match, &match_mask);
    if (result != DOCA_SUCCESS) {
            printf("Failed to set doca_flow_pipe_cfg match: %s\n", doca_error_get_descr(result));
            goto destroy_pipe_cfg;
    }
    result = doca_flow_pipe_cfg_set_actions(pipe_cfg, actions_arr, NULL, NULL, 1);
    if (result != DOCA_SUCCESS) {
            printf("Failed to set doca_flow_pipe_cfg actions: %s\n", doca_error_get_descr(result))
;
            goto destroy_pipe_cfg;
    }
    result = doca_flow_pipe_cfg_set_monitor(pipe_cfg, &monitor);
    if (result != DOCA_SUCCESS) {
            printf("Failed to set doca_flow_pipe_cfg monitor: %s\n", doca_error_get_descr(result))
;
            goto destroy_pipe_cfg;
    }

    fwd.type = DOCA_FLOW_FWD_PORT;
    fwd.port_id = 1;
    fwd_miss.type = DOCA_FLOW_FWD_DROP;

    DOCA_LOG_INFO("Creating pipe now...");
    result = doca_flow_pipe_create(pipe_cfg, &fwd, &fwd_miss, &pipe);
    DOCA_LOG_INFO("Pipe has been successfully created!");

destroy_pipe_cfg:
    doca_flow_pipe_cfg_destroy(pipe_cfg);

}

void PyDFlowPipe::create_entry() {
    struct doca_flow_match match;
	struct doca_flow_actions actions;
	struct doca_flow_monitor monitor;
	struct doca_flow_pipe_entry *entry_mac;
	struct doca_flow_fwd fwd;
    struct entries_status status;

	doca_error_t result;

	memset(&match, 0, sizeof(match));
	memset(&actions, 0, sizeof(actions));
	memset(&monitor, 0, sizeof(monitor));
	memset(&fwd, 0, sizeof(fwd));
    memset(&status, 0, sizeof(status));

	match.outer.ip4.src_ip = BE_IPV4_ADDR(0, 0, 0, 0);

	actions.action_idx = 0;

	SET_MAC_ADDR(actions.outer.eth.dst_mac, 0xa0, 0x88, 0xc2, 0xb5, 0xf4, 0x5a);	
	SET_MAC_ADDR(actions.outer.eth.src_mac, 0xc4, 0x70, 0xbd, 0xa0, 0x56, 0xbd);

    DOCA_LOG_INFO("Adding entry to pipe...");
	result = doca_flow_pipe_add_entry(0, pipe, &match, &actions, &monitor, NULL, 0, &status, &entry_mac);
	if (result != DOCA_SUCCESS) {
		DOCA_LOG_ERR("Failed to add entry: %s", doca_error_get_descr(result));
		exit(-1);
	}
    DOCA_LOG_INFO("Entry successfully added to pipe!");
}

void PyDFlowPipe::dumpPipeInformationForPort(int portNr, std::string fileName) {
    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen(fileName.c_str(), "a+");
    if (fptr == NULL) {
        printf("Error opening file!\n");
        return;
    }
    DOCA_LOG_INFO("Dumping port %i data to file %s...", portNr, fileName.c_str());
    doca_flow_port_pipes_dump(ports[portNr], fptr);
    fclose(fptr);
}