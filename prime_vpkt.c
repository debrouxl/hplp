#include "hpcalcs.h"
#include "logging.h"

// -----------------------------------------------
// Calcs - HP Prime virtual packets
// -----------------------------------------------


prime_vtl_pkt * prime_vtl_pkt_new(uint32_t size) {
    prime_vtl_pkt * pkt = (prime_vtl_pkt *)malloc(sizeof(*pkt));

    pkt->size = size;
    pkt->data = (uint8_t *)calloc(size, sizeof(*pkt->data));

    if (pkt->data == NULL) {
        free(pkt);
        pkt = NULL;
    }

    return pkt;
}

void prime_vtl_pkt_del(prime_vtl_pkt * pkt) {
    if (pkt != NULL) {
        free(pkt->data);
        free(pkt);
    }
    else {
        hpcalcs_error("%s: pkt is NULL", __FUNCTION__);
    }
}

int prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        prime_raw_pkt raw;
        // TODO: fragmentation.
        raw.size = (uint8_t)pkt->size;
        memcpy(&raw.data, pkt->data, PRIME_RAW_DATA_SIZE);
        res = prime_send(handle, &raw);
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

int prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        prime_raw_pkt raw;
        // TODO: reassembly.
        raw.size = PRIME_RAW_DATA_SIZE;
        for (;;) {
            res = prime_recv(handle, &raw);
            if (res == 0) {
                do_print_buf(raw.data, raw.size);
            }
            else {
                break;
            }
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}
