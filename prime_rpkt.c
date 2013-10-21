#include "hpcalcs.h"
#include "logging.h"

int prime_send(calc_handle * handle, prime_raw_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            res = hpcables_cable_send(cable, pkt->data, pkt->size);
            if (res == 0) {
                hpcables_info("%s: send succeeded", __FUNCTION__);
            }
            else {
                hpcables_error("%s: send failed", __FUNCTION__);
            }
        }
        else {
            hpcalcs_error("%s: cable is NULL", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

int prime_recv(calc_handle * handle, prime_raw_pkt * pkt) {
    int res = -1;
    if (handle != NULL && pkt != NULL) {
        cable_handle * cable = handle->cable;
        if (cable != NULL) {
            res = hpcables_cable_recv(cable, pkt->data, &pkt->size);
            if (res == 0) {
                hpcables_info("%s: send succeeded", __FUNCTION__);
            }
            else {
                hpcables_error("%s: send failed", __FUNCTION__);
            }
        }
        else {
            hpcalcs_error("%s: cable is NULL", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}
