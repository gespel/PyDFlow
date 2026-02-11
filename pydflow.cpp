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

Pipe *PyDFlow::create_pipe() {
    auto *p = new Pipe(ports);
    return p;
}

void PyDFlow::dumpPipeInformationForPort(int portNr, std::string fileName) {
    FILE *fptr;

    // Open a file in writing mode
    fptr = fopen(fileName.c_str(), "a+");
    if (fptr == NULL) {
        printf("Error opening file!\n");
        return;
    }
    printf("[PyDFlow] Dumping of port data is not available in this DOCA version, but would be dumped to file %s...\n", fileName.c_str());
    // DOCA_LOG_INFO("Dumping port %i data to file %s...", portNr, fileName.c_str());
    // doca_flow_port_pipes_dump not available in this DOCA version
    // doca_flow_port_pipes_dump(ports[portNr], fptr);
    fprintf(fptr, "Port %d information\n", portNr);
    fclose(fptr);
}