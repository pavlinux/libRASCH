/**
 * \file ra_endian.h
 *
 * The header file contains macros handling endian issues.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2002-2006, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef _RA_ENDIAN_H
#define _RA_ENDIAN_H

#ifdef __cplusplus
extern "C" {
#endif

extern int always_return_zero();

#define RA_LITTLE_ENDIAN  1
#define RA_BIG_ENDIAN     2

#ifdef WIN32
#define _swap16(x,l)                                \
  __pragma(warning(push));                          \
  __pragma(warning(disable:4127));                  \
  do {                                              \
    short endian_t = x;                             \
    int endian_type = get_endian_type();            \
                                                    \
    if (l && (endian_type == RA_LITTLE_ENDIAN))     \
         break;                                     \
    if (!l && (endian_type == RA_BIG_ENDIAN))       \
         break;                                     \
                                                    \
    x = ((endian_t & 0x00ff) << 8) |                \
        ((endian_t & 0xff00) >> 8);                 \
  } while(0)                                        \
  __pragma(warning(pop))
#else
#define _swap16(x,l)                                \
  do {                                              \
    short endian_t = x;                             \
    int endian_type = get_endian_type();            \
                                                    \
    if (l && (endian_type == RA_LITTLE_ENDIAN))     \
         break;                                     \
    if (!l && (endian_type == RA_BIG_ENDIAN))       \
         break;                                     \
                                                    \
    x = ((endian_t & 0x00ff) << 8) |                \
        ((endian_t & 0xff00) >> 8);                 \
  } while(0)
#endif


#ifdef WIN32
#define _swap32(x,l)                                \
  __pragma(warning(push));                          \
  __pragma(warning(disable:4127));                  \
  do {                                              \
    long endian_t = x;                              \
    int endian_type = get_endian_type();            \
                                                    \
    if (l && (endian_type == RA_LITTLE_ENDIAN))     \
         break;                                     \
    if (!l && (endian_type == RA_BIG_ENDIAN))       \
         break;                                     \
                                                    \
    x = ((endian_t & 0x000000ff) << 24) |           \
        ((endian_t & 0x0000ff00) << 8)  |           \
        ((endian_t & 0x00ff0000) >> 8)  |           \
        ((endian_t & 0xff000000) >> 24);            \
  } while(0)                                        \
  __pragma(warning(pop))
#else
#define _swap32(x,l)                                \
  do {                                              \
    long endian_t = x;                              \
    int endian_type = get_endian_type();            \
                                                    \
    if (l && (endian_type == RA_LITTLE_ENDIAN))     \
         break;                                     \
    if (!l && (endian_type == RA_BIG_ENDIAN))       \
         break;                                     \
                                                    \
    x = ((endian_t & 0x000000ff) << 24) |           \
        ((endian_t & 0x0000ff00) << 8)  |           \
        ((endian_t & 0x00ff0000) >> 8)  |           \
        ((endian_t & 0xff000000) >> 24);            \
  } while(0)
#endif



#define be16_to_cpu(x) _swap16(x,0)
#define be32_to_cpu(x) _swap32(x,0)
#define cpu_to_be16(x) _swap16(x,0)
#define cpu_to_be32(x) _swap32(x,0)

#define le16_to_cpu(x) _swap16(x,1)
#define le32_to_cpu(x) _swap32(x,1)
#define cpu_to_le16(x) _swap16(x,1)
#define cpu_to_le32(x) _swap32(x,1)


#ifdef __cplusplus
}
#endif

#endif /* _RA_ENDIAN_H */
