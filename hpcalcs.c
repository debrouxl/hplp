#include "hpcables.h"
#include "hpcalcs.h"
#include "logging.h"

extern const calc_fncts calc_prime_fncts;

const calc_fncts * hpcalcs_all_calcs[CALC_MAX] = {
    NULL,
    &calc_prime_fncts
};

int hpcalcs_init(void) {
    return 0;
}

int hpcalcs_exit(void) {
    return 0;
}

calc_handle * hpcalcs_handle_new(calc_model model) {
    calc_handle * handle = NULL;
    if (model > CALC_NONE && model < CALC_MAX) {
        handle = (calc_handle *)calloc(1, sizeof(*handle));

        if (handle != NULL) {
            handle->model = model;
            handle->fncts = hpcalcs_all_calcs[model];
            hpcalcs_info("%s: calc handle allocation succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: calc handle allocation failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: invalid model", __FUNCTION__);
    }
    return handle;
}

int hpcalcs_handle_del(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        free(handle->handle);
        handle->handle = NULL;

        free(handle);
        handle = NULL;
        res = 0;
        hpcalcs_info("%s: calc handle deletion succeeded", __FUNCTION__);
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcalcs_cable_attach(calc_handle * handle, cable_handle * cable) {
    int res = -1;
    if (handle != NULL && cable != NULL) {
        handle->cable = cable;
        handle->attached = 1;

        if (hpcables_cable_open(cable) == 0) {
            handle->open = 1;
            res = 0;
            hpcalcs_info("%s: cable open succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: cable open failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

int hpcalcs_cable_detach(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        if (hpcables_cable_close(handle->cable) == 0) {
            handle->open = 0;
            handle->attached = 0;
            handle->cable = NULL;
            res = 0;
            hpcalcs_info("%s: cable close succeeded", __FUNCTION__);
        }
        else {
            hpcalcs_error("%s: cable close failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

int hpcalcs_calc_get_infos(calc_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        const calc_fncts * fncts = handle->fncts;
        if (fncts != NULL) {
            int (*get_infos) (calc_handle *) = fncts->get_infos;
            if (get_infos != NULL) {
                res = (*get_infos)(handle);
                if (res == 0) {
                    hpcables_info("%s: get_infos succeeded", __FUNCTION__);
                }
                else {
                    hpcables_error("%s: get_infos failed", __FUNCTION__);
                }
            }
            else {
                hpcables_error("%s: fncts->get_infos is NULL", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: fncts is NULL", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}
