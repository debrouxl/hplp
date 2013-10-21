#include "hpcalcs.h"
#include "logging.h"

int calc_prime_get_infos (calc_handle * handle) {
    int res;

    res = calc_prime_s_get_infos(handle);
    if (res == 0) {
        res = calc_prime_r_get_infos(handle);
        if (res != 0) {
            hpcalcs_error("%s: r_get_infos failed", __FUNCTION__);
        }
    }
    else {
        hpcalcs_error("%s: s_get_infos failed", __FUNCTION__);
    }
    return res;
}

const calc_fncts calc_prime_fncts =
{
    CALC_PRIME,
    &calc_prime_get_infos
};
