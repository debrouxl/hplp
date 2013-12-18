/*
 * libhpcalcs: hand-helds support libraries.
 * Copyright (C) 2013 Lionel Debroux
 * Code patterns and snippets borrowed from libticables & libticalcs:
 * Copyright (C) 1999-2009 Romain Liévin
 * Copyright (C) 2009-2013 Lionel Debroux
 * Copyright (C) 1999-2013 libti* contributors.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/**
 * \file hpcalcs.h Calcs: base part.
 */

#ifndef __HPLIBS_CALCS_H__
#define __HPLIBS_CALCS_H__

#include <stdint.h>
#include <stdarg.h>
#include <time.h>

#include "hplibs.h"
#include "hpfiles.h"
#include "hpcables.h"

//! Opaque type for internal _calc_fncts.
typedef struct _calc_fncts calc_fncts;
//! Opaque type for internal _calc_handle.
typedef struct _calc_handle calc_handle;

//! Indices of the function pointers in _calc_fncts.
typedef enum {
    CALC_FNCT_CHECK_READY = 0,
    CALC_FNCT_GET_INFOS = 1,
    CALC_FNCT_SET_DATE_TIME = 2,
    CALC_FNCT_RECV_SCREEN = 3,
    CALC_FNCT_SEND_FILE = 4,
    CALC_FNCT_RECV_FILE = 5,
    CALC_FNCT_RECV_BACKUP = 6,
    CALC_FNCT_SEND_KEY = 7,
    CALC_FNCT_SEND_CHAT = 8,
    CALC_FNCT_LAST ///< Keep this one last
} calc_fncts_idx;

//! Used in the bit field of _calc_fncts, indicating whether a given calculator supports a given operation.
typedef enum {
    CALC_OPS_NONE = 0,
    CALC_OPS_CHECK_READY = (1 << CALC_FNCT_CHECK_READY),
    CALC_OPS_GET_INFOS = (1 << CALC_FNCT_GET_INFOS),
    CALC_OPS_SET_DATE_TIME = (1 << CALC_FNCT_SET_DATE_TIME),
    CALC_OPS_RECV_SCREEN = (1 << CALC_FNCT_RECV_SCREEN),
    CALC_OPS_SEND_FILE = (1 << CALC_FNCT_SEND_FILE),
    CALC_OPS_RECV_FILE = (1 << CALC_FNCT_RECV_FILE),
    CALC_OPS_RECV_BACKUP = (1 << CALC_FNCT_RECV_BACKUP),
    CALC_OPS_SEND_KEY = (1 << CALC_FNCT_SEND_KEY),
    CALC_OPS_SEND_CHAT = (1 << CALC_FNCT_SEND_CHAT)
} calc_features_operations;

//! Screenshot formats supported by the calculators, list is known to be incomplete.
typedef enum {
    // 5 is triggered periodically by the official connectivity kit. It returns something with a PNG header, but much smaller.
    CALC_SCREENSHOT_FORMAT_FIRST = 8,
    CALC_SCREENSHOT_FORMAT_PRIME_PNG_320x240x16 = 8,
    CALC_SCREENSHOT_FORMAT_PRIME_PNG_320x240x4 = 9,
    CALC_SCREENSHOT_FORMAT_PRIME_PNG_160x120x16 = 10,
    CALC_SCREENSHOT_FORMAT_PRIME_PNG_160x120x4 = 11,
    CALC_SCREENSHOT_FORMAT_LAST ///< Keep this one last
} calc_screenshot_format;

//! Structure containing information returned by the calculator. This will change a lot when the returned data is better documented.
typedef struct {
    uint32_t size;
    uint8_t * data;
} calc_infos;

//! Internal structure containing information about the calculator, and function pointers.
struct _calc_fncts {
    calc_model model;
    const char * name;
    const char * description;
    int features;
    int (*check_ready) (calc_handle * handle, uint8_t ** out_data, uint32_t * out_size);
    int (*get_infos) (calc_handle * handle, calc_infos * infos);
    int (*set_date_time) (calc_handle * handle, time_t timestamp);
    int (*recv_screen) (calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);
    int (*send_file) (calc_handle * handle, files_var_entry * file);
    int (*recv_file) (calc_handle * handle, files_var_entry * request, files_var_entry ** out_file);
    int (*recv_backup) (calc_handle * handle, files_var_entry *** out_vars);
    int (*send_key) (calc_handle * handle, uint32_t code);
    int (*send_chat) (calc_handle * handle, uint16_t * data, uint32_t size);
};

