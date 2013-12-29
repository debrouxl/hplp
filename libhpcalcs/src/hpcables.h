/*
 * libhpcables: hand-helds support libraries.
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
 * \file hpcables.h Cables: base part.
 */

#ifndef __HPLIBS_CABLES_H__
#define __HPLIBS_CABLES_H__

#include <stdint.h>
#include <stdarg.h>

#include "hplibs.h"

//! Opaque type for internal _cable_fncts.
typedef struct _cable_fncts cable_fncts;
//! Opaque type for internal _cable_handle.
typedef struct _cable_handle cable_handle;

//! Internal structure containing information about the cable, and function pointers.
struct _cable_fncts {
    cable_model model;
    const char * name;
    const char * description;
    int (*probe) (cable_handle * handle);
    int (*open) (cable_handle * handle);
    int (*close) (cable_handle * handle);
    int (*set_read_timeout) (cable_handle * handle, int read_timeout);
    int (*send) (cable_handle * handle, uint8_t * data, uint32_t len);
    int (*recv) (cable_handle * handle, uint8_t ** data, uint32_t * len);
};

//! Internal structure containing state about the cable, returned and passed around by the user.
struct _cable_handle {
    cable_model model;
    void * handle;
    const cable_fncts * fncts;
    int read_timeout;
    int open; // Should be made explicitly atomic with GCC >= 4.7 or Clang, but int is atomic on most ISAs anyway.
    int busy; // Should be made explicitly atomic with GCC >= 4.7 or Clang, but int is atomic on most ISAs anyway.
};


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes library internals. Must be called before any other libhpcables function.
 * \param log_callback callback function for receiving logging output.
 * \return Whether the initialization succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpcables_init(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Tears down library internals. No other libhpcables function can be called after this one.
 * \return Whether the teardown succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpcables_exit(void);

/**
 * \brief Returns the library version string.
 * \return The library version string, usually under the form "X.Y.Z".
 **/
HPEXPORT const char* HPCALL hpcables_version_get(void);

/**
 * \brief Gets the error message if the error was produced by this library
 * \param number the error number
 * \param message out pointer for a newly allocated text error message, which must be freed by the caller
 * \return 0 if the error was produced by this library, otherwise the error number (for propagation).
 **/
HPEXPORT int HPCALL hpcables_error_get(int number, char **message);

/**
 * \brief Sets the callback function used by the library for logging
 * \param log_callback function pointer
 */
HPEXPORT void HPCALL hpcables_log_set_callback(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Sets the log level of the library, for controlling how much logging output is produced.
 * \param log_level log level (from hplibs.h)
 * \return the previous log level
 */
HPEXPORT hplibs_logging_level HPCALL hpcables_log_set_level(hplibs_logging_level log_level);

/**
 * \brief Creates a new handle (opaque structure) for the given cable model.
 * The handle must be freed with \a hpcables_handle_del when no longer needed.
 * \param model the cable model.
 * \return NULL if an error occurred, otherwise a valid handle.
 **/
HPEXPORT cable_handle * HPCALL hpcables_handle_new(cable_model model);
/**
 * \brief Deletes a handle (opaque structure) created by \a hpcables_handle_new().
 * \param handle the handle to be deleted.
 * \return 0 if the deletion succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_handle_del(cable_handle * handle);
/**
 * \brief Shows basic information about a handle.
 * \param handle the handle to be dumped.
 * \return 0 if the handle was non-NULL, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_handle_display(cable_handle * handle);

/**
 * \brief Retrieves the cable model from the given cable handle.
 * \param handle the cable handle
 * \return the cable model corresponding to the given handle.
 */
HPEXPORT cable_model HPCALL hpcables_get_model(cable_handle * handle);

/**
 * \brief Gets the read timeout (in ms) for the given cable handle.
 * \param handle the cable handle
 * \return the current read timeout, 0 if error.
 */
HPEXPORT int HPCALL hpcables_options_get_read_timeout(cable_handle * handle);
/**
 * \brief Sets the timeout (in ms) for the given cable handle.
 * \param handle the cable handle
 * \param timeout the new timeout.
 * \return 0 if the operation succeeded, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcables_options_set_read_timeout(cable_handle * handle, int timeout);

/**
 * \brief Probes the given cable.
 * \param handle the handle to be probed.
 * \return 0 if the operation succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_cable_probe(cable_handle * handle);
/**
 * \brief Opens the given cable.
 * \param handle the handle to be opened.
 * \return 0 if the operation succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_cable_open(cable_handle * handle);
/**
 * \brief Closes the given cable.
 * \param handle the handle to be closed.
 * \return 0 if the operation succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_cable_close(cable_handle * handle);
/**
 * \brief Sends data through the given cable.
 * \param handle the cable handle.
 * \param data the data to be sent.
 * \param len the size of the data to be sent.
 * \return 0 if the operation succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_cable_send(cable_handle * handle, uint8_t * data, uint32_t len);
/**
 * \brief Receives data through the given cable.
 * \param handle the cable handle.
 * \param data storage area for the data to be received.
 * \param len storage area for the length of the received data.
 * \return 0 if the operation succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_cable_recv(cable_handle * handle, uint8_t ** data, uint32_t * len);

/**
 * \brief Detects usable cables and builds an array of uint8_t booleans corresponding to the items of enum cable_model.
 * \param result storage area for the cable models which were found. Use \a hpcables_probe_free to free the allocated memory.
 * \return the number of usable cables. 0 means severe problem (e.g. memory allocation failure), as the null cable is always usable.
 */
HPEXPORT int HPCALL hpcables_probe_cables(uint8_t ** result);
/**
 * \brief Frees the result of probing, created by \a hpcables_probe_cables.
 * \param models the memory to be freed.
 * \return 0 if the operation succeeded, nonzero otherwise.
 */
HPEXPORT int HPCALL hpcables_probe_free(uint8_t * models);
/**
 * \brief Shows basic information about a probing result.
 * \param models the probing information to be dumped.
 * \return 0 if the operation succeeded, nonzero otherwise.
 **/
HPEXPORT int HPCALL hpcables_probe_display(uint8_t * models);



/**
 * \brief Converts a calculator model to a printable string.
 * \param model the calculator model.
 * \return the string corresponding to the calculator model.
 **/
HPEXPORT const char * HPCALL hpcables_model_to_string(cable_model model);
/**
 * \brief Converts a string to a supported calculator model, if possible.
 * \param str the string.
 * \return the calculator model corresponding to the string, CALC_NONE if failed.
 **/
HPEXPORT cable_model HPCALL hpcables_string_to_model(const char *str);

#ifdef __cplusplus
}
#endif

#endif
