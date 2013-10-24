/* libhpfiles, libhpcables, libhpcalcs - hand-helds support library
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
 * \file export.h Stuff for function export and calling convention.
 */

#ifndef __HPCALCS_EXPORT__
#define __HPCALCS_EXPORT__

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* 
	Calling convention: default 
*/
# define HPCALL

/* 
	Symbols exporting 
*/
#if defined(HAVE_FVISIBILITY)	// GCC 4.0 has introduced the -fvisibility flag (similar to declspec)
# define HPEXPORT __attribute__ ((visibility("default")))

#elif defined(__WIN32__)
# if defined(__BORLANDC__)		// BCC32 v5.x (or C++Builder)
#  if __BORLANDC__ >= 0x0500	// (c) 2001 Thomas Wolf (two@chello.at)
#   define HPEXPORT
#  else
#   define HPEXPORT
#  endif

# elif defined(_MSC_VER)		// MSVC 5.0 mini
#  if defined(HPCALCS_EXPORTS)
#   define HPEXPORT __declspec(dllexport)
#  else
#   define HPEXPORT __declspec(dllimport)
#  endif

# elif defined(__MINGW32__)		// MinGW - GCC for Windows, (c) 2002 Kevin Kofler
#  if defined(HPCALCS_EXPORTS)	// defined by the configure script
#   define HPEXPORT __declspec(dllexport)
#  else
#   define HPEXPORT __declspec(dllimport)
#  endif
# endif

#else
# define HPEXPORT				// default
#endif

#ifdef __cplusplus
}
#endif

/* 
	Symbols deprecating
*/
#ifndef TILIBS_DEPRECATED
# ifdef __GNUC__
#  if (__GNUC__>3) || (__GNUC__==3 && __GNUC_MINOR__>=3)
#   define TILIBS_DEPRECATED __attribute__((deprecated))
#  else /* not GCC >= 3.3 */
#   define TILIBS_DEPRECATED
#  endif /* GCC >= 3.3 */
# else /* not __GNUC__ */
#  ifdef _MSC_VER
#   if _MSC_VER >= 1300
#    define TILIBS_DEPRECATED __declspec(deprecated)
#   else /* not _MSC_VER >= 1300 */
#    define TILIBS_DEPRECATED
#   endif /* _MSC_VER >= 1300 */
#  else /* not _MSC_VER */
#   define TILIBS_DEPRECATED
#  endif /* _MSC_VER */
# endif /* __GNUC__ */
#endif /* TILIBS_DEPRECATED */

#endif
