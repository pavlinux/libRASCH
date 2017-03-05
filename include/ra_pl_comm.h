/**
 * \file ra_pl_comm.h
 *
 * Implements a Qt like signal/slot architecture for communication between
 * libRASCH plugins.
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

#ifndef _RA_PL_COMM_H
#define _RA_PL_COMM_H

#include <ra.h>

#ifdef __cplusplus
extern "C" {
#endif


#define MAX_SIGNAL_NAME  250
typedef void *dest_handle;

/**
 * \struct slot_info
 * \brief contains infos for a slot
 * \param <plugin> plugin handle
 * \param <dest> destination handle
 */
struct slot_info
{
	plugin_handle plugin;
	dest_handle dest;
}; /* struct slot_info */


/* FIXME: use hash to find signal */
/**
 * \struct signal
 * \brief contains infos for a signal
 * \param <name> name of the signal
 * \param <num_slots> number of registered slots (destinations)
 * \param <slot> registered slots
 */
struct signal
{
	char name[MAX_SIGNAL_NAME];

	int num_slots;
	struct slot_info *slot;	/* variable-name 'slots' doesn't work with Qt */
}; /* struct signal */


/**
 * \struct plugin_comm
 * \brief contains all available signals
 * \param <num_signals> number of registered signals
 * \param <sig> infos about each signal
 */
struct plugin_comm
{
	int num_signals;
	struct signal *sig;
}; /* struct plugin_comm */


#define PARA_CHAR                         0
#define PARA_BYTE                         1
#define PARA_SHORT                        2
#define PARA_UNSIGNED_SHORT               3
#define PARA_INT                          4
#define PARA_UNSIGNED_INT                 5
#define PARA_LONG                         6
#define PARA_UNSIGNED_LONG                7
#define PARA_FLOAT                        8
#define PARA_DOUBLE                       9
#define PARA_CHAR_POINTER                10
#define PARA_BYTE_POINTER                11
#define PARA_SHORT_POINTER               12
#define PARA_UNSIGNED_SHORT_POINTER      13
#define PARA_INT_POINTER                 14
#define PARA_UNSIGNED_INT_POINTER        15
#define PARA_LONG_POINTER                16
#define PARA_UNSIGNED_LONG_POINTER       17
#define PARA_FLOAT_POINTER               18
#define PARA_DOUBLE_POINTER              19
#define PARA_VOID_POINTER                20

/* TODO: think if struct comm_para can be combined with struct ra_info or struct ra_info use
   this struct and if it's useful to add parameter description */
struct comm_para
{
	int type;		/* see defines PARA_* above */
	union
	{
		char c;
		unsigned char b;
		short int sn;
		unsigned short int usn;
		int n;
		unsigned int un;
		long l;
		unsigned long ul;
		float f;
		double d;

		char *pc;
		unsigned char *pb;
		short int *psn;
		unsigned short int *pusn;
		int *pn;
		unsigned int *pun;
		long *pl;
		unsigned long *pul;
		float *pf;
		double *pd;
		void *pv;
	}
	v;
}; /* struct comm_para */


/* interface used by plugins */
/* TODO: think about if these protoypes should be moved to ra.h */
LIBRAAPI int ra_comm_add(meas_handle mh, plugin_handle p, dest_handle dh, const char *sig_name);
LIBRAAPI void ra_comm_del(meas_handle mh, dest_handle dh, const char *sig_name);
LIBRAAPI void ra_comm_emit(meas_handle mh, const char *sender, const char *sig_name, int num_para, struct comm_para *p);

/* internal functions */
struct signal *find_signal(struct plugin_comm *plc, const char *sig_name);
struct signal *add_signal(struct plugin_comm *plc, const char *sig_name);
int find_slot(struct signal *sig, dest_handle dh);

#ifdef __cplusplus
}
#endif

#endif /* _RA_PL_COMM_H */
