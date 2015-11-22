/*
 * libhpopers: hand-helds support libraries.
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
 * \file hpopers.h Higher-level operations: base part.
 */

#ifndef __HPLIBS_OPERS_H__
#define __HPLIBS_OPERS_H__

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#include "hplibs.h"
#include "hpfiles.h"
#include "hpcables.h"
#include "hpcalcs.h"


//! Structure passed to \a hpopers_init, contains e.g. callbacks for logging and memory allocation.
typedef struct {
    unsigned int version; ///< Config version number.
    void (*log_callback)(const char *format, va_list args); ///< Callback function for receiving logging output.
    hplibs_malloc_funcs * alloc_funcs; ///< Function pointers used for dynamic memory allocation. If NULL, the library defaults to malloc(), calloc(), realloc(), free().
} hpopers_config;

//! Latest revision of the \a hpopers_config struct layout supported by this version of the library.
#define HPOPERS_CONFIG_VERSION (1)


#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes library internals. Must be called before any other libhpopers function.
 * \param config pointer to struct containing e.g. callbacks passed to the library.
 * \return Whether the initialization succeeded.
 * \note the contents of alloc_funcs are copied.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpopers_init(hpopers_config * config);
/**
 * \brief Tears down library internals. No other libhpopers function can be called after this one.
 * \return Whether the teardown succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpopers_exit(void);

/**
 * \brief Returns the library version string.
 * \return The library version string, usually under the form "X.Y.Z".
 **/
HPEXPORT const char* HPCALL hpopers_version_get(void);

/**
 * \brief Gets the error message if the error was produced by this library
 * \param number the error number (from internal error.h)
 * \param message out pointer for a newly allocated text error message, which must be freed by the caller
 * \note the string is allocated with malloc(), therefore it must be freed with free().
 * \return 0 if the error was produced by this library, otherwise the error number (for propagation).
 **/
HPEXPORT int HPCALL hpopers_error_get(int number, char **message);

/**
 * \brief Sets the callback function used by the library for logging
 * \param log_callback function pointer
 */
HPEXPORT void HPCALL hpopers_log_set_callback(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Sets the log level of the library, for controlling how much logging output is produced.
 * \param log_level log level (from hplibs.h)
 * \return the previous log level
 */
HPEXPORT hplibs_logging_level HPCALL hpopers_log_set_level(hplibs_logging_level log_level);


// Tentative APIs, may change in the near future.

/**
 * \brief Retrieves a screenshot from the calculator, without any form of conversion
 * \param handle the calculator handle.
 * \param format the desired screenshot format.
 * \param out_file struct FILE pointer for storing the output.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpcalcs_calc_recv_screen .
 */
HPEXPORT int HPCALL hpopers_oper_recv_screen_verbatim_to_file(calc_handle * handle, calc_screenshot_format format, FILE * out_file);
/**
 * \brief Retrieves a screenshot from the calculator, and convert it to R8G8B8 before recompressing it as PNG.
 * \param handle the calculator handle.
 * \param format the desired screenshot format.
 * \param out_data storage area for converted R8G8B8 screenshot.
 * \param out_size storage area for size of the R8G8B8 screenshot contained in the calculator's reply.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpcalcs_calc_recv_screen and \a hpopers_oper_convert_raw_screen_to_png_r8g8b8.
 */
HPEXPORT int HPCALL hpopers_oper_recv_screen_png_r8g8b8(calc_handle * handle, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);
/**
 * \brief Retrieves a screenshot from the calculator, and convert it to R8G8B8 before recompressing it as PNG and writing it to a file.
 * \param handle the calculator handle.
 * \param format the desired screenshot format.
 * \param out_file struct FILE pointer for storing the output.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpopers_oper_recv_screen_png_r8g8b8 .
 */
HPEXPORT int HPCALL hpopers_oper_recv_screen_png_r8g8b8_to_file(calc_handle * handle, calc_screenshot_format format, FILE * out_file);
/**
 * \brief Converts a raw screenshot (output by e.g. \a hpcalcs_calc_recv_screen) to a more usual PNG R8G8B8 format.
 * \param in_data storage area for raw screenshot.
 * \param in_size storage area for size of the raw screenshot.
 * \param format the desired screenshot format.
 * \param out_data storage area for R8G8B8 screenshot.
 * \param out_size storage area for size of the R8G8B8 screenshot.
 * \return 0 upon success, nonzero otherwise.
 */
HPEXPORT int HPCALL hpopers_oper_convert_raw_screen_to_png_r8g8b8(uint8_t * in_data, uint32_t in_size, calc_screenshot_format format, uint8_t ** out_data, uint32_t * out_size);
/**
 * \brief Sends a file to the calculator.
 * \param handle the calculator handle.
 * \param file struct FILE pointer containing the contents to be sent.
 * \param filename struct FILE pointer containing the contents to be sent.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpcalcs_calc_send_file .
 */
HPEXPORT int HPCALL hpopers_calc_send_file(calc_handle * handle, FILE * file, const char16_t * filename);
/**
 * \brief Receives a file from the calculator.
 * \param handle the calculator handle.
 * \param calculator_filename name (on the calculator side) of the file to be received.
 * \param file struct FILE pointer used for writing data.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpcalcs_calc_recv_file .
 */
HPEXPORT int HPCALL hpopers_calc_recv_file(calc_handle * handle, const char16_t * calculator_filename, FILE * file);
/**
 * \brief Receives a backup (made of multiple files) from the calculator to a folder.
 * \param handle the calculator handle.
 * \param out_path name of the folder used as root for the files hierarchy.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpcalcs_calc_recv_backup .
 */
HPEXPORT int HPCALL hpopers_calc_recv_backup(calc_handle * handle, const char * out_path);
/**
 * \brief Sends a backup (made of multiple files) from a folder to the calculator.
 * \param handle the calculator handle.
 * \param in_path name of the folder used as root for the files hierarchy.
 * \return 0 upon success, nonzero otherwise.
 * \note This shall be a wrapper over \a hpcalcs_calc_send_file (called in a loop) .
 */
HPEXPORT int HPCALL hpopers_calc_send_backup(calc_handle * handle, const char * in_path);


#ifdef __cplusplus
}
#endif

#endif

