#include "hpcables.h"
#include "logging.h"

#include "hidapi.h"

extern const cable_fncts cable_prime_hid_fncts;

const cable_fncts * hpcables_all_cables[CABLE_MAX] = {
    NULL,
    &cable_prime_hid_fncts
};


int hpcables_init(void) {
    int res = hid_init();
    if (res == 0) {
        hpcables_info("%s: init succeeded", __FUNCTION__);
    }
    else {
        hpcables_error("%s: init failed", __FUNCTION__);
    }
    return res;
}

int hpcables_exit(void) {
    hid_exit();
    return 0;
}


cable_handle * hpcables_handle_new(cable_model model) {
    cable_handle * handle = NULL;
    if (model > CABLE_NONE && model < CABLE_MAX) {
        handle = (cable_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->fncts = hpcables_all_cables[model];
            hpcables_info("%s: handle allocation succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: handle allocation failed", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: invalid model", __FUNCTION__);
    }

    return handle;
}

int hpcables_handle_del(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        handle = NULL;
        res = 0;
        hpcables_info("%s: handle deletion succeeded", __FUNCTION__);
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }

    return res;
}


int hpcables_cable_open(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*open) (cable_handle *) = fncts->open;
            if (open != NULL) {
                res = (*open)(handle);
                if (res == 0) {
                    handle->open = 1;
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: open failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->open is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcables_cable_close(cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*close) (cable_handle *) = fncts->close;
            if (close != NULL) {
                res = (*close)(handle);
                if (res == 0) {
                    handle->open = 0;
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: close failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->close is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcables_cable_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*send) (cable_handle *, uint8_t *, uint32_t) = fncts->send;
            if (send != NULL) {
                res = (*send)(handle, data, len);
                if (res == 0) {
                    hpcables_info("%s: send succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: send failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->send is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcables_cable_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res = -1;
    if (handle != NULL) {
        const cable_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*recv) (cable_handle *, uint8_t *, uint32_t *) = fncts->recv;
            if (recv != NULL) {
                res = (*recv)(handle, data, len);
                if (res == 0) {
                    hpcables_info("%s: recv succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: recv failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->recv is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}
