/**
 * \file ra_error.h
 *
 * The file contains defines contstans of libRASCH error codes.
 *
 * \author Raphael Schneider (rasch@med1.med.tum.de)
 */

/*----------------------------------------------------------------------------
 *
 * Copyright (C) 2003-2007, Raphael Schneider
 * See the file COPYING for information on usage and redistribution.
 *
 * $Id$
 *
 *--------------------------------------------------------------------------*/

#ifndef RA_ERROR_H
#define RA_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

#define RA_ERR_NONE                     1
#define RA_ERR_ERROR                    2
#define RA_ERR_READ_CONFIG              3
#define RA_ERR_LOAD_PLUGINS             4
#define RA_ERR_UNSUP_FORMAT             5
#define RA_ERR_UNKNOWN_PLUGIN           6
#define RA_ERR_PL_API_NOT_AVAIL         7
#define RA_ERR_UNKNOWN_HANDLE           8
#define RA_ERR_UNKNOWN_INFO             9
#define RA_ERR_OUT_OF_RANGE            10
#define RA_ERR_UNSUPPORTED             11
#define RA_ERR_WRONG_HANDLE            12
#define RA_ERR_READ_EVAL               13
#define RA_ERR_WRONG_EVENT_TYPE        14
#define RA_ERR_UNKNOWN_EV_PROP         15
#define RA_ERR_UNKNOWN_EV_SET          16
#define RA_ERR_WRONG_EV_PROP           17
#define RA_ERR_TEMPL_NOT_FOUND         18
#define RA_ERR_EVAL_FILE_CORRUPT       19
#define RA_ERR_PLUGIN_MISSING          20
#define RA_ERR_MEAS_CORRUPT            21
#define RA_ERR_INFO_MISSING            22
#define RA_ERR_EV_PROP_MISSING         23
#define RA_ERR_OPTION_UNKNOWN          24
#define RA_ERR_SIGNAL_NOT_AVAIL        25
#define RA_ERR_OPTION_NO_OFFSET        26
#define RA_ERR_ERROR_INTERNAL          27
#define RA_ERR_INTERNAL_PLUGIN_ERROR   28
#define RA_ERR_WRONG_INPUT             29
#define RA_ERR_ALGORITHM               30
#define RA_ERR_SAVE_FILE               31
#define RA_ERR_MEAS_MISSING_INFO       32
#define RA_ERR_EVAL_WRONG_TYPE         33
#define RA_ERR_EVAL_FILE_WRONG_VERSION 34
#define RA_ERR_DATA_NOT_AVAIL_FAST     35


#define RA_WARNING                    999
#define RA_WARN_NO_DATA              1000
#define RA_WARN_NO_CONFIG_FILE       1001
#define RA_WARN_NO_DEFAULT_EVAL      1002
#define RA_WARN_NOT_ALL_PROCESSED    1003
#define RA_WARN_CH_INDEP_PROP_VALUE  1004

#ifdef __cplusplus
}
#endif

#endif /* RA_ERROR_H */
