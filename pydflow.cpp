#include "pydflow.h"
#include "doca_dev.h"
#include "doca_error.h"
#include "doca_flow.h"
#include "doca_log.h"
#include "flow_common.h"

DOCA_LOG_REGISTER(0);

struct doca_dev *PyDFlow::open_doca_dev_by_pci(const char *pci_bdf) {
    struct doca_devinfo **list;
    uint32_t nb;
    doca_error_t err;

    err = doca_devinfo_create_list(&list, &nb);
    if (err != DOCA_SUCCESS) {
        printf("devinfo_create_list failed\n");
        return NULL;
    }

    struct doca_dev *dev = NULL;

    for (uint32_t i = 0; i < nb; i++) {
        char pci[DOCA_DEVINFO_PCI_ADDR_SIZE] = {0};
		uint8_t ipv4[4];

        if (doca_devinfo_get_pci_addr_str(list[i], pci) != DOCA_SUCCESS)
            continue;

        if (strcmp(pci, pci_bdf) == 0) {
			doca_devinfo_get_ipv4_addr(list[i], ipv4, DOCA_DEVINFO_IPV4_ADDR_SIZE);
			DOCA_LOG_INFO("IPv4: %u.%u.%u.%u", ipv4[0], ipv4[1], ipv4[2], ipv4[3]);
			err = doca_dev_open(list[i], &dev);
            break;
        }
    }

    doca_devinfo_destroy_list(list);
    return dev;
}

PyDFlow::PyDFlow(std::string name, std::string pcie_address_port_a, std::string pcie_address_port_b) : name(name) {
    //doca_error_t result;
	struct doca_log_backend *sdk_log;
    struct flow_resources resource = {};
    resource.mode = DOCA_FLOW_RESOURCE_MODE_SYSTEM;
    uint32_t nr_shared_resources[SHARED_RESOURCE_NUM_VALUES] = {0};
    uint32_t action_mem[2] = {0};
    struct doca_dev *dev_arr[1];
    application_dpdk_config dpdk_config = {
        { 2, 4,},
    };
    doca_error_t result;

	result = doca_log_backend_create_standard();
	result = doca_log_backend_create_with_file_sdk(stderr, &sdk_log);
	result = doca_log_backend_set_sdk_level(sdk_log, DOCA_LOG_LEVEL_WARNING);

    printf("Starting PyDFlow Wrapper: %s\n", name.c_str());

	result = doca_argp_init(name.c_str(), NULL);
	if (result != DOCA_SUCCESS) {
		printf("Failed to init ARGP resources: %s\n", doca_error_get_descr(result));
	}
	doca_argp_set_dpdk_program(dpdk_init);
    char *n = (char*)name.c_str();
	char **args = (char**)malloc(sizeof(char*)*16);
    
    // Build PCIe device strings with dv_flow_en=2
    std::string pcie_a = pcie_address_port_a + ",dv_flow_en=2";
    std::string pcie_b = pcie_address_port_b + ",dv_flow_en=2";
    
    args[0] = n;
    /*args[1] = (char*)"-a";
    args[2] = (char*)pcie_a.c_str();
    args[3] = (char*)"-a";
    args[4] = (char*)pcie_b.c_str();
    args[5] = (char*)"-c";
    args[6] = (char*)"0xff";
    args[7] = (char*)"--";
    args[8] = (char*)"-l";
    args[9] = (char*)"70";*/
    result = doca_argp_start(1, args);
	if (result != DOCA_SUCCESS) {
		printf("Failed to parse sample input: %s\n", doca_error_get_descr(result));
	}
    
	result = dpdk_queues_and_ports_init(&dpdk_config);
	if (result != DOCA_SUCCESS) {
		printf("Failed to update ports and queues: %s\n", doca_error_get_descr(result));
        exit(-1);
	}

    result = init_doca_flow(dpdk_config.port_config.nb_queues, "vnf,hws", &resource, nr_shared_resources);
    if (result != DOCA_SUCCESS) {
        printf("Failed to initialize DOCA Flow\n");
    }

    std::string full_pcie_a = "0000:" + pcie_address_port_a;
    std::string full_pcie_b = "0000:" + pcie_address_port_b;

    struct doca_dev *dev1 = open_doca_dev_by_pci(full_pcie_a.c_str());
	struct doca_dev *dev2 = open_doca_dev_by_pci(full_pcie_b.c_str());

    dev_arr[0] = dev1;
	dev_arr[1] = dev2;

    result = init_doca_flow_ports(2, ports, true, dev_arr, action_mem, &resource);
    if (result != DOCA_SUCCESS) {
        printf("Failed to init DOCA Flow ports: %s", doca_error_get_descr(result));
        exit(-1);
    }
}

struct doca_flow_port *PyDFlow::getPort(int portNr) {
    return ports[portNr];
}

int PyDFlow::add_numbers(int a, int b) {
    return a + b;
}

Pipe *PyDFlow::create_pipe() {
    auto *p = new Pipe(ports);
    return p;
}

void PyDFlow::create_testing_pipe() {
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

    /*SET_MAC_ADDR(actions0.outer.eth.dst_mac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    SET_MAC_ADDR(actions0.outer.eth.src_mac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);*/

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

void PyDFlow::create_entry() {
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

	SET_MAC_ADDR(actions.outer.eth.dst_mac, 0xa0, 0x88, 0xc2, 0xb5, 0xf4, 0x5a);	
	SET_MAC_ADDR(actions.outer.eth.src_mac, 0xc4, 0x70, 0xbd, 0xa0, 0x56, 0xbd);

    DOCA_LOG_INFO("Adding entry to pipe...");
	result = doca_flow_pipe_add_entry(0, pipe, &match, 0, &actions, &monitor, &fwd, 0, &status, &entry_mac);
	if (result != DOCA_SUCCESS) {
		DOCA_LOG_ERR("Failed to add entry: %s", doca_error_get_descr(result));
		exit(-1);
	}
    DOCA_LOG_INFO("Entry successfully added to pipe!");
}

void PyDFlow::dumpPipeInformationForPort(int portNr, std::string fileName) {
    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen(fileName.c_str(), "a+");
    if (fptr == NULL) {
        printf("Error opening file!\n");
        return;
    }
    DOCA_LOG_INFO("Dumping port %i data to file %s...", portNr, fileName.c_str());
    // doca_flow_port_pipes_dump not available in this DOCA version
    // doca_flow_port_pipes_dump(ports[portNr], fptr);
    fprintf(fptr, "Port %d information\n", portNr);
    fclose(fptr);
}