//! Internal structure containing state about the calculator, returned and passed around by the user.
struct _calc_handle {
    calc_model model;
    void * handle;
    const calc_fncts * fncts;
    cable_handle * cable;
    int attached; // Should be made explicitly atomic with GCC >= 4.7 or Clang, but int is atomic on most ISAs anyway.
    int open; // Should be made explicitly atomic with GCC >= 4.7 or Clang, but int is atomic on most ISAs anyway.
    int busy; // Should be made explicitly atomic with GCC >= 4.7 or Clang, but int is atomic on most ISAs anyway.
};


//! Structure defining a raw packet for the Prime, used at the lowest layer of the protocol implementation.
typedef struct
{
    uint32_t size;
    uint8_t data[PRIME_RAW_HID_DATA_SIZE + 1];
} prime_raw_hid_pkt;


//! Structure defining a virtual packet for the Prime, used at the middle layer of the protocol implementation (fragmented to / reassembled from raw packets).
typedef struct
{
    uint32_t size;
    uint8_t * data;
    uint8_t cmd;
} prime_vtl_pkt;


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes library internals. Must be called before any other libhpcalcs function.
 * \param log_callback callback function for receiving logging output.
 * \return Whether the initialization succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpcalcs_init(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Tears down library internals. No other libhpcalcs function can be called after this one.
 * \return Whether the teardown succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpcalcs_exit(void);

/**
 * \brief Returns the library version string.
 * \return The library version string, usually under the form "X.Y.Z".
 **/
HPEXPORT const char* HPCALL hpcalcs_version_get(void);

/**
 * \brief Gets the error message if the error was produced by this library
 * \param number the error number (from internal error.h)
 * \param message out pointer for a newly allocated text error message, which must be freed by the caller
 * \return 0 if the error was produced by this library, otherwise the error number (for propagation).
 **/
HPEXPORT int HPCALL hpcalcs_error_get(int number, char **message);

/**
 * \brief Sets the callback function used by the library for logging
 * \param log_callback function pointer
 */
HPEXPORT void HPCALL hpcalcs_log_set_callback(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Sets the log level of the library, for controlling how much logging output is produced.
 * \param log_level log level (from hplibs.h)
 * \return the previous log level
 */
HPEXPORT hplibs_logging_level HPCALL hpcalcs_log_set_level(hplibs_logging_level log_level);


/**
 * \brief Create a new handle (opaque structure) for the given calc model.
 * The handle must be freed with \a hpcalcs_handle_del when no longer needed.
 * \param model the calculator model.
 * \return NULL if an error occurred, otherwise a valid handle.
 **/
HPEXPORT calc_handle * HPCALL hpcalcs_handle_new(calc_model model);
/**
 * \brief Deletes a handle (opaque structure) created by \a hpcalcs_handle_new().
 * \param handle the handle to be deleted.
 * \return 0 if the deletion succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcalcs_handle_del(calc_handle * handle);
/**
 * \brief Shows basic information about a handle
 * \param handle the handle to be dumped.
 * \return 0 if the handle was non-NULL, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcalcs_handle_display(calc_handle * handle);

/**
 * \brief Retrieves the calc model from the given calc handle.
 * \param handle the calc handle
 * \return the calc model corresponding to the given handle.
 */
HPEXPORT calc_model HPCALL hpcalcs_get_model(calc_handle * handle);

/**
 * \brief Opens and attaches the given cable for use with the given calculator.
 * \param handle the calculator handle.
 * \param cable the cable handle.
 * \return 0 upon success, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcalcs_cable_attach(calc_handle * handle, cable_handle * cable);
/**
 * \brief Closes and detaches the cable attached to the given calculator handle.
 * \param handle the calculator handle.
 * \return 0 upon success, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcalcs_cable_detach(calc_handle * handle);
/**
 * \brief Retrieves the cable handle attached to the given calculator handle, if any.
 * \param handle the calculator handle.
 * \return NULL if an error occurred, otherwise a cable handle.
 **/
HPEXPORT cable_handle * HPCALL hpcalcs_cable_get(calc_handle * handle);

/**
 * \brief Checks whether the calculator is ready
 * \param handle the calculator handle.
 * \param out_data storage area for information contained in the calculator's reply.
 * \param out_size storage area for size of the information contained in the calculator's reply.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_check_ready(calc_handle * handle, uint8_t ** out_data, uint32_t * out_size);
/**
 * \brief Retrieves some information, such as firmware version, from the calculator.
 * \param handle the calculator handle.
 * \param infos storage area for information contained in the reply.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_get_infos(calc_handle * handle, calc_infos * infos);
/**
 * \brief Sets the calculator's date and time from a standard C89 / *nix timestamp.
 * \param handle the calculator handle.
 * \param timestamp the timestamp since the Epoch (1970).
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_set_date_time(calc_handle * handle, time_t timestamp);
/**
 * \brief Retrieves a screenshot from the calculator
 * \param handle the calculator handle.
 * \param format the desired screenshot format.
 * \param out_data storage area for screenshot contained in the calculator's reply
 * \param out_size storage area for size of the screenshot contained in the calculator's reply.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_recv_screen(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);
/**
 * \brief Sends a file to the calculator.
 * \param handle the calculator handle.
 * \param file information about the file to be sent.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_send_file(calc_handle * handle, files_var_entry * file);
/**
 * \brief Receives a file from the calculator.
 * \param handle the calculator handle.
 * \param request information about the file to be received.
 * \param out_file storage area for the file to be received.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_recv_file(calc_handle * handle, files_var_entry * request, files_var_entry ** out_file);
/**
 * \brief Receives a backup (made of multiple files) from the calculator.
 * \param handle the calculator handle.
 * \param out_vars storage area for the files to be received.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_recv_backup(calc_handle * handle, files_var_entry *** out_vars);
/**
 * \brief Sends a keypress to the calculator.
 * \param handle the calculator handle.
 * \param code the key code.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_send_key(calc_handle * handle, uint32_t code);
/**
 * \brief Sends chat data to the calculator.
 * \param handle the calculator handle.
 * \param data the data to be sent (UTF-16LE string, with U+0000 terminator, without UTF-16 LE BOM).
 * \param size the size of the data to be sent.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcalcs_calc_send_chat(calc_handle * handle, uint16_t * data, uint32_t size);


/**
 * \brief Sends the given raw packet to the Prime calculator using given calculator handle.
 * \param handle the calculator handle.
 * \param pkt the raw packet.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL prime_send(calc_handle * handle, prime_raw_hid_pkt * pkt);
/**
 * \brief Receives a raw packet from the Prime calculator using given calculator handle, and store the result to given packet.
 * \param handle the calculator handle.
 * \param pkt the dest raw packet.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL prime_recv(calc_handle * handle, prime_raw_hid_pkt * pkt);


/**
 * \brief Creates a virtual packet for the Prime calculator, preallocating the given size.
 * \param size the size to be preallocated.
 * \return NULL if an error occurred, a virtual packet otherwise.
 */
HPEXPORT prime_vtl_pkt * HPCALL prime_vtl_pkt_new(uint32_t size);
/**
 * \brief Deletes a virtual packet for the Prime calculator.
 * \param pkt the packet to be deleted.
 */
HPEXPORT void HPCALL prime_vtl_pkt_del(prime_vtl_pkt * pkt);

/**
 * \brief Sends the given virtual packet to the Prime calculator using given calculator handle.
 * \param handle the calculator handle.
 * \param pkt the virtual packet.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL prime_send_data(calc_handle * handle, prime_vtl_pkt * pkt);
/**
 * \brief Receives a virtual packet from the Prime calculator using given calculator handle, and store the result to given packet.
 * \param handle the calculator handle.
 * \param pkt the dest virtual packet.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL prime_recv_data(calc_handle * handle, prime_vtl_pkt * pkt);


/**
 * \brief Converts a calculator model to a printable string.
 * \param model the calculator model.
 * \return the string corresponding to the calculator model.
 **/
HPEXPORT const char * HPCALL hpcalcs_model_to_string(calc_model model);
/**
 * \brief Converts a string to a supported calculator model, if possible.
 * \param string the string.
 * \return the calculator model corresponding to the string, CALC_NONE if failed.
 **/
HPEXPORT calc_model HPCALL hpcalcs_string_to_model(const char *str);

#ifdef __cplusplus
}
#endif

#endif
