/*
 * libhpfiles: hand-helds support libraries.
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
 * \file hpfiles.h Files: base part.
 */

#ifndef __HPLIBS_FILES_H__
#define __HPLIBS_FILES_H__

#include <stdint.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

// As of 2013/11, too many environments still don't support <uchar.h>.
// Since the only thing from this header libhpfiles uses (for now) is char16_t:
// * on modern GCC and Clang (the main targets of this code base), use the builtin __CHAR16_TYPE__ preprocessor define;
// * otherwise, fall back to uint16_t.
#ifdef __CHAR16_TYPE__
typedef __CHAR16_TYPE__ char16_t;
#else
typedef uint16_t char16_t;
#endif

#include "hplibs.h"
#include "filetypes.h"

//! Maximum length of a variable name for the calculators supported by libhpfiles.
// Not sure the target calculators even support such long filenames...
#define FILES_VARNAME_MAXLEN 128

//! Generic structure for storing the contents of a variable, or requesting that a variable be sent. 
typedef struct
{
    char16_t name[FILES_VARNAME_MAXLEN+1];

    uint8_t type;
    uint8_t model;
    uint8_t invalid; ///< Set to nonzero by e.g. hpcalcs_calc_recv_file() if a packet loss was detected.
    uint32_t size;
    uint8_t* data;
} files_var_entry;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief Initializes library internals. Must be called before any other libhpfiles function.
 * \param log_callback callback function for receiving logging output.
 * \return Whether the initialization succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpfiles_init(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Tears down library internals. No other libhpfiles function can be called after this one.
 * \return Whether the teardown succeeded.
 * \todo return instance count instead.
 **/
HPEXPORT int HPCALL hpfiles_exit(void);

/**
 * \brief Returns the library version string.
 * \return The library version string, usually under the form "X.Y.Z".
 **/
HPEXPORT const char* HPCALL hpfiles_version_get(void);

/**
 * \brief Gets the error message if the error was produced by this library
 * \param number the error number (from internal error.h)
 * \param message out pointer for a newly allocated text error message, which must be freed by the caller
 * \return 0 if the error was produced by this library, otherwise the error number (for propagation).
 **/
HPEXPORT int HPCALL hpfiles_error_get(int number, char **message);

/**
 * \brief Sets the callback function used by the library for logging
 * \param log_callback function pointer
 */
HPEXPORT void HPCALL hpfiles_log_set_callback(void (*log_callback)(const char *format, va_list args));
/**
 * \brief Sets the log level of the library, for controlling how much logging output is produced.
 * \param log_level log level (from hplibs.h)
 * \return the previous log level
 */
HPEXPORT hplibs_logging_level HPCALL hpfiles_log_set_level(hplibs_logging_level log_level);


/**
 * \brief Creates an empty files_var_entry structure.
 * \return Pointer to files_var_entry, NULL if failed.
 */
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create(void);
/**
 * \brief Creates and preallocates a files_var_entry structure.
 * \param size the size to be allocated
 * \return Pointer to files_var_entry, NULL if failed.
 */
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_size(uint32_t size);
/**
 * \brief Creates and fills a files_var_entry structure with the given data.
 * \param data the data to be copied
 * \param size the size of the data.
 * \return Pointer to files_var_entry, NULL if failed.
 */
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data(uint8_t * data, uint32_t size);
/**
 * \brief Creates and fills a files_var_entry structure with the given data.
 * \param data the data to be copied
 * \param size the size of the data.
 * \param name the file name on the calculator side.
 * \return Pointer to files_var_entry, NULL if failed.
 */
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_with_data_and_name(uint8_t * data, uint32_t size, const char16_t * name);
/**
 * \brief Creates and fills a files_var_entry structure from the given file, if it exists.
 * \param file the FILE pointer to be used for filling in a files_var_entry instance.
 * \param filename the UTF-16LE name of the file on the calculator side, can be NULL if you want to set it later.
 * \return Pointer to files_var_entry, NULL if failed.
 */
HPEXPORT files_var_entry * HPCALL hpfiles_ve_create_from_file(FILE * file, const char16_t * filename);
/**
 * \brief Destroys the given files_var_entry instance (embedded data + the entry itself).
 * \param entry the entry
 */
HPEXPORT void HPCALL hpfiles_ve_delete(files_var_entry * entry);

/**
 * \brief Allocates data for the \a data field of files_var_entry.
 * \param size the size to be allocated
 * \return Pointer to allocated data, NULL if failed.
 */
HPEXPORT void * HPCALL hpfiles_ve_alloc_data(uint32_t size);
/**
 * \brief Copies files_var_entry and its content (if any) from pre-existing src to pre-existing dst.
 * \param dst destination entry.
 * \param src source entry.
 * \return dst, NULL if failed.
 **/
HPEXPORT files_var_entry * HPCALL hpfiles_ve_copy(files_var_entry * dst, files_var_entry * src);
/**
 * \brief Duplicates files_var_entry and its content (if any) by creating a new instance.
 * \param src source entry
 * \return The new entry, NULL if failed.
 **/
HPEXPORT files_var_entry * HPCALL hpfiles_ve_dup(files_var_entry * src);

/**
 * \brief Shows basic information about a variable entry
 * \param entry the entry to be dumped.
 * \return 0 if the handle was non-NULL, nonzero otherwise.
 **/
HPEXPORT int hpfiles_ve_display(files_var_entry * entry);

/**
 * \brief Creates a NULL-terminated array for storing the given number of potentially non-NULL files_var_entry pointers.
 * \param element_count the number of pointers
 * \return The new array containing element_count + 1 NULL pointers, NULL if failed.
 **/
HPEXPORT files_var_entry ** HPCALL hpfiles_ve_create_array(uint32_t element_count);
/**
 * \brief Reallocates a NULL-terminated array for storing a new number of potentially non-NULL files_var_entry pointers.
 * \param entries the array of entries.
 * \param element_count the new number of pointers
 * \return The new array containing element_count + 1 pointers, NULL if failed.
 * \note If the array was enlarged, the new storage space isn't cleared.
 **/
HPEXPORT files_var_entry ** HPCALL hpfiles_ve_resize_array(files_var_entry ** entries, uint32_t element_count);
/**
 * \brief Destroys the whole array of files_var_entry structs (including the structures themselves and their embedded data).
 * \param entries the array to be destroyed.
 **/
HPEXPORT void HPCALL hpfiles_ve_delete_array(files_var_entry ** entries);


/**
 * \brief Converts a calculator model to a printable string.
 * \param model the calculator model.
 * \return the string corresponding to the calculator model.
 **/
HPEXPORT const char * HPCALL hpfiles_model_to_string(calc_model model);
/**
 * \brief Converts a string to a supported calculator model, if possible.
 * \param string the string.
 * \return the calculator model corresponding to the string, CALC_NONE if failed.
 **/
HPEXPORT calc_model HPCALL hpfiles_string_to_model(const char *str);

/**
 * \brief Converts a file type ID to a printable file type string, according to the given calculator model.
 * \param model the calculator model
 * \param type the file type ID
 * \return the file type string corresponding to the given type ID, if any.
 */
HPEXPORT const char * HPCALL hpfiles_vartype2str(calc_model model, uint8_t type);
/**
 * \brief Converts a file type (under string form) to a file type ID, according to the given calculator model.
 * \param model the calculator model
 * \param type the file type string
 * \return the file type ID corresponding to the given type string, if any.
 */
HPEXPORT uint8_t HPCALL hpfiles_str2vartype(calc_model model, const char * type);
/**
 * \brief Converts a file type to an extension, according to the given calculator model.
 * \param model the calculator model
 * \param type the file type ID
 * \return the file extension corresponding to the given type, if any.
 * \note may have to use char16_t instead of char...
 */
HPEXPORT const char * HPCALL hpfiles_vartype2fext(calc_model model, uint8_t type);
/**
 * \brief Converts a file extension to a file type ID, according to the given calculator model.
 * \param model the calculator model
 * \param type the file type as string.
 * \return the file type ID corresponding to the given extension, if any.
 */
HPEXPORT uint8_t HPCALL hpfiles_fext2vartype(calc_model model, const char * type);
/**
 * \brief Converts a file path to a file type ID, according to the given calculator model.
 * \param model the calculator model
 * \param filepath the file path as string.
 * \return the file type ID corresponding to the given file, if any.
 */
HPEXPORT uint8_t HPCALL hpfiles_filename2vartype(calc_model model, const char * filepath);
/**
 * \brief Interprets a file path to a file type ID + calculator-side file name, according to the given calculator model.
 * \param model the calculator model
 * \param filepath the file path as string.
 * \param out_type storage space for the file type ID corresponding to the given file, if any.
 * \param calcfilename a dynamically allocated string containing the calculator-side filename (usually stripped from the computer-side file extension).
 * \return 0 if parsing succeeded, nonzero otherwise.
 * \note may have to use char16_t instead of char...
 */
HPEXPORT int HPCALL hpfiles_parsefilename(calc_model model, const char * filepath, uint8_t * out_type, char ** out_calcfilename);

#ifdef __cplusplus
}
#endif

#endif
