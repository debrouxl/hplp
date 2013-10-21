#include "hpcalcs.h"
#include "logging.h"

// -----------------------------------------------
// Calcs - HP Prime command protocols implementation
// -----------------------------------------------
int calc_prime_s_get_infos (calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            uint8_t * ptr;

            // 1) 00 E7 01 00 00 00 0A 00 00 54 1E 0D 0A 13 (and the rest seems to vary), whatever that means
            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = 0xE7; // Command ?
            *ptr++ = 0x01; // 0x00000001, LE ?
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x0A; // '\n' ?
            *ptr++ = 0x00;
            *ptr++ = 0x00;
            *ptr++ = 0x54; // 'T'
            *ptr++ = 0x1E;
            *ptr++ = 0x0D; // '\r'
            *ptr++ = 0x0A; // '\n'
            *ptr++ = 0x13;
            res = prime_send_data(handle, pkt);
            if (res != 0) {
                goto end;
            }

            // 2) 00 FF (and the rest seems to vary)
            memset(pkt->data, 0, PRIME_RAW_DATA_SIZE);
            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = 0xFF; // Command ?
            res = prime_send_data(handle, pkt);
            if (res != 0) {
                goto end;
            }

            // 3) 00 FA (and the rest seems to vary)
            memset(pkt->data, 0, PRIME_RAW_DATA_SIZE);
            ptr = pkt->data;
            *ptr++ = 0x00; // Report number ?
            *ptr++ = 0x00; // ?
            *ptr++ = 0xFA; // Command ?
            res = prime_send_data(handle, pkt);
            if (res != 0) {
                goto end;
            }

end:
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int calc_prime_r_get_infos (calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        prime_vtl_pkt * pkt = prime_vtl_pkt_new(PRIME_RAW_DATA_SIZE);
        if (pkt != NULL) {
            res = prime_recv_data(handle, pkt);
            if (res != 0) {
                goto end;
            }
end:
            prime_vtl_pkt_del(pkt);
        }
        else {
            hpcalcs_error("%s: couldn't create packet", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}
