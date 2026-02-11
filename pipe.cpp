#include "pipe.h"

Pipe::Pipe(struct doca_flow_port **ports) {
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

    //SET_MAC_ADDR(actions0.outer.eth.dst_mac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);
    //SET_MAC_ADDR(actions0.outer.eth.src_mac, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff);

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

    printf("[PyDFlow] Creating pipe now...\n");
    result = doca_flow_pipe_create(pipe_cfg, &fwd, &fwd_miss, &pipe);
    if (result != DOCA_SUCCESS) {
            printf("[PyDFlow] Failed to create doca_flow_pipe: %s\n", doca_error_get_descr(result));
            goto destroy_pipe_cfg;
    }
    else {
        printf("[PyDFlow] Pipe has been successfully created!\n");
    }

destroy_pipe_cfg:
    doca_flow_pipe_cfg_destroy(pipe_cfg);

}

void Pipe::create_entry() {
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

    printf("[PyDFlow] Adding entry to pipe...\n");
    result = doca_flow_pipe_add_entry(0, pipe, &match, 0, &actions, &monitor, &fwd, 0, &status, &entry_mac);
    if (result != DOCA_SUCCESS) {
        printf("[PyDFlow] Failed to add entry: %s\n", doca_error_get_descr(result));
        exit(-1);
    }
    printf("[PyDFlow] Entry successfully added to pipe!\n");
}