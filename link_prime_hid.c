#include "link_prime_hid.h"
#include "logging.h"

static int cable_prime_hid_open (cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        hid_device * device_handle = hid_open(USB_VID_HP, USB_PID_PRIME, NULL);
        if (device_handle) {
            handle->model = CABLE_PRIME_HID;
            handle->handle = (void *)device_handle;
            handle->fncts = &cable_prime_hid_fncts;
            handle->read_timeout = 500;
            handle->open = 1;
            handle->busy = 0;
            res = 0;
            hpcables_error("%s: cable open succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: cable open failed", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime_hid_close (cable_handle * handle) {
    int res = -1;
    if (handle != NULL) {
        hid_device * device_handle = (hid_device *)handle->handle;
        if (device_handle != NULL) {
            hid_close(device_handle);
            res = 0;
            hpcables_error("%s: cable close succeeded", __FUNCTION__);
        }
        else {
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: handle is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime_hid_send (cable_handle * handle, uint8_t * data, uint32_t len) {
    int res = -1;
    if (handle != NULL && data != NULL) {
        hid_device * device_handle = (hid_device *)handle->handle;
        if (device_handle != NULL) {
            res = hid_write(device_handle, data, len);
            if (res >= 0) {
                hpcables_info("%s: wrote %d bytes", __FUNCTION__, res);
                res = 0; // Indicate success.
            }
            else {
                hpcables_error("%s: write failed %ls", __FUNCTION__, hid_error(device_handle));
            }
        }
        else {
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

static int cable_prime_hid_recv (cable_handle * handle, uint8_t * data, uint32_t * len) {
    int res = -1;
    if (handle != NULL && data != NULL && len != NULL) {
        hid_device * device_handle = (hid_device *)handle->handle;
        if (device_handle != NULL) {
            res = hid_read_timeout(device_handle, data, PRIME_RAW_DATA_SIZE, handle->read_timeout);
            if (res >= 0) {
                hpcables_info("%s: read %d bytes", __FUNCTION__, res);
                *len = res;
                res = 0; // Indicate success
            }
            else {
                hpcables_error("%s: read failed", __FUNCTION__);
            }
        }
        else {
            hpcables_error("%s: device_handle is NULL", __FUNCTION__);
        }
    }
    else {
        hpcables_error("%s: an argument is NULL", __FUNCTION__);
    }
    return res;
}

const cable_fncts cable_prime_hid_fncts =
{
    CABLE_PRIME_HID,
    "Prime HID cable",
    "Prime HID cable",
    &cable_prime_hid_open,
    &cable_prime_hid_close,
    &cable_prime_hid_send,
    &cable_prime_hid_recv
};
